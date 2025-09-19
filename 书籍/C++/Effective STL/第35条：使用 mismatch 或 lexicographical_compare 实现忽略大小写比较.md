# **第35条：使用 `mismatch` 或 `lexicographical_compare` 实现忽略大小写比较**

## **核心思想**

* STL 算法 **默认区分大小写**，比如 `std::equal`、`std::lexicographical_compare`
* 如果希望忽略大小写，可以结合 **函数适配器或自定义比较函数** 使用 `mismatch` 或 `lexicographical_compare`
* 这种方法 **避免手动逐字符循环**，简洁且高效

---

## **1️⃣ `std::mismatch` 忽略大小写**

### 功能

* `mismatch` 找到两个区间中 **第一个不匹配的元素对**
* 可以提供自定义比较函数，用于忽略大小写

### 示例

```cpp
#include <algorithm>
#include <cctype>
#include <string>

std::string s1 = "Hello";
std::string s2 = "heLLo";

auto result = std::mismatch(
    s1.begin(), s1.end(),
    s2.begin(),
    [](char a, char b) { return std::tolower(a) == std::tolower(b); }
);

bool equal_ignore_case = (result.first == s1.end()); // true
```

* **解释**：

  * `std::tolower` 将字符转换为小写
  * lambda 比较函数忽略大小写
  * `mismatch` 返回的迭代器指向第一个不同字符
  * 如果到末尾都匹配 → 字符串相等

---

## **2️⃣ `std::lexicographical_compare` 忽略大小写**

### 功能

* 比较两个区间的 **字典序**
* 可以提供自定义比较函数，实现忽略大小写排序

### 示例

```cpp
#include <algorithm>
#include <cctype>
#include <string>

std::string s1 = "apple";
std::string s2 = "Banana";

bool less_ignore_case = std::lexicographical_compare(
    s1.begin(), s1.end(),
    s2.begin(), s2.end(),
    [](char a, char b){ return std::tolower(a) < std::tolower(b); }
);
```

* **解释**：

  * 比较每个字符的 **小写形式**
  * 忽略大小写实现字典序排序
  * 返回 true → s1 < s2

---

## **3️⃣ 使用场景**

1. **忽略大小写判断字符串相等** → `mismatch` 或 `equal` + 自定义比较函数
2. **忽略大小写排序或比较大小** → `lexicographical_compare`
3. **字符串集合操作** → 可结合 `std::sort` + 自定义比较函数进行忽略大小写排序

---

## **4️⃣ 注意事项**

* 仅对 **ASCII 字符** 简单有效
* 对 **Unicode** 或多字节字符，需要更复杂的转换函数
* 自定义比较函数必须一致，否则会导致排序/查找逻辑错误

---

## **5️⃣ 核心总结**

* **mismatch** → 忽略大小写判断相等
* **lexicographical\_compare** → 忽略大小写比较字典序
* 利用 **lambda 或函数适配器** 将字符统一为同一大小写即可
* 避免手动循环字符 → 简洁、安全

---

💡 **一句口诀**

> **“mismatch找不等，lex\_compare比大小，小写统一最方便。”**
