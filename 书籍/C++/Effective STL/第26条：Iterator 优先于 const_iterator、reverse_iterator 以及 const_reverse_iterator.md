# 第26条：Iterator 优先于 const\_iterator、reverse\_iterator 以及 const\_reverse\_iterator

**核心思想**：在使用 STL 容器时，**普通迭代器（iterator）通常优先于 const\_iterator、reverse\_iterator 和 const\_reverse\_iterator**，原因是普通迭代器在性能和灵活性上通常更优，除非明确需要只读或反向遍历。

---

## 1️⃣ 迭代器类型概览

| 类型                           | 是否可修改容器元素 | 遍历方向 | 典型使用场景       |
| ---------------------------- | --------- | ---- | ------------ |
| **iterator**                 | ✅ 可以修改    | 正向   | 默认选择，性能最好    |
| **const\_iterator**          | ❌ 只读      | 正向   | 容器只读访问，防止误修改 |
| **reverse\_iterator**        | ✅ 可以修改    | 反向   | 需要从尾部到头部遍历   |
| **const\_reverse\_iterator** | ❌ 只读      | 反向   | 只读反向遍历       |

---

## 2️⃣ 使用建议

1. **默认使用 `iterator`**

   * 当你需要修改容器元素或只是顺序遍历时，使用普通 `iterator`。
   * 性能最高，迭代操作最直接。

2. **使用 `const_iterator`**

   * 当函数或算法只需读取容器，不允许修改时。
   * 可以保证安全性，避免意外修改。

3. **使用 `reverse_iterator` / `const_reverse_iterator`**

   * 仅在必须从尾部向头部遍历时使用。
   * 注意，反向迭代器通常性能略低于正向迭代器，因为实现上需要做偏移转换。

4. **不要滥用只读或反向迭代器**

   * 如果不需要只读或反向功能，优先使用普通迭代器，减少额外开销。

---

## 3️⃣ 示例

```cpp
#include <vector>
#include <iostream>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3, 4, 5};

    // 普通迭代器：可修改
    for (auto it = v.begin(); it != v.end(); ++it) {
        *it += 1; // 可以修改
    }

    // const_iterator：只读
    for (auto it = v.cbegin(); it != v.cend(); ++it) {
        // *it += 1; // ❌ 编译错误，只读
        cout << *it << " ";
    }
    cout << endl;

    // reverse_iterator：可修改，反向遍历
    for (auto rit = v.rbegin(); rit != v.rend(); ++rit) {
        *rit *= 2;
    }

    // const_reverse_iterator：只读，反向遍历
    for (auto rit = v.crbegin(); rit != v.crend(); ++rit) {
        cout << *rit << " ";
    }
    cout << endl;

    return 0;
}
```

**总结**：

* 默认使用普通迭代器（iterator）以获得最佳性能。
* 只有在需要只读访问或反向遍历时，才使用 const\_iterator、reverse\_iterator 或 const\_reverse\_iterator。
* 保持迭代器选择与实际需求一致，避免不必要的性能损耗。
