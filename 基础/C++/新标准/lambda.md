# C++ lambda 表达式

lambda 是就地定义可调用对象的语法，常用于回调和标准库算法。C++11 起支持。

## 基本语法

```cpp
[捕获列表](参数列表) mutable -> 返回类型 {
    函数体
}
```

- `[]` 和 `{}` 必须保留；无参数的简单写法可以省略 `()`。
- 参数列表与普通函数一致，支持值、引用、指针等参数。
- 返回类型通常自动推导，也可以通过 `-> T` 指定；多个 `return` 的推导类型需要一致。
- `mutable` 可选，用于修改按值捕获的普通变量副本。

```cpp
auto add = [](int a, int b) { return a + b; };
int sum = add(2, 3);  // 5

// 显式返回类型
auto half = [](int x) -> double { return x / 2.0; };

// 创建后立即调用
int result = [](int x) { return x * x; }(4);  // 16
```

lambda 表达式产生闭包对象，每个 lambda 表达式都有独立的类型，通常用 `auto` 接收。

## 捕获列表

捕获用于访问外部局部变量；参数则是在调用时传入。

| 写法 | 含义 |
| --- | --- |
| `[]` | 不捕获 |
| `[x]` | 按值捕获 x，创建时保存副本 |
| `[&x]` | 按引用捕获 x，访问原变量 |
| `[x, &y]` | x 按值、y 按引用捕获 |
| `[=]` | 需要捕获的变量默认按值捕获 |
| `[&]` | 需要捕获的变量默认按引用捕获 |
| `[=, &x]` | 默认按值，x 按引用捕获 |
| `[&, x]` | 默认按引用，x 按值捕获 |
| `[this]` | 捕获当前对象的指针，通过它访问成员 |

```cpp
int x = 10;
auto byValue = [x]() { return x; };
auto byRef = [&x]() { return x; };

x = 20;
byValue();  // 10
byRef();    // 20
```

注意：

- 引用捕获不延长变量生命周期。返回或异步保存 lambda 时，要检查捕获对象是否仍然存活。
- `[this]` 不复制对象，也不延长对象生命周期。
- 全局变量、静态局部变量可以直接访问，不需要捕获。
- `[=, x]`、`[&, &x]` 是非法写法：显式捕获不能与默认捕获方式重复。

## mutable

默认情况下，闭包的调用运算符是 `const`，不能修改按值捕获的普通变量副本。`mutable` 去掉这一限制，但不会修改外部原变量。

```cpp
int n = 0;
auto next = [n]() mutable -> int { return ++n; };

next();  // 1
next();  // 2，同一个闭包对象保留修改后的状态
// 外部 n 仍为 0
```

引用捕获修改原变量不需要 `mutable`。语法顺序是：`参数列表 → mutable → 返回类型`。

## 作为算法参数

```cpp
#include <algorithm>
#include <vector>

std::vector<int> nums = {3, 1, 4, 2};
std::sort(nums.begin(), nums.end(), [](int a, int b) {
    return a > b;  // 降序：a 应排在 b 前面时返回 true
});
```

比较器需满足严格弱序，不能把这里的 `>` 换成 `>=`。

## 后续标准的常见扩展

```cpp
// C++14：泛型 lambda，参数类型在调用时推导
auto add = [](auto a, auto b) { return a + b; };

// C++14：初始化捕获，在创建闭包时计算并保存初始值
int x = 10;
auto get = [value = x + 1]() { return value; };

// C++17：在非静态成员函数中复制当前对象
// auto snapshot = [*this]() { return member; };
```

实际使用时，重点看清捕获方式和生命周期；其余部分与普通函数语法基本一致。
