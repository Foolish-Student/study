# shared_ptr 是否线程安全？

> 结论先行：**`std::shared_ptr` 的控制块（引用计数）是线程安全的，但它指向的对象本身不是线程安全的。** 这两件事必须分开看，是很多人踩坑的地方。

---

## 一、三种情况，逐个说清楚

### ✅ 情况一：多个线程各自持有「副本」，同时拷贝/析构 → 安全

```cpp
auto sp = std::make_shared<Widget>();

// 线程 A、B、C 各自拷贝一份，然后各自析构
std::thread t1([sp]() { auto a = sp; /* ... */ });
std::thread t2([sp]() { auto b = sp; /* ... */ });
```

这里每个线程操作的是**自己那一份 `shared_ptr`**，它们共享的是同一个控制块里的 `ref_count`。因为 `ref_count` 是 `std::atomic`，所以 +1/-1 都是原子的，不会算错，对象也只会被销毁一次。

> **这就是"线程安全"的真正含义**——也是实现里引用计数要用 `std::atomic<long>` 而不是普通 `long` 的原因。

### ❌ 情况二：多个线程改同一个对象 → 不安全

```cpp
auto sp = std::make_shared<Widget>();

std::thread t1([sp]() { for (int i = 0; i < 10000; ++i) sp->counter++; });
std::thread t2([sp]() { for (int i = 0; i < 10000; ++i) sp->counter++; });
```

`sp->counter++` 没有加锁，两个线程同时读写 `counter`，会发生**数据竞争（data race）**，结果是未定义行为。`shared_ptr` 保护不了这个——它只保证"对象不会被提前销毁"，不保证"对象内部的访问安全"。

**解决**：给 `counter` 加锁、用 `std::atomic<int>`，或把 `Widget` 设计成不可变对象。

### ❌ 情况三：同一个 `shared_ptr` 变量被多线程同时「赋值/重置」→ 不安全

```cpp
shared_ptr<Widget> sp = make_shared<Widget>();   // 全局共享的一个变量

std::thread t1([&]() { sp = make_shared<Widget>(); });   // 改写 sp
std::thread t2([&]() { auto x = sp; });                  // 读 sp
```

这里读写的是**同一个 `sp` 变量本身**（不是副本），属于对同一内存的并发读写，没同步就是 data race。

**解决**：用 `std::atomic<std::shared_ptr<T>>`（C++20 起提供专门的原子特化），或者用互斥锁保护。

---

## 二、一句话总结

| 场景 | 安全吗 |
|---|---|
| 多线程各自持副本、只做拷贝/析构 | ✅ 安全（引用计数是原子的） |
| 多线程改 `shared_ptr` 指向的对象 | ❌ 不安全（要自己加锁） |
| 多线程读写同一个 `shared_ptr` 变量 | ❌ 不安全（用 `atomic<shared_ptr>` 或加锁） |

**口诀**：`shared_ptr` 只保证「引用计数」这一件事是线程安全的，**它管不了对象内部，也管不了同一个变量被并发读写**。
