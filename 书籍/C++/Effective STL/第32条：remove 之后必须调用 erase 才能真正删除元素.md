# **第 32 条：`remove` 之后必须调用 `erase` 才能真正删除元素**

## **核心思想**

* STL 算法如 `std::remove`、`std::remove_if` **并不真的删除容器元素**。
* 它只是 **将不需要删除的元素移动到前面**，返回一个指向“新逻辑结尾”的迭代器。
* 元素在容器末尾仍然存在，占据空间，如果不调用 `erase`，容器大小不变。

> 这就是所谓的 **“erase-remove 惯用法”**。

---

## **1️⃣ remove 算法特点**

```cpp
auto new_end = std::remove(v.begin(), v.end(), value);
```

* 功能：

  * 遍历 `v`
  * 将 **不等于 value 的元素移动到前面**
  * 返回一个迭代器 `new_end`，指向逻辑上“新尾部”的下一个位置

* 注意：

  * 容器 `v` 的 `size()` **没有改变**
  * 元素只是被覆盖，末尾仍存在旧数据

---

## **2️⃣ erase 删除元素**

```cpp
v.erase(new_end, v.end());
```

* 功能：

  * 删除从 `new_end` 到容器末尾的元素
  * 真正改变容器大小

* 完整的 **erase-remove 惯用法**：

```cpp
v.erase(std::remove(v.begin(), v.end(), value), v.end());
```

✅ 这样才能真正删除 `value` 元素，缩小容器大小。

---

## **3️⃣ remove\_if 的使用**

* 可以配合谓词删除复杂条件的元素：

```cpp
v.erase(
    std::remove_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; }),
    v.end()
);
```

* 删除所有偶数元素
* 逻辑同样是：先 `remove_if` 移动不需要删除的元素 → 再 `erase` 删除末尾冗余元素

---

## **4️⃣ 常见误区**

| 错误做法                                      | 问题                                              |
| ----------------------------------------- | ----------------------------------------------- |
| `std::remove(v.begin(), v.end(), value);` | 元素仍在容器末尾，占据空间，`v.size()` 不变                     |
| 误以为 `remove` 会真正删除                        | `remove` 只是 **逻辑删除**，需要 `erase`                 |
| 对 `list` 使用 `remove`                      | `list` 提供成员函数 `list::remove`，可以直接删除，不需要 `erase` |

---

## **5️⃣ 总结**

1. **`remove` / `remove_if` 不改变容器大小**
2. **真正删除元素必须调用 `erase`**
3. **erase-remove 是 STL 常用模式**

```text
STL删除元素口诀：
remove先移位，erase再收尾。
```

---

💡 **要点提醒**

* `vector`、`deque`、`string` 等连续容器：使用 erase-remove
* `list` 或 `forward_list`：提供成员函数直接删除，无需 erase-remove
