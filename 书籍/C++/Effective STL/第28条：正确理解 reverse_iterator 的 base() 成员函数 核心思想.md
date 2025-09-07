# **第28条：正确理解 reverse\_iterator 的 base() 成员函数**

## **核心思想**

* STL 的 `reverse_iterator` 是一个适配器，逆序遍历容器。
* `reverse_iterator::base()` 返回一个 **对应的普通 iterator**，但有一点容易误解：

> `base()` 返回的 iterator **指向 reverse\_iterator 当前元素的“下一个位置”**，不是它本身。

---

## **概念解释**

假设有一个容器：

```
容器: [A, B, C, D, E]
普通迭代器: 0  1  2  3  4
```

* `rit = reverse_iterator(end())`

  * `rit` 指向最后一个元素 E（逆序第一个元素）
  * `rit.base()` 返回 `end()`，即普通迭代器指向容器末尾的下一个位置
* `rit = reverse_iterator(begin())`

  * 迭代器指向容器开头的元素 A（逆序最后一个元素）
  * `rit.base()` 返回 `begin()`

> **口诀理解**：`base()` 永远指向 **“当前逆序元素的下一个位置”**

---

## **使用方法**

### 1️⃣ 反向遍历 + 插入或删除元素

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};
auto rit = std::find(v.rbegin(), v.rend(), 3); // 逆序查找元素 3

// 想要在普通迭代器上操作：
auto it = rit.base(); // it 指向 4（3 的下一个元素）
v.insert(it, 99);     // 在 3 之后插入 99
```

* 注意：**如果想在 `rit` 所指元素前插入，需要使用 `rit.base()`**
* 删除也是类似：

```cpp
v.erase((rit + 1).base()); // 删除 rit 所指的元素
```

---

### 2️⃣ 与普通迭代器配合算法

* 反向迭代器和普通迭代器在 STL 算法中不能直接混用
* 通过 `base()` 可以转换回普通 iterator 进行算法操作

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};
auto rit = std::find(v.rbegin(), v.rend(), 2);
std::vector<int>::iterator it = rit.base();
std::cout << *it; // 输出 3（下一个元素）
```

> 注意：这里输出的不是 2，而是 2 的下一个元素 3，这是 reverse\_iterator 的特性。

---

## **关键点总结**

| 特性         | 说明                                            |
| ---------- | --------------------------------------------- |
| base() 返回值 | 对应的普通 iterator                                |
| 指向位置       | 当前 reverse\_iterator 所指元素的下一个位置               |
| 插入操作       | `v.insert(rit.base(), value)` → 在 rit 所指元素前插入 |
| 删除操作       | `v.erase((rit + 1).base())` → 删除 rit 所指元素     |
| 算法配合       | 需要转换为普通 iterator 使用 STL 算法                    |

---

## **常见误区**

1. **误解 base() 指向当前元素**

   * 实际指向 **下一个元素**
   * 插入/删除必须注意偏移

2. **直接使用 rit 与普通 iterator 算法混用**

   * 必须用 `base()` 转换

3. **rit = rbegin(); base() != begin()**

   * rbegin() 对应 end() 的位置
   * rend() 对应 begin() 的位置

---

✅ **理解口诀**

> **“反向迭代，base 指下一个；操作容器，插入删除需 +1。”**

* 反向迭代器指向 X，`base()` 指向 X 的下一个
* 插入：`insert(rit.base(), val)` → 在 X 前
* 删除：`erase((rit + 1).base())` → 删除 X

