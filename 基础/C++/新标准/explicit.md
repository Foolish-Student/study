# C++ explicit

`explicit` 用于限制隐式类型转换，可以修饰构造函数，以及 C++11 起的类型转换函数。

## 修饰构造函数

没有 `explicit` 时，可以通过构造函数把其他类型隐式转换成当前类：

```cpp
struct Number {
    Number(int v) : value(v) {}
    int value;
};

void print(Number n) {}

// 以下代码写在函数内部
Number a = 10;  // 隐式调用 Number(int)
print(20);      // 隐式构造 Number，再传给 print
```

加上 `explicit` 后，必须明确写出构造动作：

```cpp
struct Number {
    explicit Number(int v) : value(v) {}
    int value;
};

void print(Number n) {}

// 以下代码写在函数内部
Number a(10);              // 可以：直接初始化
Number b{10};              // 可以：直接列表初始化，C++11
Number c = Number(10);     // 可以：显式构造 Number
print(Number{20});         // 可以：显式构造后传参

// Number d = 10;          // 错误：拷贝初始化不能这样调用 explicit 构造函数
// Number e = {10};        // 错误：拷贝列表初始化不能使用选中的 explicit 构造函数
// print(20);              // 错误：不允许 int 隐式转换成 Number
// print({20});            // 错误：同样属于拷贝列表初始化
```

注意：`Number c = Number(10)` 中虽然有 `=`，但右侧已经是明确构造的 `Number`。`explicit` 限制的是通过构造函数进行的隐式转换，不是禁止所有带 `=` 的初始化。

## 不限于单参数构造函数

带默认参数、能够用一个实参调用的构造函数，也可能产生隐式转换：

```cpp
struct Buffer {
    explicit Buffer(int size, char fill = ' ') {}
};

Buffer b(10);       // 可以
// Buffer b = 10;   // 错误
```

C++11 起，多参数构造函数也可以用 `explicit`，区别主要体现在列表初始化：

```cpp
struct Point {
    explicit Point(int x, int y) {}
};

Point p{1, 2};        // 可以：直接列表初始化
// Point q = {1, 2};  // 错误：拷贝列表初始化
```

## 修饰类型转换函数（C++11）

构造函数控制“其他类型 → 当前类”，类型转换函数控制“当前类 → 其他类型”。

```cpp
struct Handle {
    int id;

    explicit operator bool() const {
        return id != 0;
    }
};

Handle h{1};
bool a = static_cast<bool>(h);  // 可以：显式转换
// bool b = h;                 // 错误：不允许这里的隐式转换
// int n = h;                  // 错误：不能经 bool 隐式转换为 int
```

类型转换函数写成 `operator 目标类型()`，前面不写返回类型。

`bool` 有一个常用例外：`if`、`while`、`!`、`&&`、`||` 等需要布尔条件的上下文，允许使用 `explicit operator bool()`：

```cpp
if (h) { /* 有效句柄 */ }  // 可以
bool empty = !h;           // 可以
```

因此，`explicit operator bool()` 常用于表达对象是否有效，同时避免对象意外参与整数运算。

## 条件 explicit（C++20）

`explicit(常量表达式)` 可以根据编译期条件决定是否启用：

```cpp
struct Number {
    explicit(true) Number(int) {}     // 等价于 explicit Number(int)
    explicit(false) Number(double) {} // 等价于 Number(double)
};
```

主要用于模板，让构造函数是否允许隐式转换取决于模板参数。

## 使用建议

如果构造函数表示“明确创建对象”，通常加 `explicit`；只有当“从该参数类型自然转换成当前类型”确实是接口设计的一部分时，才保留隐式转换。`explicit` 不会阻止构造函数参数自身的类型转换，例如 `explicit Number(int)` 仍可通过 `Number(3.5)` 调用；花括号初始化则另有禁止窄化转换的规则。
