# **第40条：若一个类是函数子，则应使它可配接（Composable）**

## **1. 背景：什么是函数子（Function Object）**

* **函数子（Function Object 或 Functor）** 是定义了 `operator()` 的类，可以像函数一样被调用。
* 函数子的优势：

  1. 可以保存状态（例如阈值、计数器）
  2. 可以像普通函数一样传给 STL 算法

**例子**：

```cpp
struct Multiply {
    int factor;
    Multiply(int f) : factor(f) {}
    int operator()(int x) const { return x * factor; }
};
```

---

## **2. 什么是可配接（Composable）**

* 可配接意味着你可以将一个函数子和另一个函数子组合起来，形成新的函数子。
* 简单来说，就是可以用 **一个函数子的输出作为另一个函数子的输入**。
* 这样，你可以把简单的操作链起来，形成更复杂的操作，而不需要修改原函数子。

---

## **3. 为什么函数子要可配接**

* **STL 算法和函数式编程**常常需要组合操作：

  * 比如 `transform(v.begin(), v.end(), v.begin(), f(g(x)))`
  * 如果函数子不可组合，就必须写额外的函数或 lambda，代码不够灵活
* 可配接函数子可以提高 **代码复用性** 和 **可读性**

---

## **4. 函数子可配接的设计原则**

1. **接口统一**

   * 所有可组合的函数子应该有兼容的输入输出类型
2. **保持轻量**

   * 避免内部状态过重或复杂，确保组合后依然高效
3. **支持链式组合**

   * 可以通过函数组合器或者手动写 `operator()` 链接多个函数子

---

## **5. 示例：函数子配接**

```cpp
struct Add {
    int value;
    Add(int v) : value(v) {}
    int operator()(int x) const { return x + value; }
};

struct Multiply {
    int factor;
    Multiply(int f) : factor(f) {}
    int operator()(int x) const { return x * factor; }
};

// 配接函数子
struct AddThenMultiply {
    Add add;
    Multiply mult;
    AddThenMultiply(int a, int m) : add(a), mult(m) {}
    int operator()(int x) const { 
        return mult(add(x));  // 先加再乘
    }
};
```

* `AddThenMultiply(2,3)(5)` → `(5+2)*3 = 21`
* 原理就是把两个函数子组合在一起，形成新的操作

---

## **6. 总结**

* 可配接函数子允许你把简单操作组合成复杂操作，提高灵活性和复用性
* 设计函数子时：

  1. 保证输入输出类型可匹配
  2. 保持轻量和无副作用
  3. 支持链式调用或组合

**一句话理解**：函数子不只是可调用，它还应该能和其他函数子组合，形成更复杂的功能，这是 STL 高效设计和函数式风格的基础。
