# 第 36 条：理解 `copy_if` 算法的正确实现

## 1. 功能概述

`copy_if` 是 C++ STL 中的算法，用于从一个范围 `[first, last)` 中 **选择满足特定条件的元素**，并将它们复制到另一个目标容器或目标迭代器中。

* **基本用途**：过滤元素。
* **函数原型**（C++11 及以上）：

```cpp
template<class InputIt, class OutputIt, class UnaryPredicate>
OutputIt copy_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate pred);
```

* `first, last`：输入范围的起始和结束迭代器。
* `d_first`：输出起始迭代器。
* `pred`：一元谓词函数，返回 `true` 的元素会被复制到输出。

---

## 2. 使用示例

假设有一个整数数组，我们只想复制其中的偶数到另一个容器：

```cpp
vector<int> nums = {1,2,3,4,5,6};
vector<int> evens;

copy_if(nums.begin(), nums.end(), back_inserter(evens),
        [](int x){ return x % 2 == 0; });
```

* `back_inserter` 会在目标容器末尾自动插入元素。
* `lambda` 函数 `[](int x){ return x % 2 == 0; }` 作为谓词，只复制偶数。
* 结果：`evens = {2,4,6}`

---

## 3. 常见错误和注意事项

1. **目标迭代器类型错误**

   * 目标必须是 **可写迭代器**，如 `vector<int>::iterator` 或 `back_inserter`。
   * 不能使用 `const_iterator`，因为它只读，无法写入。

2. **未使用适当的插入迭代器**

   * 对于 `vector`、`deque` 等动态容器，如果目标容器为空，推荐使用 `back_inserter`。
   * 直接传递 `v.begin()` 如果容器为空或大小不足，会导致未定义行为。

3. **谓词函数必须返回布尔值**

   * `copy_if` 会调用谓词，对每个元素返回 `true` 才复制。
   * 返回非布尔值可能导致意想不到的行为。

4. **不改变输入容器**

   * `copy_if` 不会删除或修改输入范围的元素，只负责复制。
   * 如果希望同时过滤并删除，需要配合 `erase-remove` 习惯用法。

---

## 4. 代码实现建议

* **常规安全写法**：

```cpp
vector<int> src = {1,2,3,4,5};
vector<int> dest;

copy_if(src.begin(), src.end(), back_inserter(dest),
        [](int x){ return x > 3; });
```

* **特点**：

  * 输入不被修改
  * 输出容器会动态增长
  * 谓词可以是函数、函数对象或 lambda

---

## 5. 总结要点

* `copy_if` 是一个 **过滤并复制元素的算法**。
* 目标迭代器必须可写（不能是 `const_iterator`）。
* 谓词函数必须返回布尔值。
* 常用搭配：`back_inserter`，保证动态容器安全插入。
* 适合场景：从容器中筛选满足条件的子集，而不改变原容器。
