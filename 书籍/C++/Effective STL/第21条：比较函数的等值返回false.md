# 第 21 条：比较函数的等值返回 false

## 背景

在 C++ 中，标准模板库（STL）中的 **排序容器**（如 `std::set`、`std::map`）或算法（如 `std::sort`）经常使用**比较函数**（Compare Function）来确定元素的顺序。比较函数通常形式如下：

```cpp
bool cmp(const T &a, const T &b);
```

它用于判断 **a 是否应该排在 b 前面**。

---

## 核心要求

**无论何时，当两个元素“等值”时，比较函数必须返回 `false`**。

换句话说：

```cpp
cmp(a, b) == false && cmp(b, a) == false  // a 和 b 被认为相等
```

### 为什么？

1. **STL 容器要求严格弱序（Strict Weak Ordering）**：

   * 比较函数必须保证：

     * **反对称性**：如果 `cmp(a, b) == true`，则 `cmp(b, a) == false`
     * **等值返回 false**：如果两个元素相等，不管先后调用，`cmp(a,b)` 或 `cmp(b,a)` 都必须返回 false
   * 否则，容器无法正确判断元素是否相等，可能导致：

     * `std::set` 或 `std::map` 插入重复元素失败
     * 排序算法出现未定义行为

2. **避免重复元素被误判为不同**：

   * 如果 `cmp(a,b)` 在等值时返回 `true`，容器会认为它们不同，从而插入重复元素。
   * 可能破坏容器的唯一性约束。

---

## 示例

```cpp
struct MyCompare {
    bool operator()(int a, int b) const {
        return a < b;  // 当 a == b 时返回 false，正确
    }
};

std::set<int, MyCompare> s;
s.insert(1);
s.insert(1);  // 不会插入第二个 1，保持唯一性
```

❌ **错误示例**：

```cpp
struct WrongCompare {
    bool operator()(int a, int b) const {
        return a <= b;  // 当 a == b 时返回 true，错误！
    }
};

std::set<int, WrongCompare> s;
s.insert(1);
s.insert(1);  // 第二个 1 也会插入，破坏唯一性
```

---

## 总结要点

1. **比较函数永远不要在等值情况下返回 true**。
2. **正确实现**：

   * 对于 `a < b` 的比较，自然满足要求：`a == b` → 返回 false
3. **STL 容器与算法依赖严格弱序**：

   * 排序、去重、查找都基于此假设
4. **小技巧**：

   * 对于复杂类型比较，先比较主要字段，再比较次要字段，但最终要保证等值返回 false。

---

**一句话记忆法**：

> **“相等时，别让比较函数说它更小，返回 false 就对了。”**

