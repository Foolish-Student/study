# 第46条考虑使用函数对象，而不是普通函数，作为 STL 算法的参数

## 🧩 一、问题背景

STL 算法（如 `sort`、`find_if`、`for_each`、`count_if` 等）经常需要“比较”、“判断”、“操作”等逻辑参数。

这些逻辑既可以通过：

* 普通函数（function pointer）传递；
* 也可以通过**函数对象（functor）**传递。

例如：

```cpp
bool isEven(int x) { return x % 2 == 0; }
sort(vec.begin(), vec.end(), isEven);  // 使用普通函数
```

或者：

```cpp
struct IsEven {
    bool operator()(int x) const { return x % 2 == 0; }
};
sort(vec.begin(), vec.end(), IsEven());  // 使用函数对象
```

条款 46 告诉我们：
👉 **优先使用函数对象而不是函数指针**。

---

## ⚙️ 二、函数对象（Function Object）是什么

**函数对象**是一个“可以像函数一样被调用的对象”。
它通常是重载了 `operator()` 的类。

```cpp
struct LessThan {
    bool operator()(int a, int b) const {
        return a < b;
    }
};
```

于是我们可以：

```cpp
LessThan lt;
if (lt(3, 5)) cout << "true";
```

这种对象的本质是**可调用对象 + 状态**。

---

## 💡 三、为什么函数对象比函数更好

### ✅ 1. 性能优化：内联（Inlining）

STL 算法通常定义为模板函数。
如果参数是函数对象，编译器可以**内联调用**，从而**消除函数调用开销**。

但如果传入函数指针：

* 函数地址在编译期未知；
* 编译器无法内联；
* 每次调用都要跳转一次。

例如：

```cpp
sort(vec.begin(), vec.end(), compare);  // compare 是普通函数
```

相比：

```cpp
sort(vec.begin(), vec.end(), Compare());  // Compare 是函数对象，可内联
```

函数对象版本在大循环中可显著更快。

---

### ✅ 2. 函数对象可以携带状态（Stateful）

普通函数是“无状态的”，只能依靠全局变量或外部传参。

函数对象可以通过成员变量携带额外信息。

例如：

```cpp
struct GreaterThan {
    int threshold;
    GreaterThan(int t) : threshold(t) {}
    bool operator()(int x) const { return x > threshold; }
};
```

这样我们可以：

```cpp
count_if(vec.begin(), vec.end(), GreaterThan(10));
```

相比之下，用普通函数就得写成：

```cpp
int g_threshold;
bool greaterThan(int x) { return x > g_threshold; }
```

全局变量不仅不安全，还不灵活。

---

### ✅ 3. 更灵活的模板匹配与类型推导

函数对象是模板类型的一部分，STL 能根据它自动推导模板参数类型。
而函数指针往往需要额外的类型声明（例如 `bool(*cmp)(int,int)`），显得笨拙。

---

### ✅ 4. 可与 `bind` 或 `lambda` 搭配使用

函数对象天生能与现代 C++ 的 `std::bind` 或 lambda 完美结合，形成闭包。
例如：

```cpp
auto f = std::bind(std::greater<int>(), std::placeholders::_1, 10);
count_if(vec.begin(), vec.end(), f);
```

或者：

```cpp
count_if(vec.begin(), vec.end(), [](int x){ return x > 10; });
```

其实 lambda 本质上也是一种匿名函数对象。

---

## 🔍 四、总结归纳

| 对比点      | 普通函数   | 函数对象                  |
| -------- | ------ | --------------------- |
| **内联优化** | ❌ 无法内联 | ✅ 编译期可内联              |
| **携带状态** | ❌ 不可携带 | ✅ 可通过成员变量保存           |
| **模板推导** | 需手动指定  | ✅ 自动推导类型              |
| **可组合性** | 较差     | ✅ 可与 bind / lambda 结合 |
| **性能表现** | 稍慢     | ✅ 更快                  |

---

## ✅ 五、总结语

> **条款 46 核心要点：**
>
> * 在 STL 算法中，**优先使用函数对象**（functor）而不是普通函数；
> * 因为函数对象能：
>
>   * 被内联，提升性能；
>   * 携带状态，更灵活；
>   * 与模板系统天然兼容；
>   * 是现代 C++ lambda 的基础形式。

一句话总结：

> 💬 **函数对象既快又灵活，是现代 C++ 算法参数的首选。**
