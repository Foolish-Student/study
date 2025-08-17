# 第8条：不要在容器中使用 `auto_ptr`（用 `unique_ptr` / `shared_ptr` 替代）

## 🎯 核心观点

`auto_ptr` 的**拷贝会转移所有权**（源指针被置空），这与 STL 容器和算法的"值语义/可复制"设计**严重冲突**。因此**不要**把 `auto_ptr` 放进容器。现代 C++ 已经提供更安全的智能指针：**独占用 `unique_ptr`，共享用 `shared_ptr`**。

---

## ✅ 推荐写法（现代 C++）

```cpp
#include <vector>
#include <memory>

std::vector<std::unique_ptr<Foo>> v;
v.emplace_back(std::make_unique<Foo>());
v.emplace_back(std::make_unique<Foo>());
// 移动语义 + RAII，安全、可预期
```

如需共享所有权：

```cpp
std::vector<std::shared_ptr<Foo>> v2;
v2.emplace_back(std::make_shared<Foo>());
```

---

## ❌ 不推荐写法（踩坑示例）

```cpp
// C++03 时代的坑；在新标准中通常直接禁止
std::vector<std::auto_ptr<Foo>> v;
v.push_back(std::auto_ptr<Foo>(new Foo));
v.push_back(v[0]);      // 复制时发生"所有权转移"，v[0] 被置空！
```

问题：容器内部的拷贝/赋值（如 `push_back`、`insert`、`assign`、排序、重分配等）会**悄悄把元素变成空指针**，行为难以推断，极易出错。

---

## 🛠 为什么会这样？

1. **语义不匹配**：
   容器和算法默认按"拷贝=两个值各自独立"的值语义工作；
   而 `auto_ptr` 的拷贝=**偷走资源**（源变空），违背预期。

2. **可用性差**：
   许多算法/操作会复制元素；一旦复制，原位置变空，导致**遍历中遇到空指针**、排序/稳定性被破坏等。

3. **现代替代品更好**：
   `unique_ptr` **不可拷贝、可移动**，容器会用**移动**来转移所有权，语义明确且安全；
   `shared_ptr` 以**引用计数**共享资源，复制=共享，不会把源置空。

---

## 📦 常见场景与正确用法

| 需求        | 用法 ✅                     | 说明                                        |
| --------- | ------------------------ | ----------------------------------------- |
| 独占所有权     | `vector<unique_ptr<T>>`  | 默认首选；`erase/clear` 时自动释放                  |
| 多处共享      | `vector<shared_ptr<T>>`  | 复制即共享；注意循环引用                              |
| 自定义释放动作   | `unique_ptr<T, Deleter>` | 传入删除器类型，容器照样管理                            |
| 历史接口产出裸指针 | 立刻封进智能指针                 | `v.emplace_back(std::unique_ptr<T>(raw))` |

> 如果对象体量不大且可移动/复制，**优先直接存值**：`vector<T>`，最简单也最稳。

---

## ✅ 实用建议

* **不要在容器中使用 `auto_ptr`**（已弃用/移除）。
* 优先 `unique_ptr`；确需共享再用 `shared_ptr`。
* 使用 `std::make_unique` / `std::make_shared`，减少内存泄漏与异常风险。
* 对资源释放时机要清晰：容器元素被移除或容器销毁时，智能指针会**自动释放**所拥有的对象。

---

## 📌 小结

**能用 `unique_ptr`/`shared_ptr`，绝不用 `auto_ptr`！**
这既符合容器与算法的预期语义，也能让代码在所有权与生命周期上**可读、可控、可维护**。 🚀
