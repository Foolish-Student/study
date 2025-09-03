# 第 23 条笔记 —— 考虑用排序的 `vector` 替代关联容器

## 核心思想

* 在 C++ 中，常用关联容器有：

  * `set`, `map`, `multiset`, `unordered_set`, `unordered_map`
* 这些容器通常提供 **对数复杂度的插入/查找**（`O(log n)` 或均摊 `O(1)`）
* 但是在某些场景下，**排序的 `vector`** 可以替代关联容器，实现更高效的操作。

---

## 适用场景

1. **元素数量不大**（n 较小）
2. **插入操作少，查找/遍历操作多**
3. **可以提前一次性构建容器**，然后通过排序和二分查找进行操作

---

## 优势

1. **空间效率高**

   * `vector` 内存连续，额外开销少
   * 相比 `map`/`set` 节点指针和红黑树平衡开销，`vector` 更节省空间

2. **遍历效率高**

   * 顺序遍历 `vector` 利用缓存友好（cache-friendly）
   * 相比红黑树结构的 `set`/`map`，遍历速度快很多

3. **操作可控**

   * 可以用 `sort()` + `binary_search()` 实现高效查找
   * 用 `lower_bound()` / `upper_bound()` 实现区间操作

---

## 常用技巧

1. **初始化后排序**

```cpp
vector<int> v = {5, 2, 9, 1};
sort(v.begin(), v.end()); // 排序
```

2. **查找元素**

```cpp
if (binary_search(v.begin(), v.end(), 2)) {
    // 找到元素
}
```

3. **插入元素并保持有序**

```cpp
auto it = lower_bound(v.begin(), v.end(), 3);
v.insert(it, 3); // 插入并保持排序
```

4. **区间统计**

```cpp
int count = upper_bound(v.begin(), v.end(), 5) - lower_bound(v.begin(), v.end(), 2);
```

---

## 适用策略总结

* **小数据量 + 多查找/遍历 → 用排序 `vector`**
* **大数据量 + 插入/删除频繁 → 还是用关联容器**

---

💡 **总结一句话**：

> 当元素数量不大，且查找或遍历操作频繁时，使用排序的 `vector` 往往比 `set`/`map` 更高效，同时节省空间并利用缓存加速。
