# 第7条：容器里放"new 出来的指针"，销毁前要负责释放

## 🎯 核心观点

如果容器中存放的是通过 `new` 创建的**裸指针**（如 `vector<Foo*>`），那么**在容器销毁（或清空）前，必须逐个 `delete` 指针**，否则会内存泄漏。更现代也更稳妥的做法是：**优先存放智能指针**（如 `unique_ptr` / `shared_ptr`），把资源释放交给 RAII。

---

## ✅ 推荐写法（C++11+，首选）

```cpp
#include <vector>
#include <memory>

std::vector<std::unique_ptr<Foo>> v;
v.emplace_back(std::make_unique<Foo>());
v.emplace_back(std::make_unique<Foo>());
// 离开作用域或 v.clear() 时自动释放，无需手动 delete
```

*RAII 自动管理生命周期，避免手动释放出错。*

---

## ✅ 推荐写法（不得不存裸指针时）

```cpp
std::vector<Foo*> v;
v.push_back(new Foo());
v.push_back(new Foo());

// 在容器销毁/清空前，务必逐个 delete
for (Foo* p : v) delete p;
v.clear();
```

或用算法更简洁：

```cpp
std::for_each(v.begin(), v.end(), [](Foo* p){ delete p; });
v.clear();
```

*先 `delete`，再 `clear()`；`clear()` 只销毁"指针值"，不负责释放所指对象。*

---

## ❌ 不推荐写法

```cpp
std::vector<Foo*> v;
v.push_back(new Foo());
v.push_back(new Foo());

v.clear();   // 仅清空指针，不会 delete！→ 泄漏
// 或者什么都不做就让 v 析构 → 同样泄漏
```

*别指望容器替你释放指针所指向的对象。*

---

## 🛠 为什么要这么做？

1. **所有权问题（Ownership）**
   裸指针不表达所有权，容器并不知道要不要负责释放；如果你的设计是"容器拥有这些对象"，那就必须在容器生命周期结束前释放它们，或改用智能指针把所有权语义写清楚。

2. **异常与可维护性**
   手动 `delete` 容易遗漏，尤其是存在早退/异常路径时；智能指针用 RAII 自动托底，更安全、更易维护。

3. **避免二次释放**
   多个容器/对象"共享同一裸指针"容易出现 double delete；若需共享，使用 `shared_ptr`，由引用计数协调销毁时机。

---

## 📦 常见场景与做法

| 场景               | 更佳做法 ✅                                | 说明         |
| ---------------- | ------------------------------------- | ---------- |
| 容器**独占**对象       | `vector<unique_ptr<T>>`               | 最简单安全，默认选择 |
| 多处**共享**对象       | `vector<shared_ptr<T>>`               | 明确共享所有权    |
| 历史代码/第三方接口只能给裸指针 | 进容器前包一层 `unique_ptr`；或在清空前统一 `delete` | 逐步迁移到智能指针  |

（如果对象可复制/移动且不大，**优先直接存值**：`vector<T>`，根本不需要指针。）

---

## ✅ 实用建议

* **能不用裸指针就不用**；优先 `unique_ptr`，需要共享再用 `shared_ptr`。
* 如果暂时只能用裸指针：**建立明确的释放约定**（封装成容器管理类/辅助函数），避免遗漏。
* 清空容器前：**先 `delete` 后 `clear()`**；不要只 `clear()`。
* 跨容器共享同一裸指针容易踩雷；要共享就用 `shared_ptr`。

---

## 📌 小结

**容器不会替你 `delete` 裸指针。**
要么在销毁前**亲自释放**，要么用 **智能指针交给 RAII**。这条看似"老生常谈"，却是避免泄漏和维护噩梦的关键。
