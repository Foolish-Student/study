# 第二篇：C++ 程序接入 Sentry，从第一条消息到可读崩溃堆栈

> 前置：[基础与原理](01-Sentry是什么与工作原理.md)。本文用 CMake + sentry-native + Crashpad 演示普通桌面程序，Windows 为主要示例，并补充 Linux/macOS 差异。CEF 程序请继续看第三篇，不能直接把这里的崩溃初始化套进所有 CEF 进程。
>
> 核对日期：2026-10-18。下列示例需在你的构建环境中编译验证；本文未连接你的 Sentry 项目做实际上传。

## 1. 先准备四样东西

1. 一个 Sentry Native 项目，以及项目提供的 DSN。
2. CMake、受所选 SDK 版本支持的 C++ 编译器和 Git。
3. 固定版本的 sentry-native 源码或官方构建产物。
4. 一个可写的本地数据目录，以及到 Sentry 接收端的网络连接。

示例明确选择 Crashpad，便于理解 handler 的部署。SDK 的平台最低要求会变化，先检查所选版本的 [官方 README](https://github.com/getsentry/sentry-native)。Linux/macOS 的网络传输通常还需要 libcurl 开发依赖，Crashpad 构建还涉及 zlib 等依赖。

## 2. 创建最小项目

```text
sentry-demo/
├── CMakeLists.txt
├── main.cpp
└── third_party/
    └── sentry-native/
```

在 `sentry-demo` 目录执行：

```bash
# 获取 SDK 源码及其依赖子模块，目标目录为 third_party/sentry-native。
git clone --recurse-submodules https://github.com/getsentry/sentry-native.git third_party/sentry-native
```

第一次获取源码后，选择一个经过团队验证的稳定 tag，并把它固定下来：

```bash
# 切换到指定稳定 tag；先把下面的占位符替换成真实 tag，再执行。
git -C third_party/sentry-native checkout <经过验证的稳定tag>
# 将依赖子模块同步到该 SDK 版本要求的提交。
git -C third_party/sentry-native submodule update --init --recursive
```

`<经过验证的稳定tag>` 是要替换的占位符。正式项目应通过子模块或依赖管理固定版本，不要让每次构建都跟随 master。

### CMakeLists.txt

```cmake
# 声明本示例最低 CMake 版本；SDK 所选版本若要求更高，应同步提高。
cmake_minimum_required(VERSION 3.20)
# 启用 C/C++，因为应用和 sentry-native 源码会用到这些语言。
project(sentry_demo LANGUAGES C CXX)

# 要求 C++17，以使用 filesystem，并满足此示例的编译需求。
set(CMAKE_CXX_STANDARD 17)
# 编译器不支持要求的标准时直接报错，避免静默降级。
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 明确选择 Crashpad 原生崩溃后端；FORCE 覆盖已有同名缓存值。
set(SENTRY_BACKEND "crashpad" CACHE STRING "" FORCE)
# 构建 Sentry 动态库；运行和打包时也要携带它。
set(SENTRY_BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
# 不编译 SDK 自带测试，缩小本入门项目的构建范围。
set(SENTRY_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# 不编译 SDK 示例，下面使用自己的 sentry_demo。
set(SENTRY_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# 将 SDK 源码加入构建，获得 sentry 与 crashpad_handler 等 target。
add_subdirectory(third_party/sentry-native)

# 从 main.cpp 构建演示应用。
add_executable(sentry_demo main.cpp)
# 链接 SDK target，同时取得它公开的头文件路径等构建要求。
target_link_libraries(sentry_demo PRIVATE sentry)

# 以下调试信息选项仅用于 MSVC 编译环境。
if(MSVC)
    # 为业务应用生成调试信息；不是仅为 SDK 生成符号。
    target_compile_options(sentry_demo PRIVATE /Zi)
    # 要求链接器生成应用 PDB，后续与对应构建一起归档上传。
    target_link_options(sentry_demo PRIVATE /DEBUG)
# 结束 MSVC 专用配置。
endif()

# 为演示程序准备相邻的动态库与 handler。
# 应用构建成功后执行复制步骤，准备运行目录。
add_custom_command(TARGET sentry_demo POST_BUILD
    # 调用 CMake 的文件复制工具；内容相同则无需再次复制。
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        # 把 SDK 动态库复制到应用目录；生成器表达式按实际构建配置解析。
        $<TARGET_FILE:sentry> $<TARGET_FILE_DIR:sentry_demo>
    # 调用 CMake 的文件复制工具；内容相同则无需再次复制。
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        # 把独立崩溃处理程序复制到应用目录，程序将通过路径找到它。
        $<TARGET_FILE:crashpad_handler> $<TARGET_FILE_DIR:sentry_demo>
    # 让 CMake 正确转义命令参数，减少路径包含空格时的问题。
    VERBATIM
# 结束这条构建后命令。
)
# 确保 handler 先构建完成，避免复制时源文件尚不存在。
add_dependencies(sentry_demo crashpad_handler)
```

这是源码作为子目录集成的示例，使用 `sentry` target。若改用安装包的 `find_package` 方式，应按那个包导出的 target 和配置编写，不要把不同集成方式混合。

### main.cpp

下面把配置、初始化、上下文、发送与关闭放在同一份程序中。Sentry 相关代码逐行标注；特别留意三点：`options` 在初始化时交给 SDK 管理，事件和 breadcrumb 也有所有权转移；`capture` 不等于送达；崩溃分支不会走到普通发送与关闭代码。

```cpp
#include <sentry.h> // 引入 sentry-native 的 C API，供 C++ 调用；SDK 文本字段通常使用 UTF-8。
#include <cstdlib> // 提供 getenv 读取环境变量，以及 abort 触发测试崩溃。
#include <filesystem> // 提供路径检查与本地数据库目录创建能力。
#include <iostream> // 将初始化失败、超时等诊断信息输出到终端。
#include <string> // 使用字符串保存 message/crash 测试模式。

int main(int argc, char** argv) {
    // 用显式路径避免依赖启动时的工作目录。
    if (argc < 3) {
        std::cerr << "Usage: sentry_demo <absolute-handler-path> "
                     "<absolute-database-path> [message|crash]\n";
        return 1;
    }
    const char* dsn = std::getenv("SENTRY_DSN"); // 读取项目接收配置；这里是本示例主动读取环境变量，再传给 SDK。
    if (!dsn || !*dsn) {
        std::cerr << "Set SENTRY_DSN first\n";
        return 1;
    }
    const std::filesystem::path handler(argv[1]); // 第一个参数：独立 crashpad_handler 的绝对路径。
    const std::filesystem::path database(argv[2]); // 第二个参数：本地报告数据库路径，不是 Sentry 服务端数据库地址。
    if (!handler.is_absolute() || !database.is_absolute() ||
        !std::filesystem::is_regular_file(handler)) {
        std::cerr << "Invalid paths\n";
        return 1;
    }
    std::filesystem::create_directories(database); // 创建可写的报告存储目录；创建失败可能抛出文件系统异常。

    sentry_options_t* options = sentry_options_new(); // 创建初始化配置对象；配置在 sentry_init 之前填写。
    sentry_options_set_dsn(options, dsn); // 设置项目 DSN，让 SDK 能生成对应的事件接收地址。
    sentry_options_set_release(options, "desktop-demo@1.0.0"); // 设置应用版本标识；不会代替符号文件的精确构建匹配。
    sentry_options_set_environment(options, "development"); // 标记为开发环境，方便与 production 的事件分开筛选。
    sentry_options_set_debug(options, 1); // 开启 SDK 自身的调试日志以排查接入问题；生产验收后设为 0。
#ifdef _WIN32 // Windows 分支：filesystem::path 使用 wchar_t 路径，调用宽字符 API。
    sentry_options_set_handler_pathw(options, handler.c_str()); // 设置 Windows handler 路径；SDK 将按所选后端启动或配置采集进程。
    sentry_options_set_database_pathw(options, database.c_str()); // 设置 Windows 本地报告目录；应可写并能跨应用重启保留。
#else // 非 Windows 分支：使用该平台的窄字符文件系统路径。
    sentry_options_set_handler_path(options, handler.c_str()); // 设置 Crashpad handler 路径；这里只配置位置，初始化时才建立后端。
    sentry_options_set_database_path(options, database.c_str()); // 设置本地报告目录，用于后端报告和 SDK 运行数据。
#endif // 结束平台路径 API 的条件编译。
    // sentry_init 接管 options；失败后也不要再次释放 options。
    if (sentry_init(options) != 0) { // 应用配置并初始化 SDK 与后端；返回 0 表示初始化成功，不代表上传已成功。
        std::cerr << "Sentry initialization failed\n";
        return 2;
    }

    sentry_set_tag("component", "demo"); // 在当前进程的 SDK scope 设置标签；后续适用事件可携带 component=demo。
    sentry_value_t breadcrumb = sentry_value_new_breadcrumb("default", "demo started"); // 创建一条 breadcrumb：类型 default，消息 demo started；不会单独生成错误事件。
    sentry_value_set_by_key(breadcrumb, "category", sentry_value_new_string("app.lifecycle")); // 设置轨迹分类；新建的字符串值交给 breadcrumb 管理，不再自行释放它。
    sentry_add_breadcrumb(breadcrumb); // 把轨迹加入 SDK 上下文，并交出该值的所有权；不要再对 breadcrumb 调用 decref。

    const std::string mode = argc >= 4 ? argv[3] : "message"; // 第三个参数选择测试路径，未传时默认发送普通消息。
    if (mode == "crash") { // 选择原生崩溃测试分支，而不是普通 capture 消息分支。
        // 仅在测试环境执行。故意异常终止，验证崩溃采集链路。
        std::abort(); // 触发异常终止；由已建立的崩溃采集链路处理，后面的普通发送代码不会执行。
    }

    sentry_value_t event = sentry_value_new_message_event( // 创建消息事件对象，下面依次指定级别、logger 和消息。
        SENTRY_LEVEL_INFO, // 事件级别：info，用于普通消息测试；不是一次原生崩溃。
        "demo",          // logger 名称：标明消息来源，方便识别。
        "Hello from C++" // 事件消息：稍后在 Sentry 网页中查找这段文字。
    ); // 完成消息事件的构造；此时还没有交给发送流程。
    sentry_capture_event(event); // 交出事件所有权，交给 SDK 处理和发送；返回事件 ID 不等于服务端已收到。
    // capture 异步交付，flush 给普通事件上传留出时间。
    if (sentry_flush(5000) != 0) { // 最多等待普通传输队列工作 5000 毫秒；非 0 表示超时，不是网页可见性检查。
        std::cerr << "Flush did not finish within the timeout\n";
    }
    sentry_close(); // 正常关闭 SDK 并处理传输收尾；不等于等待独立 handler 的所有崩溃报告上传。
    return 0;
}
```

### 这四行 tag / breadcrumb 代码，到底有什么用

它们是在给后续错误准备“排查线索”。执行这四行时，不会因此单独创建并上传一条错误事件。

假设用户启动程序、选择文件、开始解析，然后程序崩溃。你除了想知道崩在哪里，还想知道：**这是哪个模块的问题？崩溃之前做了什么？**

- **Tag（标签）**回答“这条错误属于哪一类”，便于筛选。
- **Breadcrumb（操作轨迹）**回答“错误之前经历了哪些关键步骤”，便于还原过程。

#### 第一行：给后续事件加一个分类标签

```cpp
// 设置标签：键是 component，值是 demo。
// component 这个名字由我们选择，用它表示业务组件；demo 是演示组件名。
// 实际项目可以用 component=document、component=player 等值。
sentry_set_tag("component", "demo");
```

这相当于给后续适用事件附加一张便签：

```text
component = demo
```

以后看错误时，你就能通过这个标签区分不同组件的事件。它不是在记录“用户刚做了一次 demo 操作”，也不是创建一个叫 demo 的进程或模块。

这个设置会保留在当前 SDK 上下文中，直到被更新、移除或上下文生命周期结束。因此，多组件程序不能在启动时随便设置一个值，就期待 SDK 自动知道每次错误来自哪个组件。

#### 第二行：先创建一条“程序已启动”的轨迹对象

```cpp
// 创建一条轨迹对象，暂时由本地变量 breadcrumb 持有。
// default：轨迹类型，表示通用记录；不是“启用默认配置”的开关。
// demo started：轨迹消息，表示“演示程序已经启动”。
sentry_value_t breadcrumb = sentry_value_new_breadcrumb("default", "demo started");
```

你可以把这个对象先想成：

```text
一条尚未加入轨迹列表的记录：
  type    = default
  message = demo started
```

SDK 会为轨迹记录时间信息。这里只写了一条简短记录，没有抓取堆栈，也没有触发崩溃。

#### 用“点击打开文件”的例子理解后两行

假设用户点击了“打开文件”按钮，我们希望记录这一步。下面的代码应放在按钮点击的业务回调中执行：

```cpp
// ① 创建一条记录，填写“发生了什么”。
// default 表示通用轨迹类型；第二个参数才是记录的消息。
sentry_value_t breadcrumb = sentry_value_new_breadcrumb("default", "用户点击了打开文件");

// ② 给这一条记录填写 category（分类）字段。
// sentry_value_new_string：把文本 ui.action 包装成 SDK 使用的字符串值。
// sentry_value_set_by_key：把这个值写到 breadcrumb 对象的 category 字段。
// ui.action 是我们自己起的分类名，表示“用户界面操作”。
sentry_value_set_by_key(breadcrumb, "category", sentry_value_new_string("ui.action"));

// ③ 把填写好的记录加入 SDK 的近期轨迹列表。
// SDK 接管记录的内存管理；交出后不要再自行修改或释放 breadcrumb。
sentry_add_breadcrumb(breadcrumb);
```

**先看第②步：它只是修改一条记录，还没有把记录加入轨迹列表。**

把它拆开想，相当于执行：

```text
创建一个字符串值：ui.action
         ↓
找到 breadcrumb 对象
         ↓
把它的 category 字段填写为 ui.action
```

记录修改前后对比：

| 字段 | 执行第②步之前 | 执行第②步之后 |
| --- | --- | --- |
| message（消息） | 用户点击了打开文件 | 用户点击了打开文件 |
| category（分类） | 尚未填写 | ui.action |

如果你熟悉对象赋值，可以把第②步理解成下面的伪代码；这不是可直接编译的 Sentry API：

```text
breadcrumb["category"] = "ui.action"
```

`category` 是字段名，`ui.action` 是填进去的值。这个分类不会自动监听按钮，只有你的按钮回调实际执行上述代码，才会记录本次点击。

**再看第③步：它才把这一条记录加入 SDK 的轨迹列表。**

假设 SDK 的列表之前只有一条“程序启动”：

```text
执行 sentry_add_breadcrumb 之前：

SDK轨迹列表：
  1. 程序启动

你刚创建的记录（还没加入）：
  消息 = 用户点击了打开文件
  分类 = ui.action
```

调用后变成：

```text
执行 sentry_add_breadcrumb 之后：

SDK轨迹列表：
  1. 程序启动
  2. [ui.action] 用户点击了打开文件
```

后面如果又记录了“开始解析文件”，接着捕获一条错误事件，这条事件就可以带上这些线索：

```text
错误：解析文件失败

错误之前的轨迹：
  1. 程序启动
  2. [ui.action] 用户点击了打开文件
  3. [document] 开始解析文件
```

所以，这两个函数各做一件事：

- `sentry_value_set_by_key`：给**单条记录**填写字段。
- `sentry_add_breadcrumb`：把这条记录加入 **SDK 的近期轨迹列表**，留给后续事件使用。

回到原示例，`app.lifecycle` 与这里的 `ui.action` 作用相同，都只是分类名称：前者用来描述“程序启动”这样的生命周期记录，后者用来描述“点击按钮”这样的界面操作。

这里的“加入列表”不等于立即上传一条错误，也不会单独产生一个 Issue。列表有数量上限，并非永久保存全部操作的日志文件。

#### 真正报错时，它们怎样一起出现

假设我们又在正常业务代码中记录了“选择文件”“开始解析”，然后捕获一条错误事件。最终事件可以概念性地表示为：

```text
错误：文档解析失败

标签：
  component = document

错误之前的轨迹（时间为示意）：
  10:00:00  [app.lifecycle] 程序启动
  10:00:05  [document]      用户选择了文件
  10:00:06  [document]      开始解析
```

这样，堆栈告诉你“在哪个函数出错”，轨迹帮助你理解“之前走了什么路径”，标签帮助你筛选“哪些事件属于文档组件”。

如果之后没有发生事件，单独添加这些 breadcrumb 不会让 Sentry 的错误列表多出三条 Issue。原示例接着调用 `sentry_capture_event` 发送 `Hello from C++`，就是让你能在这条消息事件里检查标签和启动轨迹。

本文采用的 sentry-native Crashpad 集成也会按其能力将上下文用于崩溃报告；但**另外由 CEF 自带机制上传的 dump，不会自动带上你在独立 Native SDK 中设置的这些内容**，详见第三篇。

这里提到的“所有权”，只是说明谁负责释放对象内存。初学时先记住：交给 `sentry_add_breadcrumb` 后由 SDK 管理，不要再自行 `decref`。它与上传是否成功是两个问题。

这里通过命令行传入路径，是为了把路径问题明确暴露出来。生产程序应从安装位置和系统应用数据目录推导路径；不要通过不稳定的当前工作目录寻找 handler。Windows 路径使用宽字符 API；若需要支持带中文的命令行路径，还应使用 `wmain` 等方式取得原始 Unicode 参数，再转换环境变量和其他字符串。上面的窄字符 `main` 示例建议先用 ASCII 测试路径。

SDK 的一般文本字段使用 UTF-8。`sentry_capture_event`、`sentry_add_breadcrumb` 等 API 有所有权转移规则，调用后不要随意再 `decref` 已交出的值。具体签名与规则可查 [官方头文件](https://github.com/getsentry/sentry-native/blob/master/include/sentry.h)。

## 3. 构建并发送第一条消息

Windows，在 Visual Studio 开发环境中：

```powershell
# 在 build 目录生成 Windows x64 工程；需要支持 -A 的生成器，如 Visual Studio。
cmake -S . -B build -A x64
# 构建带调试信息的发布配置，以便保留对应 PDB。
cmake --build build --config RelWithDebInfo

# 仅为当前 PowerShell 会话配置示例读取的 DSN；替换为项目提供的值。
$env:SENTRY_DSN = "替换为你的项目DSN"
# 解析实际 handler 文件的绝对路径，找不到文件时会报错。
$handler = (Resolve-Path ".\build\RelWithDebInfo\crashpad_handler.exe").Path
# 选择当前用户可写的本地数据目录，避免往只读安装目录写报告。
$database = Join-Path $env:LOCALAPPDATA "SentryDemo\database"
# 传入 handler 与数据库路径，运行普通消息上报测试。
.\build\RelWithDebInfo\sentry_demo.exe $handler $database message
```

如果生成器或输出目录不同，以实际产物为准。确认运行目录里包含 `sentry.dll`、`crashpad_handler.exe` 和程序需要的其他运行库。

Linux/macOS，使用单配置生成器时：

```bash
# 用单配置生成器配置构建，并保留调试信息。
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
# 并行编译示例、SDK 及其依赖。
cmake --build build --parallel
# 为当前 shell 及其子进程设置示例读取的 DSN。
export SENTRY_DSN='替换为你的项目DSN'
# 以两个绝对路径运行普通消息测试；HOME 对应当前用户目录。
./build/sentry_demo "$PWD/build/crashpad_handler" "$HOME/.sentry-demo/database" message
```

Linux 需检查动态库搜索路径和依赖；macOS 正式应用需处理 app bundle、动态库加载路径、handler 的签名与公证。上面命令仅演示构建目录运行，不是最终安装包方案。

在 Sentry 里找到 `Hello from C++`，检查 release、environment、tag 和 breadcrumb。找不到时先看 SDK 日志，并取消页面上可能排除 development 的筛选条件。

## 4. 已捕获的异常怎样上报

在程序仍健康的正常执行路径里，可以这样记录失败：

```cpp
try {
    LoadDocument(); // 调用你的业务函数；这里仅演示接入位置，需要用真实实现替换。
} catch (const std::exception& e) { // 异常已被业务捕获，使用主动消息上报，不依赖原生崩溃处理器。
    sentry_value_t event = sentry_value_new_message_event( // 创建消息事件对象，下面依次指定级别、logger 和消息。
        SENTRY_LEVEL_ERROR, // 错误级别：error；程序仍可以继续运行。
        "document.load",   // logger 名称：标记错误来自文档加载逻辑。
        e.what()           // 消息文本：读取已捕获异常的说明，不包含原始抛出堆栈。
    ); // 构造消息事件，尚未提交上传。
    sentry_capture_event(event); // 交出事件所有权，交给 SDK 处理和发送；返回事件 ID 不等于服务端已收到。
    // 然后根据业务需要返回错误或继续处理。
}
```

这是消息事件，不会自动恢复 `throw` 当时的堆栈；它也没有为异常构造完整的结构化 exception 数据。需要异常类型、原始堆栈时，应设计额外的采集逻辑，避免把 catch 中的当前位置误当成抛出位置。

不要把这段代码放进原生信号处理函数或 SEH 崩溃回调里照搬。崩溃现场应由后端处理。

## 5. 上传符号，才能看懂崩溃

先保留这一次构建的程序和符号，再使用官方 `sentry-cli` 检查与上传：

```bash
# 在本地检查 PDB 格式、调试标识和可用信息；此命令不上传文件。
sentry-cli debug-files check /path/to/sentry_demo.pdb
# 上传指定目录的符号；-o/-p 指定组织与项目，--wait 等待服务端处理。
sentry-cli debug-files upload -o YOUR_ORG -p YOUR_PROJECT --wait /path/to/build-symbols
```

示例目录应该只放准备上传的符号产物。将具备所需权限的 `SENTRY_AUTH_TOKEN` 配置为构建机或 CI 环境变量；自建 Sentry 还需按 CLI 配置指定服务地址。`YOUR_ORG` 和 `YOUR_PROJECT` 使用组织、项目 slug。参考 [官方 CLI 文档](https://docs.sentry.io/cli/dif/)。

| 平台 | 应保留的调试产物 | 常见遗漏 |
| --- | --- | --- |
| Windows | 与 EXE/DLL 精确匹配的 PDB | 只生成库的 PDB，没生成应用 PDB |
| Linux | 带 DWARF 的 ELF 或独立 debug 文件 | strip 之前没有归档符号 |
| macOS | 与 Mach-O UUID 对应的 dSYM | CMake 构建后未实际生成 dSYM |

macOS 命令行工具可使用 `dsymutil /path/to/sentry_demo -o /path/to/sentry_demo.dSYM` 生成对应 dSYM，前提是构建时保留可用的调试信息。Linux 使用 `RelWithDebInfo` 后仍应检查真实的编译参数和产物。

需要上传业务 DLL/共享库的符号，不能只上传主 EXE。服务端显示源码上下文与基础符号化是不同层次；`--include-sources` 涉及上传源码，应按团队要求配置。

## 6. 再验证真正的崩溃

先完成符号上传，然后脱离调试器运行同一份构建，把最后一个参数改成 `crash`。例如 Windows：

```powershell
# 运行同一构建的崩溃分支；应在测试环境脱离调试器执行，预期异常退出。
.\build\RelWithDebInfo\sentry_demo.exe $handler $database crash
```

此时程序异常退出是预期结果。检查 handler、本地数据库和服务端事件，必要时重启同一程序观察后续报告处理。不要通过正常退出路径里的 `sentry_flush` 判断崩溃报告是否成功；`abort()` 后本来就不会执行那些代码。

验收至少包含：消息事件可达；崩溃事件可达；崩溃类型合理；应用帧有可识别的函数名，具备行号信息的帧能定位源码；release/environment 正确；离线后恢复网络时的行为已经验证。

## 7. 为什么本地成功，发给用户就失败

| 现象 | 优先排查 |
| --- | --- |
| 链接错误或加载 DLL 失败 | 架构、运行库、动态/静态配置和导出宏 |
| 初始化失败 | handler 路径、可执行权限、数据库权限、依赖 |
| 消息收到，崩溃收不到 | 崩溃后端部署、调试器干预、handler 启动情况 |
| 只有地址，没有函数名 | 对应模块符号缺失或标识不匹配 |
| 函数名有了，源码行没有 | 符号详细程度、优化和第三方符号限制 |
| 安装版失败，构建目录成功 | 漏打包文件、只读安装目录、签名、库搜索路径 |
| 一直没有事件 | DSN、代理/TLS、接收端响应、限流、环境筛选 |

Windows 的 fast-fail 等特殊崩溃可能需要对应版本的 WER 支持文件和注册流程。若 SDK 构建生成了 `crashpad_wer.dll`，按该版本文档将它与 handler 一起部署并验证；最小示例不代表已覆盖全部崩溃类型。[官方 README 的 Known Limitations](https://github.com/getsentry/sentry-native#known-limitations)还列出了部分回调限制。

## 8. 发布时应该自动完成什么

```text
构建固定提交
  → 保存二进制、符号、构建标识
  → 上传符号并等待处理完成
  → 打包 SDK 动态库、handler 和必要依赖
  → 用安装包进行消息与崩溃测试
  → 发布
```

生产环境关闭开发调试日志，设置稳定的 release 和 environment，把数据库放到用户可写且能跨启动保留的位置。不要让安装清理流程无意中删除待发送报告。

下一篇：[CEF 程序接入 Sentry](03-CEF程序接入Sentry.md)。
