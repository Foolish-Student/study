# 第 22 条：切勿直接修改 `set` 或 `multiset` 中的键

## 背景

在 C++ STL 中：

* `std::set` 和 `std::multiset` 是基于 **红黑树（RB-tree）** 或 **平衡二叉搜索树** 实现的
* **每个元素的值都是节点的“键”**，用于维持容器内元素的 **有序性**

```cpp
std::set<int> s = {1, 2, 3};
```

---

## 核心要求

**切勿直接修改 set 或 multiset 中的键值**，例如：

```cpp
auto it = s.find(2);
*it = 5;  // ❌ 不允许，可能破坏内部结构
```

### 为什么？

1. **内部顺序依赖键值**

   * `set`/`multiset` 的底层是平衡树，节点位置是通过键值计算的
   * 修改键值后：

     * 节点可能不再满足树的顺序性
     * 导致容器结构破坏，出现未定义行为

2. **容器可能无法重新排序**

   * 红黑树不会自动调整节点位置
   * 结果可能是：

     * `find`、`lower_bound`、`upper_bound` 查找失败
     * 排序顺序混乱
     * 插入/删除操作异常

---

## 正确做法

1. **删除 + 插入**（推荐方式）：

```cpp
auto it = s.find(2);
if (it != s.end()) {
    s.erase(it);  // 删除旧值
    s.insert(5);  // 插入新值
}
```

2. **修改副本再插入**

   * 不直接修改 set 元素
   * 修改一个临时变量，然后 `erase` 原值，`insert` 新值

---

## 示例

✅ **正确做法**：

```cpp
#include <set>
#include <iostream>
using namespace std;

int main() {
    set<int> s = {1, 2, 3};
    auto it = s.find(2);
    if (it != s.end()) {
        int newVal = *it + 3;  // 修改副本
        s.erase(it);           // 删除旧键
        s.insert(newVal);      // 插入新键
    }

    for (auto v : s) cout << v << " ";  // 输出：1 3 5
}
```

❌ **错误做法**：

```cpp
auto it = s.find(2);
*it = 5;  // 直接修改键值，未定义行为
```

---

## 总结要点

1. **set/multiset 的元素是不可修改的**，因为它们是“键”
2. **想修改值** → 先 `erase` 再 `insert`
3. 避免直接修改，防止破坏 **有序性和内部平衡**
4. **原则**：set/multiset 里的元素 **只读**，所有修改都应通过删除和重新插入实现

---

**一句话记忆法**：

> **“树上的钥匙不许动，动了就乱套，想换就删掉再挂上新的。”**
