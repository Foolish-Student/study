# 第45条：正确区分 count、find、binary_search、lower_bound、upper_bound、equal_range

## 1. 六个名字分别做什么？（一张表看懂）

| 名称              | 作用                                    | 典型使用场景                                   | 返回值           | 复杂度（典型）                                                 |
| --------------- | ------------------------------------- | ---------------------------------------- | ------------- | ------------------------------------------------------- |
| `find`          | 找到**一个**等价于 `key` 的元素                 | 关联容器成员函数；或 `<algorithm>` 的线性查找           | 迭代器或 `end()`  | 关联容器 `O(logN)`；算法版 `O(N)`                               |
| `count`         | 统计与 `key` 等价的元素**个数**                 | `set/map` 返回 0/1；`multiset/multimap` 可>1 | 整数            | `set/map` 为 `O(logN)`；`multi*` `O(logN + k)`；算法版 `O(N)` |
| `binary_search` | 只问“**是否存在**”                          | **已排序**的序列容器区间                           | `bool`        | `O(logN)`                                               |
| `lower_bound`   | 第一个 **`>= key`** 的位置                  | 已排序区间；关联容器成员函数                           | 迭代器           | `O(logN)`                                               |
| `upper_bound`   | 第一个 **`> key`** 的位置                   | 同上                                       | 迭代器           | `O(logN)`                                               |
| `equal_range`   | 同时得到 **`[lower_bound, upper_bound)`** | 同上，拿成段最方便                                | `pair<it,it>` | `O(logN)`（一次查找）                                         |

> 关联容器：`set/multiset/map/multimap`（有自己的成员函数；底层红黑树）。
> 序列容器：`vector/deque/list`（二分族算法仅在**已按同一比较器排序**时可用）。

---

## 2. 如何正确选择？（决策树）

1. **你在用关联容器吗（set/map/multi*）？**

   * ✅ 用**成员函数**：`find / count / lower_bound / upper_bound / equal_range`
   * ❌ 不要用 `<algorithm>` 的 `find/count` —— 那是**线性 O(N)**，浪费树结构优势。
2. **你在用已排序的序列容器吗（如排序后的 vector）？**

   * 只问有没有：`binary_search`
   * 要一个位置：`lower_bound`（再判 `*it == key`）
   * 要整段相等元素：`equal_range`（最省心）
   * ❌ 不要再用 `std::find/std::count`（线性）。
3. **需要“所有等于 key 的元素”**（尤其是 `multimap/multiset`）？

   * ✅ `equal_range`（一次查找同时得 `[l, r)`，后续遍历/删除更顺手）
   * 备选：`lower_bound + upper_bound`（两次查找，略慢）

---

## 3. “相等”到底看谁？（**等价≠`==`** 的大坑）

* 关联容器用**比较器**（默认 `std::less<T>`）定义“等价”：既非 `comp(a,b)` 也非 `comp(b,a)` ⇒ 等价。
* 这**不必等于** `operator==`。所以自定义比较器时要满足**严格弱序**，并与“相等”的直觉一致，否则会出现“看着相等却查不到”的错觉。
* 结论：**查找成功与否由比较器决定**；不要拿 `==` 来推断 `find/count` 的结果。

---

## 4. 模板用法（拿来即用）

### 4.1 关联容器（成员函数）

```cpp
// 1) 存在性 & 取一个元素迭代器
if (auto it = s.find(key); it != s.end()) {
    // use *it
}

// 2) 计数
int c = s.count(key); // set/map: 0 or 1；multi*: 可能>1

// 3) 等于 key 的完整区间（multi* 推荐）
auto [l, r] = ms.equal_range(key);
for (auto it = l; it != r; ++it) { /* ... */ }

// 删除所有 key（multi*）
ms.erase(l, r);

// 4) 单边界（常用于定位插入点或范围）
auto lb = s.lower_bound(key); // 第一个 >= key
auto ub = s.upper_bound(key); // 第一个 >  key
```

> C++20：`set/map::contains(key)` 等价于 `find != end()`，语义更清晰。

### 4.2 已排序序列容器（如 `vector<int> v`）

```cpp
sort(v.begin(), v.end()); // 确保与比较器一致的升序

// 是否存在
bool ok = binary_search(v.begin(), v.end(), key);

// 单个位置（若 *lb == key 则存在）
auto lb = lower_bound(v.begin(), v.end(), key);

// 区间 [==key]
auto [l, r] = equal_range(v.begin(), v.end(), key);
for (auto it = l; it != r; ++it) { /* ... */ }
```

---

## 5. 复杂度与性能建议

* 关联容器成员函数：对数 `O(logN)`；`multi*::count` 为 `O(logN + k)`（`k` 为匹配数量）。
* `<algorithm>` 中的 `find/count`：**线性 O(N)**（除非你本就想线性扫，否则避免）。
* `equal_range` 往往比 `lower_bound + upper_bound` 更快（一次树搜索得到两端）。
* 二分族算法要求**区间已按相同规则排序**，否则**未定义行为**（很隐蔽的雷）。

---

## 6. 典型易错点清单

1. 在 `set/map` 上写 `std::find(s.begin(), s.end(), key)` —— 复杂度从 `O(logN)` 退化到 `O(N)`。
2. 对未排序的 `vector` 调 `binary_search/lower_bound` —— **UB**（结果不可用）。
3. 自定义比较器与 `==` 含义不一致，导致“看似相等查不到”。
4. 在 `multimap/multiset` 上多次查找以统计/删除，忘了 `equal_range` 一步到位。
5. 查存在性时用 `lower_bound` 却没检查 `it != end() && !comp(key, *it)`。

---

## 7. 快速口令（背诵版）

* **关联系统用成员，序列排序用二分。**
* **要区间：`equal_range`；要位置：`find/lower_bound`；要存在：`binary_search/contains`。**
* **相等看比较器，别拿 `==` 判结果。**

