# 第 37 条：使用 `accumulate` 或 `for_each` 进行区间统计

## 1. 功能概述

C++ STL 提供了两种常用方法来统计容器区间内的数值：

1. **`accumulate`**：直接对区间进行累加或自定义运算，返回一个最终结果。
2. **`for_each`**：对区间内每个元素执行自定义操作，可在操作中统计或更新外部变量。

这两种方法都是 **不改变原容器的元素**，适用于统计、求和、统计条件数量等场景。

---

## 2. `accumulate` 用法

* **定义**（在 `<numeric>` 头文件中）：

```cpp
template<class InputIt, class T>
T accumulate(InputIt first, InputIt last, T init);

template<class InputIt, class T, class BinaryOp>
T accumulate(InputIt first, InputIt last, T init, BinaryOp op);
```

* **参数**：

  * `first, last`：输入区间
  * `init`：初始值（累加起点）
  * `op`：二元运算函数（可选，默认为加法）

* **示例 1：求和**

```cpp
vector<int> v = {1,2,3,4,5};
int sum = accumulate(v.begin(), v.end(), 0);  // sum = 15
```

* **示例 2：自定义运算，例如计算乘积**

```cpp
int product = accumulate(v.begin(), v.end(), 1, [](int a, int b){ return a * b; }); // product = 120
```

* **应用场景**：

  * 区间求和、求积
  * 累加某个条件下的值（结合 lambda）
  * 可用于统计区间内满足条件的元素数量，例如：

```cpp
int count = accumulate(v.begin(), v.end(), 0, [](int acc, int x){ return acc + (x % 2 == 0); });
// count = 偶数个数
```

---

## 3. `for_each` 用法

* **定义**（在 `<algorithm>` 头文件中）：

```cpp
template<class InputIt, class UnaryFunction>
UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction f);
```

* **参数**：

  * `first, last`：输入区间
  * `f`：对每个元素执行的函数，可捕获外部变量统计

* **示例：统计偶数个数**

```cpp
vector<int> v = {1,2,3,4,5};
int count = 0;
for_each(v.begin(), v.end(), [&](int x){
    if(x % 2 == 0) count++;
});
```

* **特点**：

  * 可以执行任意操作
  * 可以修改外部变量（通过 lambda 捕获）
  * 对统计复杂逻辑更灵活

---

## 4. `accumulate` vs `for_each` 区别

| 特性       | accumulate   | for_each        |
| -------- | ------------ | --------------- |
| 返回值      | 有（累加结果）      | 无（返回函数对象）       |
| 灵活性      | 受限（必须是可累加运算） | 高（任意操作，包括条件统计）  |
| 使用复杂条件统计 | 需要结合 lambda  | 直接在 lambda 内部处理 |
| 修改容器元素   | 不修改          | 可修改（通过引用）       |
| 易读性      | 简洁、直接        | 逻辑清晰但代码稍长       |

---

## 5. 注意事项与最佳实践

1. **初始值要合适**

   * 对整数求和，初始值通常为 `0`
   * 对乘积，初始值为 `1`

2. **避免副作用**

   * `accumulate` 的 lambda 应尽量无副作用（只返回新累加值）
   * 统计复杂逻辑时，`for_each` 可以捕获外部计数器

3. **避免使用全局变量**

   * 尽量使用 lambda 捕获局部变量，避免不必要的全局依赖

4. **性能**

   * 对大区间统计，`accumulate` 和 `for_each` 都是 O(n)
   * 区别主要在可读性和灵活性

---

## 6. 小结

* **`accumulate`**：适合 **简单累加/求和/求积** 的区间统计，简洁明了。
* **`for_each`**：适合 **复杂逻辑统计或条件统计**，灵活性高，可同时更新外部变量。
* 两者结合 lambda 函数使用，可以高效实现各种区间统计功能而不修改原容器。

