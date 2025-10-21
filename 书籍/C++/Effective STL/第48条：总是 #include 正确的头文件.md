# 第48条：总是 `#include` 正确的头文件

## 核心要点（一句话）

**用到什么，就显式 `#include` 对应的标准头；不要依赖“间接包含（transitive includes）”。**
不同实现的内部包含关系不受标准约束，今天能编过，明天换编译器/版本就可能挂。

---

## 为什么必须这样做？

1. **可移植性**
   标准并不保证 `<vector>` 会“顺便”把 `<algorithm>` 包进来。你今天因“巧合”编过，换个实现就会失败。

2. **可维护性**
   显式列出依赖，别的同事一看你的文件顶部就知道它用到了哪些库；工具也更容易做依赖分析/增量构建。

3. **可读性与诊断**
   一旦符号未定义、重名、二义性等问题出现，明确的 `#include` 能让问题定位更直观。

---

## 具体建议与示例

### 1）容器/算法/工具：各就各位

* 容器：
  `std::vector` → `<vector>`
  `std::string` → `<string>`
  `std::map`/`std::set` → `<map>` / `<set>`
  `std::unordered_map`/`std::unordered_set` → `<unordered_map>` / `<unordered_set>`
  `std::queue`/`std::stack>`/`std::priority_queue` → `<queue>` / `<stack>` / `<queue>`

* 算法与数值：
  `std::sort`、`std::max_element`、`std::lower_bound` → `<algorithm>`
  `std::accumulate`、`std::inner_product` → `<numeric>`
  `std::min`/`std::max`（常用）→ **也在 `<algorithm>`**（别误以为在 `<utility>`）

* 迭代器工具：
  `std::advance`、`std::distance`、迭代器标签 → `<iterator>`

* 函数对象/哈希/绑定：
  `std::less`、`std::greater`、`std::hash`、`std::function`、`std::bind` → `<functional>`

* 类型与工具：
  `std::pair`、`std::move`、`std::forward`、`std::swap` → `<utility>`
  `std::size_t`、`std::nullptr_t` → `<cstddef>`
  `std::numeric_limits` → `<limits>`
  `std::optional`、`std::variant`、`std::any` → `<optional>` / `<variant>` / `<any>`
  `std::string_view` → `<string_view>`

* I/O：
  `std::cout`/`std::cin`/`std::cerr` → `<iostream>`
  `std::ifstream`/`std::ofstream` → `<fstream>`
  `std::ostringstream`/`std::istringstream` → `<sstream>`
  **仅做类型前置声明**（减少编译开销）→ `<iosfwd>`

* 智能指针/内存：
  `std::unique_ptr`、`std::shared_ptr`、`std::weak_ptr` → `<memory>`

* C 函数的 C++ 头：
  **优先使用 `<cxxx>` 版本**：
  `<cstdio>`（代替 `<stdio.h>`）、`<cstdlib>`（代替 `<stdlib.h>`）、`<cstring>`、`<cctype>`、`<cmath>` 等。
  这些符号在 `std::` 命名空间里（如 `std::printf`）。

> 经验口诀：**“谁的符号，谁的头”**——凡是你代码里出现的标准库名字，都能追溯到一个明确的标准头。

---

### 2）别做这些事

* **不要依赖间接包含**
  比如你写了 `std::sort` 但没 `#include <algorithm>`，只是因为另一个头“碰巧”带进来，**不可靠**。

* **不要前置声明（forward-declare）标准库模板**
  标准明文禁止你自己写 `template<class T> class std::vector;` 之类。
  需要用 `std::vector` 就 `#include <vector>`。

* **不要在头文件里用 `using namespace std;`**
  污染命名空间、制造二义性，且会影响所有包含它的翻译单元。

---

### 3）头文件最小化依赖：对 STL 的“度量”

* **在头文件里只放必须的 `#include`**
  若只需要声明指针/引用指向你的自定义类型，可以前置声明该**自定义类型**，避免引入大头；
  但对 **STL 容器本体**（如 `std::vector<T>`）**不能前置声明**，必须 `#include` 它的头。

* **I/O 类型的轻量声明**
  若只需声明函数签名使用 `std::istream&` / `std::ostream&`，可 `#include <iosfwd>`，
  需要真正读写时再在 `.cpp` 里 `#include <iostream>`。

---

### 4）模板代码的可见性（常见配套点）

虽然本条聚焦“包含正确头文件”，但和模板有**天然关联**：
**你自己写的模板**的一切定义通常必须放在头文件或可被头文件包含的地方（.tpp/.ipp），否则使用处链接不到定义。

---

## 速查表（常见漏包含）

| 用到了什么                               | 应该包含                       |
| ----------------------------------- | -------------------------- |
| `std::sort` / `lower_bound` / `min` | `<algorithm>`              |
| `std::accumulate`                   | `<numeric>`                |
| `std::hash` / `std::function`       | `<functional>`             |
| `std::size_t`                       | `<cstddef>`                |
| `std::pair` / `std::move`           | `<utility>`                |
| `std::string`                       | `<string>`                 |
| `std::string_view`                  | `<string_view>`            |
| `std::vector` / `std::array>`       | `<vector>` / `<array>`     |
| `std::unordered_map`                | `<unordered_map>`          |
| `std::optional` / `std::variant`    | `<optional>` / `<variant>` |
| `std::numeric_limits`               | `<limits>`                 |
| `std::tolower` / `std::isalpha`     | `<cctype>`                 |
| `std::memcpy`                       | `<cstring>`                |
| `std::printf`                       | `<cstdio>`                 |
| `std::istream`/`ostream` 前置声明       | `<iosfwd>`                 |

---

## 小例子：为什么“间接包含”会害人

```cpp
// 错误示范（可能在某些实现里侥幸编过）
#include <vector>
int main() {
    std::vector<int> v{3,1,2};
    std::sort(v.begin(), v.end());   // sort 并不属于 <vector>
}
```

换个实现就报错：**未定义标识符 `sort`**。
正确做法：再 `#include <algorithm>`。

---

## 实战建议清单

* ✅ 为每个用到的标准符号，**加上自己的、显式的头文件**。
* ✅ 头文件里**少而准**：只包含必须的。
* ✅ 不能前置声明标准库模板；要用就 `#include`。
* ✅ C 库用 `<cxxx>` 版本，配合 `std::` 前缀。
* ✅ 只需要 I/O 类型的声明时用 `<iosfwd>`，需要读写再 `#include <iostream>`。
* ❌ 不要指望“别的头里顺便带进来”。
* ❌ 不要在头文件里 `using namespace std;`。

---

**结论**
把“**包含正确头文件**”当成硬性规范执行，会让你的 C++/STL 代码更健壮、可移植、易维护。
一句话：**别靠运气，靠标准。**
