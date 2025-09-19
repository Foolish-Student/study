# **第33条：对包含指针的容器使用 `remove` 类算法时要特别小心**

## **核心思想**

* 如果容器中存放的是 **原始指针（raw pointer）**，例如：

```cpp
std::vector<Foo*> v;
```

* 使用 `std::remove` / `std::remove_if` **只会移动指针本身**，并不会释放指针指向的对象。

> 换句话说，`remove` 仅改变容器内指针的位置，而不会调用 `delete` 或析构对象。

---

## **1️⃣ 典型问题**

```cpp
std::vector<Foo*> v = {new Foo, new Foo, new Foo};
auto new_end = std::remove(v.begin(), v.end(), v[0]);
v.erase(new_end, v.end());
```

* `v[0]` 指针被移出容器
* **对象仍然在堆上存在** → 内存泄漏
* 如果使用智能指针（如 `std::unique_ptr`）则会自动释放，但原始指针必须手动 delete

---

## **2️⃣ 安全做法**

### 2.1 删除对象 + 移除指针

```cpp
for (Foo* p : v) {
    if (/* p 需要删除 */) delete p;
}
v.erase(
    std::remove(v.begin(), v.end(), /* 指针 */),
    v.end()
);
```

* 先释放对象
* 再调用 `erase-remove` 删除指针

### 2.2 使用智能指针

* 使用 `std::unique_ptr<Foo>` 或 `std::shared_ptr<Foo>` 替代原始指针
* `remove` + `erase` 会自动释放对象，无需手动 delete

```cpp
std::vector<std::unique_ptr<Foo>> v;
// erase-remove 自动调用析构
v.erase(
    std::remove_if(v.begin(), v.end(),
        [](const std::unique_ptr<Foo>& p){ return 条件; }),
    v.end()
);
```

* 安全且不易出错

---

## **3️⃣ 常见误区**

| 错误做法                                             | 问题                                                     |
| ------------------------------------------------ | ------------------------------------------------------ |
| `std::remove(v.begin(), v.end(), p)` 后直接 `erase` | 仅删除指针，**不会 delete 对象** → 内存泄漏                          |
| 对智能指针不理解 `remove` 行为                             | 对 `std::unique_ptr` 执行 remove + erase 是安全的，因为对象析构会自动调用 |

---

## **4️⃣ 核心总结**

1. **原始指针容器 + remove** → 容器指针移动，堆对象仍在 → 可能内存泄漏
2. **处理方法**：

   * 手动 delete 对象，再 erase 指针
   * 使用智能指针，自动管理生命周期
3. **erase-remove 适用场景**：

   * 连续容器（vector、deque、string）
   * 非指针元素更安全

---

💡 **一句口诀**

> **“remove只搬指针，析构需自己动；智能指针保安全，erase-remove更放心。”**
