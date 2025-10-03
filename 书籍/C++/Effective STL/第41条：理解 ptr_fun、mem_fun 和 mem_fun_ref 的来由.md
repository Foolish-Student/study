# **第 41 条：理解 `ptr_fun`、`mem_fun` 和 `mem_fun_ref` 的来由**

## **1. 背景：STL 算法与函数**

* STL 算法（如 `transform`、`for_each`、`count_if`）需要**可调用对象（Callable）**，通常是函数指针或函数子（Functors）。
* 早期 C++ 面临一个问题：

  1. 普通函数指针可以直接使用。
  2. 但是类成员函数 **不能直接当作函数指针使用**，因为它们需要一个对象来调用。
* 因此出现了 `ptr_fun`、`mem_fun` 和 `mem_fun_ref`，用于**将函数或成员函数包装成可以被 STL 算法使用的函数对象**。

---

## **2. ptr_fun**

* **用途**：把普通的函数指针转换成函数对象，使其可以直接用于 STL 算法。
* **例子**：

```cpp
bool isOdd(int x) { return x % 2 == 1; }

std::vector<int> v = {1,2,3,4};
auto it = std::count_if(v.begin(), v.end(), std::ptr_fun(isOdd));
```

* 这里 `ptr_fun` 将 `isOdd` 转成可调用的对象，满足算法要求。

---

## **3. mem_fun**

* **用途**：把指向对象成员函数的指针（非静态成员函数）转换成函数对象。
* **特点**：可以接收 **指针对象** 作为参数。
* **例子**：

```cpp
struct Foo {
    bool isPositive() const { return true; }
};
std::vector<Foo*> v;
// 将成员函数包装成函数对象
auto f = std::mem_fun(&Foo::isPositive);
std::count_if(v.begin(), v.end(), f);
```

* `mem_fun` 让算法能直接对成员函数进行操作。

---

## **4. mem_fun_ref**

* **用途**：类似 `mem_fun`，但是针对 **对象引用** 而不是指针。
* **例子**：

```cpp
std::vector<Foo> v;
// v 存储的是对象而不是指针
auto f = std::mem_fun_ref(&Foo::isPositive);
std::count_if(v.begin(), v.end(), f);
```

* `mem_fun_ref` 解决了成员函数和引用类型对象的兼容问题。

---

## **5. 这些函数的来由**

* 早期 C++ 不支持 lambda 表达式，STL 算法要求可调用对象。
* 但类成员函数 **和普通函数指针不一样**：

  * 需要对象（或者对象指针）来调用。
  * 无法直接传给算法。
* 因此引入：

  * `ptr_fun` → 包装普通函数指针
  * `mem_fun` → 包装对象指针的成员函数
  * `mem_fun_ref` → 包装对象引用的成员函数

---

## **6. 现代 C++ 替代方案**

* 在 C++11 以后，**lambda 表达式**和 `std::function` 更常用。
* 上面的例子可以用 lambda 代替：

```cpp
// ptr_fun 替代
std::count_if(v.begin(), v.end(), [](int x){ return x % 2 == 1; });

// mem_fun 替代
std::count_if(v.begin(), v.end(), [](Foo* p){ return p->isPositive(); });

// mem_fun_ref 替代
std::count_if(v.begin(), v.end(), [](Foo& obj){ return obj.isPositive(); });
```

* lambda 更灵活，可读性也更高，不再依赖这些老式包装函数。

---

## **7. 总结**

* **核心问题**：STL算法需要可调用对象，但普通成员函数不能直接作为可调用对象。
* **解决方案**：

  * `ptr_fun` → 包装普通函数指针
  * `mem_fun` → 包装对象指针的成员函数
  * `mem_fun_ref` → 包装对象引用的成员函数
* **现代替代**：C++11+ 用 lambda 表达式，写法更直观、灵活、类型安全。
