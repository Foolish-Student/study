# **第 27 条：将 const\_iterator 转换为 iterator**

## **核心思想**

在 C++ STL 中，`const_iterator` 是只读迭代器，只能访问元素而不能修改元素；`iterator` 是普通迭代器，可以修改元素的值。

标准 STL 不允许直接把 `const_iterator` 转换成 `iterator`，这样做会破坏 **const 安全性**。
但在某些场景下，我们确实需要 **从 const\_iterator 得到对应位置的可修改 iterator**，书中提出了安全的方法：

> 使用 `distance` + `advance` 间接完成转换。

---

## **具体操作步骤**

假设我们有一个容器 `d`，其中 `ci` 是一个 `const_iterator`：

```cpp
std::deque<int> d;
std::deque<int>::const_iterator ci = d.cbegin(); // 指向容器中某个元素
```

### **1. 定义普通迭代器**

```cpp
std::deque<int>::iterator i = d.begin();
```

* `i` 是普通迭代器，初始化指向容器起点。
* 注意不能直接写 `iterator i = ci;`，会报错。

---

### **2. 计算偏移量**

```cpp
auto offset = std::distance(d.begin(), ci);
```

* `std::distance(first, last)`：计算两个迭代器之间的元素个数。
* 这里得到 `ci` 离容器起点 `d.begin()` 的偏移量。

---

### **3. 移动普通迭代器到目标位置**

```cpp
std::advance(i, offset);
```

* `std::advance(it, n)`：从 `it` 出发，移动 `n` 个位置。
* 现在 `i` 与 `ci` 指向同一个元素，但 `i` 可修改元素。

---

### **4. 完整示例**

```cpp
#include <deque>
#include <iostream>
#include <iterator>

int main() {
    std::deque<int> d = {1, 2, 3, 4};

    std::deque<int>::const_iterator ci = d.cbegin() + 2; // 指向第三个元素（3）

    std::deque<int>::iterator i = d.begin();
    std::advance(i, std::distance(d.begin(), ci));

    *i = 99; // 可以修改
    for (auto x : d) std::cout << x << " "; // 输出: 1 2 99 4
}
```

---

## **要点总结**

| 步骤 | 方法                               | 作用                                  |
| -- | -------------------------------- | ----------------------------------- |
| 1  | 定义普通迭代器指向容器起点                    | 准备修改元素                              |
| 2  | `std::distance(begin, ci)`       | 计算 const\_iterator 的偏移量             |
| 3  | `std::advance(iterator, offset)` | 移动普通迭代器到 const\_iterator 对应位置       |
| 4  | 使用 iterator 修改元素                 | 安全完成 const\_iterator → iterator 的转换 |

---

## **为什么不能直接赋值**

```cpp
iterator i = ci; // ❌ 错误
```

* `const_iterator` → `iterator` 是**向下转换**。
* STL 保证 **const 安全性**：不允许只读迭代器直接变为可修改迭代器。
* 必须通过 **计算偏移 + 移动** 的方式间接实现。

---

## **适用场景**

* 需要修改容器中某个元素，但手头只有 `const_iterator`。
* 典型场景：在算法中返回 `const_iterator`（如 `find_if`、`lower_bound`），但需要修改容器内容。

---

✅ **总结口诀**

> **“从头开始算距离，向前移动普通迭代器。”**

* 头：`begin()`
* 算：`distance(begin, ci)`
* 移动：`advance(iterator, distance)`
* 结果：得到可修改的迭代器