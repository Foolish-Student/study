# 第三篇：CEF 程序接入 Sentry，先分清进程和错误类型

> 前置：[Sentry 原理](01-Sentry是什么与工作原理.md)、[普通 C++ 接入](02-C++程序接入Sentry.md)。本文从 CEF C++ 应用出发，以 Windows 的配置为主；Linux/macOS 的后端和启动方式须按你的 CEF 版本核实。
>
> 核对日期：2026-10-18。这里给出可逐步实施的集成路线；未获得你的 CEF 版本、构建配置和项目代码，不能声称某个初始化顺序已经适用于你的程序。

## 1. 一个 CEF 应用其实包含多个进程

```text
桌面应用
├── Browser 主进程：窗口管理、业务 C++、CEF 生命周期
├── Renderer 进程：页面、JavaScript、渲染相关逻辑
├── GPU 进程：图形相关工作
├── Utility 等进程：按 Chromium 实际需要启动
└── 崩溃处理进程：收集和上传报告
```

同一个 EXE 可能按命令行参数扮演不同角色；也可能使用独立 subprocess EXE 或 macOS Helper。Browser 指 CEF 主进程，不是某一个网页窗口。

因此，在主进程里调用一次 `sentry_init()`，不能推导出所有子进程都被覆盖。Renderer 崩溃时，主窗口甚至可能仍在运行。

同时要分清三类问题：

| 问题 | 对应采集路径 |
| --- | --- |
| 主进程、Renderer、GPU 的原生崩溃 | CEF 原生崩溃采集 + Sentry Minidump 接收 |
| 页面未捕获 JS 错误、Promise rejection | 页面中的 Sentry Browser SDK |
| 业务错误、Renderer 退出通知 | 正常执行路径中的主动事件或日志 |

JS SDK 无法代替原生 dump；原生 dump 也不能代替网页错误堆栈和 source maps。

### 1.1 Qt + CEF：为什么 Qt 代码崩溃也能捕获

CEF 可以与 Qt 结合使用，但它本身不依赖 Qt。另外，Qt WebEngine 与直接集成 CEF 是不同方案；这里讨论的是你在 Qt 桌面应用中直接嵌入 CEF 的情况。

常见结构是：Qt 窗口、业务 C++ 和 CEF Browser 主进程代码在**同一个操作系统进程**里运行：

```text
MyQtCefApp.exe，PID 100
├── Qt 主窗口和按钮回调
├── 业务 C++ 代码
├── CEF Browser 主进程代码
└── 已建立的进程级原生崩溃采集机制

CEF Renderer，PID 200：另一个进程
CEF GPU，PID 300：另一个进程
Crashpad handler：独立采集进程
```

**Crashpad 关注的是“哪个进程、哪个线程发生了原生故障”，不是“这条指令属于 Qt 还是 CEF”。** CEF 建立的主进程崩溃采集关系生效后，主进程内 Qt 按钮回调、业务代码或 Qt 动态库中的符合采集条件的未处理原生异常，也可能进入同一条崩溃处理路径。

例如用户点击 Qt 的“打开文件”按钮：

```cpp
// 这是 Qt 按钮对应的业务槽函数；假设它运行在已建立采集关系的主进程中。
void MainWindow::OnOpenClicked() {
    // 执行普通业务代码；这里即使没有调用任何 CEF API，也仍处于同一个主进程。
    LoadDocument();
}
```

假设 `LoadDocument()` 里发生未处理的无效内存访问，采集链路是：

```text
Qt 按钮回调 → LoadDocument 中发生原生异常
                     ↓
            操作系统分发主进程异常
                     ↓
         进入已配置的进程级崩溃处理路径
                     ↓
       handler 采集这个进程的线程、栈、模块等
                     ↓
          上传 dump，Sentry 还原业务与 Qt 相关帧
```

不是先由 Qt 通知 CEF “请帮我上传”，也不是 CEF 只在自己的 API 调用期间才能监控。关键条件是：**故障发生在已经被采集机制覆盖的进程中，且走到了可用的崩溃采集路径。** 通知细节见[第四篇](04-崩溃现场与堆栈还原原理.md)。

### 1.2 哪些 Qt 场景不能由此推断一定能捕获

| 场景 | 应怎样判断 |
| --- | --- |
| 同进程中业务函数发生未处理原生异常 | 可能被当前进程的采集机制捕获，需实测 |
| 同进程的 QThread 中发生原生崩溃 | 也属于该进程，但线程异常入口及具体崩溃类型仍需验证 |
| Qt 创建了另一个独立工具进程 | 需要那个进程自己的有效采集关系，主进程接入不能代替 |
| QApplication 创建或静态初始化阶段崩溃 | 要看故障发生时崩溃采集是否已经就绪 |
| qWarning、普通错误返回、被捕获的异常 | 没有因此发生原生崩溃，不会自动变成 dump |
| UI 卡死、事件循环阻塞 | 不一定有原生异常，通常需要另外检测 |
| 进程被强制结束、某些 OOM 或特殊终止 | 不能保证走到可用的采集路径 |

如果想把 Qt 日志自动转为 Sentry 事件或错误前的操作记录，那是另外的日志集成，不是进程级崩溃采集自动完成的功能。

### 1.3 能收到 Qt 崩溃，不等于 Qt 堆栈一定可读

同一份 dump 可包含主 EXE、业务 DLL、Qt 动态库和 libcef 等模块。每个模块的符号都需单独匹配：

- 只上传业务 PDB：可能看懂 `MainWindow::OnOpenClicked`，却看不懂 Qt 库内部的帧。
- 需要分析 Qt 内部：取得与发布时 Qt 构建匹配的 PDB/dSYM/调试信息，按项目符号策略提供。
- 需要分析 CEF 内部：仍需要对应 CEF 构建的符号。

这也是为什么应该用 Qt 按钮回调、业务工作线程、CEF Browser/Renderer 等不同入口分别触发测试，并核对事件中的故障线程与模块，而不是只验证一条 CEF 测试 URL。

## 2. 选择一种明确的原生崩溃归属

CEF 已有自己的崩溃报告机制。**不要默认在每个进程再启动一套 sentry-native Crashpad。** 两套机制同时争用异常处理入口、handler 和上传流程，可能造成遗漏、重复或初始化顺序问题。

对新手，本教程建议先采用这条路线：

```text
CEF 自带崩溃采集
   → CEF 的 ServerURL 指向 Sentry Minidump Endpoint
   → Sentry 解析 dump
   → 使用匹配的业务模块及 CEF 符号生成可读堆栈

页面 JavaScript
   → @sentry/browser
   → Sentry 网页项目
```

这是依据 CEF 的 multipart 上传能力和 Sentry 的 Minidump 接口组合的工程方案，不是声称存在一个能自动处理全部细节的 CEF 专用 SDK。必须在实际版本和安装包里做端到端验证。

如果需要丰富的原生主动事件，可另外让 sentry-native 以 `SENTRY_BACKEND=none` 编译来发送消息，保留唯一的原生崩溃采集方。它不负责捕获崩溃；它的 scope、tags、breadcrumbs 也不会自动加入 CEF 独立上传的 dump。

需要用 Native SDK 完全接管 CEF 崩溃时，应作为另一套专项方案研究：逐进程核对采集入口、沙箱、启动时序和后端兼容性。不能把“在 `CefExecuteProcess` 前初始化一下”当作通用答案。

## 3. 拿到正确的上传地址

进入 Sentry 项目的 Client Keys（DSN）设置，复制 **Minidump Endpoint URL**。以界面提供的完整地址为准，特别是 SaaS 的区域接收域名或自建部署的路径。

它的常见形式是：

```text
https://INGEST_HOST/api/PROJECT_ID/minidump/?sentry_key=PUBLIC_KEY
```

配置到 CEF 的是这个 Minidump Endpoint，而不是普通 DSN、管理 API 地址或 `/envelope/` 地址。

已有一个测试 dump 时，可先独立验证接收端：

```bash
curl -X POST '替换为完整的MinidumpEndpointURL' \
  -F 'upload_file_minidump=@/absolute/path/test.dmp'
```

这一步仅验证接收链路，不验证 CEF 自动采集、重试和进程覆盖。[Sentry 官方 Minidump 接口文档](https://docs.sentry.io/platforms/native/guides/minidumps/)说明了 endpoint 的获取方法及 multipart 字段。

## 4. 配置 CEF 的 crash_reporter.cfg

下面先展示配置文件结构，随后给出通过环境变量动态指定接收地址的实际启动方式。文件名必须是 `crash_reporter.cfg`：

```ini
[Config]
ProductName=MyCefApp
ProductVersion=1.0.0
AppName=MyCefApp
ServerURL=替换为完整的MinidumpEndpointURL

# 开发验收期间，便于重复触发测试。
RateLimitEnabled=false
MaxUploadsPerDay=0

[CrashKeys]
app_release=small
app_environment=small
app_process=small
feature=small
```

把占位符替换为真实地址；不要给 URL 额外加引号。`ProductVersion` 应由构建系统更新。上面关闭上传限流只用于验收，正式配置应按产品策略恢复适当限制。

| 平台 | 配置放置位置 |
| --- | --- |
| Windows / Linux | 主应用可执行文件旁 |
| macOS | 顶层 `.app/Contents/Resources/` |

配置应在进程启动前就位。不要等到 `CefInitialize()` 完成后才创建文件，也不要假设重启网页就会重新读取它。

Windows 的 handler 启动可能重新使用主 EXE，也可通过版本支持的 `ExternalHandler` 配置。不要将普通 sentry-native 的 `crashpad_handler.exe` 随意填进去：名字相似不代表启动协议、版本和依赖匹配。保持当前 CEF 分发包要求的入口流程与 `chrome_elf.dll` 等组件一致。

平台默认值、配置项和目录以随你使用的 CEF 版本发布的 `include/cef_crash_util.h` 为准。[官方头文件](https://github.com/chromiumembedded/cef/blob/master/include/cef_crash_util.h)比旧博客中的 `user_data_path` 等说法更适合核对当前 API。

### 4.1 接收地址通过环境变量传入，不必写死

接收地址可以根据测试、生产、发布渠道等部署配置注入，无需固定在源码或配置文件里。不过，**DSN 不一定经常变化，环境变量是配置传递方式；启动后修改变量也不代表已运行的 handler 会自动切换接收地址。**

CEF 当前源码的 `ReadCrashConfigFile()` 在读取配置后，还会读取 `CEF_CRASH_REPORTER_SERVER_URL`，用它覆盖上传地址。请在你实际使用的 CEF 分支中核对是否支持，不能仅凭最新源码推断旧版本行为。参考 [CEF 官方实现](https://github.com/chromiumembedded/cef/blob/master/libcef/common/crash_reporter_client.cc)，搜索这个变量名即可找到。

三个名字需要分清：

| 名称 | 谁使用 | 填什么 |
| --- | --- | --- |
| `SENTRY_DSN` | 由应用或相应 SDK 集成读取；不是这条 CEF 配置路径的通用开关 | 普通 Sentry DSN |
| `CEF_CRASH_REPORTER_SERVER_URL` | 支持该变量的 CEF 原生崩溃报告实现 | 完整的 Minidump Endpoint URL |
| 配置中的 `ServerURL` | CEF 从 cfg 读取，当前实现可被上述变量覆盖 | 同样是 Minidump Endpoint URL |

不能只在系统里设置 `SENTRY_DSN`，就认为 CEF 会自动采用它；也不要把普通 DSN 直接放进 CEF 的 URL 变量。

### 4.2 Windows：保留基础配置，通过启动环境提供地址

在主应用旁保留 `crash_reporter.cfg`，例如：

```ini
# 保留基础配置，地址由启动环境提供。
[Config]
ProductName=MyQtCefApp
ProductVersion=1.0.0
AppName=MyQtCefApp

# 开发测试时关闭限流；生产环境按产品策略配置。
RateLimitEnabled=false
MaxUploadsPerDay=0

[CrashKeys]
app_release=small
app_environment=small
app_process=small
feature=small
```

然后在 PowerShell 中启动：

```powershell
# 设置本次启动使用的完整 Minidump Endpoint；值从项目设置或部署系统获取。
# 这个变量值不是普通 DSN；支持该变量的 CEF 会直接读取它。
$env:CEF_CRASH_REPORTER_SERVER_URL = "https://INGEST_HOST/api/PROJECT_ID/minidump/?sentry_key=PUBLIC_KEY"

# 启动应用；新进程默认继承此 PowerShell 的环境变量。
& "C:\Apps\MyQtCefApp\MyQtCefApp.exe"
```

在这条源码实现路径中，配置文件打不开就会提前返回，读取环境变量的代码也不会执行。因此不要把“URL 可以来自环境变量”误解成“可以把 cfg 文件全部删掉”。

Linux 的 shell 示例：

```bash
# 向后续启动的进程传递动态接收地址；替换占位符。
export CEF_CRASH_REPORTER_SERVER_URL='https://INGEST_HOST/api/PROJECT_ID/minidump/?sentry_key=PUBLIC_KEY'

# 从当前 shell 启动应用，并验证当前 CEF 平台实现是否按预期读取配置。
/path/to/MyQtCefApp
```

macOS 从 Finder 或其他启动器运行时，不能假定它会继承终端里的 `export`；应按实际部署入口提供环境。

### 4.3 Qt 程序里 qputenv 可以吗？先看设置时机

下面两行可以在**尚未启动目标应用的独立 Qt launcher** 中使用，演示环境变量的传递；它们不是要求你照搬进主应用的 `main()`：

```cpp
// 从 launcher 的部署环境读取完整 Minidump Endpoint；为空时应由启动逻辑报错或明确禁用上传。
const QByteArray endpoint = qgetenv("APP_SENTRY_MINIDUMP_URL");

// 写入当前 launcher 环境，供之后创建且继承此环境的目标进程使用。
qputenv("CEF_CRASH_REPORTER_SERVER_URL", endpoint);
```

启动目标进程时，要让它继承这份环境；若使用 `QProcess::setProcessEnvironment()` 自定义了环境，就应在那份环境中也明确加入该变量。不要在多个子进程启动途中反复修改全局环境。

CEF 的部分 Windows 集成会在加载 `chrome_elf.dll` 等很早的阶段初始化崩溃采集，可能早于应用 `main()`。因此，在主应用里先构造 `QApplication`、再 `qputenv`，未必赶得上。优先由父进程、启动脚本或 launcher 在目标进程创建前设置。

如果平台部署只提供普通 `SENTRY_DSN`，可由 launcher 使用可靠的 URL 解析逻辑转换成 Minidump Endpoint 后再传入；要正确处理自建服务的路径前缀、区域主机和 URL 编码，不能简单字符串替换。直接提供项目给出的完整 endpoint 更便于核对。

对于不支持上述环境变量的旧版 CEF，可由部署系统或独立 launcher **在目标进程启动前**生成正确位置的 cfg。安装目录权限、多实例竞争和 macOS 签名包的完整性都需要考虑，不能让各个 CEF 子进程同时重写配置。

### 4.4 动态配置的验证步骤

1. 启动前确认 cfg 在正确位置，并从实际启动入口提供环境变量。
2. 用测试项目 endpoint 启动，触发一次受控崩溃，确认报告进入该项目。
3. 完整退出应用及相关 handler 后，修改启动环境，再启动到另一个测试项目验证。
4. 分别检查 Browser、Renderer、GPU 路径；不能只凭主进程能读取变量就认为全部生效。

当前机制通常是在初始化时取得地址，传给 handler 后使用；不是每次崩溃都重新读取你后来修改的环境变量。已保存报告在地址变更后的处理方式，也应按对应版本验证。

## 5. 增加能够解释崩溃的 Crash Keys

```cpp
#include "include/cef_crash_util.h"

void SetBrowserCrashContext() {
    // 在该版本允许调用的初始化阶段之后执行。
    if (!CefCrashReportingEnabled()) {
        return; // 记录应用日志，检查配置是否被识别。
    }
    CefSetCrashKeyValue("app_release", "my-cef-app@1.0.0");
    CefSetCrashKeyValue("app_environment", "staging");
    CefSetCrashKeyValue("app_process", "browser");
    CefSetCrashKeyValue("feature", "startup");
}
```

将 Browser 设置放到你的 `CefBrowserProcessHandler::OnContextInitialized()` 等合适位置；Renderer 中可在 `CefRenderProcessHandler::OnWebKitInitialized()` 等对应生命周期里设置自己的字段。这里展示的是要加入现有 handler 的逻辑，不是完整 CEF 应用。

主进程设置不等于子进程自动继承。Renderer 中将 `app_process` 设为 `renderer`；GPU 等不直接运行你业务回调的进程，应结合其默认元数据识别，不能假定调用了 Renderer 回调就覆盖所有子进程。

自定义 key 需要先在 `[CrashKeys]` 声明。值有长度限制，会截断，适合短分类信息。它不适合装完整日志、文件内容或很长的 URL。

还有一个容易误解的地方：`app_release` 是自定义元数据名，并不保证自动成为 Sentry 的标准 `release`；`app_environment` 也不保证自动成为标准环境字段。应在收到的真实事件中检查字段位置。

如果需要可靠映射成 Sentry 标准字段，可增加服务端转发层，把 CEF multipart 中的元数据转换为 Sentry 支持的 `sentry` JSON 字段，再连同 dump 转发。例如手工验证：

```bash
curl -X POST '替换为完整的MinidumpEndpointURL' \
  -F 'upload_file_minidump=@/absolute/path/test.dmp' \
  -F 'sentry={"release":"my-cef-app@1.0.0","environment":"staging","tags":{"process":"renderer"}}'
```

这个 curl 演示的是接口能力，不表示 CEF 会自动生成该 JSON。自建转发层需要处理文件大小、鉴权、限流、失败重试和元数据校验。

## 6. CEF 启动流程中，哪些步骤与 Sentry 有关

**本文采用的是“CEF 自带崩溃采集 → 上传到 Sentry”的方案。** 所以这里没有通过 `sentry_init()` 安装另一套原生崩溃采集机制。

下面用两种标记区分：

- **【上报配置】**：为了把崩溃报告送到 Sentry，需要提前准备或验证的内容。
- **【CEF 原有步骤】**：即使不接 Sentry，CEF 应用本来就需要的启动或退出流程。

### 6.1 带标注的完整流程

```text
目标应用启动之前（由部署系统、启动脚本或独立 launcher 完成）
  │
  ├─【上报配置】把 crash_reporter.cfg 放在当前平台规定的位置
  │              作用：提供 CEF 崩溃报告配置和需要声明的 Crash Keys
  │
  ├─【上报配置】设置 CEF_CRASH_REPORTER_SERVER_URL 环境变量
  │              作用：在支持该变量的版本中，将上传地址指向 Sentry Minidump Endpoint
  │              若地址已由 cfg 的 ServerURL 提供，则不必重复用环境变量配置
  │
  └─ 启动目标应用，让它获得准备好的文件和环境
        │
        ├─【CEF 原有步骤】平台入口 / 必要的 CEF bootstrap
        │
        ├─【CEF 原有步骤】构造 CefMainArgs、CefApp 和沙箱参数
        │
        ├─【CEF 原有步骤】CefExecuteProcess(...)
        │    ├─ 返回 >= 0：子进程路径已处理，直接返回该退出码
        │    └─ 返回 < 0：继续主进程路径
        │
        ├─【CEF 原有步骤】CefInitialize(...)
        │
        ├─【上报配置】在允许的生命周期回调中检查 CefCrashReportingEnabled()
        │              作用：检查当前进程是否启用了 CEF 崩溃报告能力
        │              注意：不是检查 Sentry 服务端是否已经收到报告
        │
        ├─【上报配置，可选】调用 CefSetCrashKeyValue(...) 补充版本、操作阶段等
        │                    作用：为后续崩溃提供解释线索，本身不触发上传
        │
        ├─【CEF 原有步骤】创建 Browser、运行消息循环和业务逻辑
        │
        └─【CEF 原有步骤】主进程正常退出时调用 CefShutdown()
```

最后几步展示主进程路径。Renderer 等子进程会在它们对应的生命周期回调里检查能力、设置自己的 Crash Keys，而不是等待主进程为它们执行这些操作。

### 6.2 哪一步真正启用了崩溃采集

**不是在 `CefInitialize()` 后检查一次 `CefCrashReportingEnabled()`，才开始采集。** 后者是查询函数，不是开启开关。

CEF 在平台特定的早期初始化路径中读取配置、安装或登记异常采集入口，并启动或连接 handler。这些动作由 CEF/Chromium 内部完成，不需要在上面的业务流程中再调用一个通用的 `EnableSentry()`。

这个内部阶段不能统一标在某一行之后：例如部分 Windows 集成会在 `chrome_elf.dll` 加载等很早的阶段执行，甚至早于应用 `main()`。这就是为什么上报文件和环境变量要提前准备，不能等 `CefInitialize()` 之后再设置。

这里有两个需要分别核对的结果：

```text
CEF 的内部初始化建立了崩溃采集关系
    → 发生支持范围内的原生崩溃时，可以采集报告

上报配置将接收地址指定为 Sentry Minidump Endpoint
    → handler 按配置将报告发往 Sentry
```

第一项成功，不代表第二项的地址和网络一定正确。最终仍要触发测试崩溃，在 Sentry 网页找到对应进程的事件。

### 6.3 原来那些 CEF API 与 Sentry 的关系

| 步骤 | 本来做什么 | 与本次 Sentry 接入的关系 |
| --- | --- | --- |
| `CefExecuteProcess` | 按当前进程角色执行相应 CEF 子进程逻辑 | 要保留正确分流，否则可能干扰子进程或 handler 的启动；它不是 Sentry 上传函数 |
| `CefInitialize` | 初始化主进程的 CEF 运行环境 | 保留正常初始化；不能把返回成功当成 Sentry 上传成功 |
| 创建 Browser、运行消息循环 | 让页面和应用正常工作 | 业务运行阶段，之后可能在这里发生被采集的故障 |
| `CefShutdown` | 正常关闭主进程中的 CEF | 正常退出的清理步骤，不是“现在上传崩溃”的命令；崩溃时不保证执行 |
| `CefCrashReportingEnabled` | 查询当前进程的崩溃报告启用状态 | 用于接入检查，不能证明网络可达或符号齐全 |
| `CefSetCrashKeyValue` | 为当前进程设置崩溃元数据 | 可选的诊断增强，不是开启采集或立即上传 |

这是流程说明，省略了 Windows 沙箱、Qt 事件循环整合与 macOS framework/helper 等平台代码，实际入口应以所用版本官方示例和已有工程为准。接入时保留正确的沙箱、签名和打包方式，并用真实安装包验证。

## 7. 页面里的 JavaScript 单独接入

你能修改页面源码时，在前端工程安装：

```bash
npm install @sentry/browser
```

在页面业务脚本执行前的入口中初始化：

```javascript
import * as Sentry from "@sentry/browser";

Sentry.init({
  dsn: "替换为网页项目的DSN",
  release: "my-cef-web@1.0.0",
  environment: "staging",
  sendDefaultPii: false,
});

Sentry.setTag("container", "cef");

// 只用于验证，运行一次后删除。
setTimeout(() => {
  throw new Error("CEF page test error");
}, 1000);
```

Browser SDK 的使用入口见 [Sentry JavaScript 官方文档](https://docs.sentry.io/platforms/javascript/)。应固定实际安装的 SDK 版本，并确认其浏览器支持范围覆盖你所用 CEF 的 Chromium 版本。

压缩后的 JS 需要上传匹配的 source maps，才能定位到原始源码；C++ 的 PDB 对此不起作用。页面 CSP、`connect-src`、自定义 scheme、离线资源和网络策略都可能影响 SDK 加载与发送。

上面适用于能修改且支持打包的自有页面。第三方页面不应直接套用；若改用 JS bridge 转交事件，需要明确授权来源、字段格式、大小和频率，并防止页面伪造原生上报。

建议初期将 native 与 web 分项目管理，统一保留客户端构建号作为关联线索。即使两边 release 字符串相同，也不会自动把两条事件合成一次端到端调用。

## 8. 符号文件：业务模块与 libcef 都要考虑

业务 EXE、业务 DLL 和 libcef 是不同模块。只上传你的 PDB，通常只能改善你自己的代码帧。

Windows 需要与实际运行的 `libcef.dll` 精确匹配的符号：同一 CEF 构建、平台、架构与配置；macOS/Linux 同样需要匹配的模块标识。优先从对应分发渠道取得该构建提供的符号，自己编译时则保存自己的符号产物。

```bash
sentry-cli debug-files upload -o YOUR_ORG -p YOUR_NATIVE_PROJECT --wait /path/to/matching-symbols
```

符号可能很大，应在 CI 中管理、上传和缓存。若报告缺少某个模块符号，按事件中的模块标识逐个核对，不能仅凭“都是 CEF 版本 1xx”判断匹配。命令细节见 [第二篇](02-C++程序接入Sentry.md)。

## 9. 按进程逐项验收

只在专门测试环境使用以下地址，先保存工作内容。CEF 官方提供的测试入口包括：

| 测试目标 | 地址 | 应观察到什么 |
| --- | --- | --- |
| Browser 主进程 | `chrome://inducebrowsercrashforrealz` | 主程序异常退出，有对应原生事件 |
| Renderer | `chrome://crash` | 页面进程退出，有对应子进程事件 |
| GPU | `chrome://gpucrash` | 按平台表现出现恢复/图形异常，并检查对应报告 |
| 页面 JS | 上一节测试代码 | 有 JS 错误事件，通常不导致原生进程崩溃 |

测试地址的可用性受 CEF 版本和应用导航策略影响；不能打开时应在受控测试构建中使用相应测试机制。脱离调试器分别测试，不要只测主进程。入口与测试思路可参考 [CEF 官方 crash_server.py](https://github.com/chromiumembedded/cef/blob/master/tools/crash_server.py)。旧 Python 测试脚本还可能需要适配当前 Python 环境。

对于每一类原生崩溃，检查本地报告、上传响应、服务端事件、进程元数据和符号化结果。再覆盖安装包、非管理员用户、断网恢复和连续重启。

`OnRenderProcessTerminated` 可用于记录退出状态和恢复页面，但它是主进程获知 Renderer 退出的通知，不是崩溃现场。你在这里 capture 的主进程堆栈不是 Renderer 崩溃堆栈；若又生成一个事件，它与 dump 可能是两条独立记录。回调签名请以当前 CEF 头文件为准。

## 10. 常见故障按层定位

| 现象 | 从哪里开始查 |
| --- | --- |
| `CefCrashReportingEnabled()` 为 false | 配置文件名、位置、启动前是否存在、当前版本支持 |
| dump 存在，Sentry 没有事件 | ServerURL 是否为 minidump endpoint、上传响应、TLS/代理、限流退避 |
| 主进程成功，Renderer/GPU 失败 | 真实子进程路径、handler 分支、沙箱、平台支持和启动逻辑 |
| 只有业务帧可读，libcef 是地址 | 对应 CEF 模块符号是否上传、标识是否匹配 |
| Crash Keys 没出现 | 是否声明、是否在实际崩溃进程设置、是否超长、界面字段位置 |
| JS 错误不出现 | 页面初始化时机、SDK 兼容性、CSP、网络、自定义协议 |
| 同一故障出现两份报告 | 是否两套 crash handler 或退出回调重复上报 |
| 安装后失败 | cfg 和运行时组件是否打包、目录权限、Helper 签名与启动路径 |

CEF 官方的 [Crash Reporting 文档](https://github.com/chromiumembedded/cef/blob/master/docs/crash_reporting.md)含有跨平台说明，但部分字段描述可能与最新头文件存在差异；工程中优先核对你实际使用版本的头文件、示例和产物。

## 11. 回到你已有的工程，先核对这些位置

打开 AI 帮你写的接入代码，沿着这几个问题阅读：

1. 现在是谁采集原生崩溃：CEF 还是 sentry-native？是否同时启动了两套？
2. 哪些进程实际执行了初始化？handler 的专用启动路径有没有被业务逻辑干扰？
3. 上传地址是哪种接口？DSN 与 Minidump Endpoint 是否混用？
4. 业务模块和 libcef 的符号分别来自哪一次构建？
5. 主进程设置的上下文，是否被误认为会自动传播给子进程？
6. JS 错误有没有独立验证？是否上传了匹配的 source maps？
7. 是否用真实安装包分别触发过 Browser、Renderer、GPU 崩溃？

能把这些问题与具体代码、配置和实际事件一一对应，你就能判断接入方案为什么有效，以及故障发生后该从哪一层排查。
