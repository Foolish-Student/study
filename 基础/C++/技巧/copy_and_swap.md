# Copy-and-Swap（拷贝并交换）

给一个持有资源的对象写拷贝赋值时，可以**先拷贝出临时对象，再和左边交换资源**。交换完成后，临时对象持有左边原来的资源，由它的析构函数负责清理。

## 例子

```cpp
#include <cassert>
#include <utility>

class Number {
public:
    explicit Number(int value) : value_(new int(value)) {}
    Number(const Number& other) : Number(*other.value_) {} // 深拷贝
    ~Number() { delete value_; }

    Number& operator=(const Number& other) {
        Number tmp(other); // 先拷贝右边的数据
        swap(tmp);         // 左边接收新数据，tmp 接收旧数据
        return *this;      // tmp 析构，释放旧数据
    }

    void swap(Number& other) noexcept {
        std::swap(value_, other.value_); // 只交换两个指针
    }

    int get() const { return *value_; }

private:
    int* value_;
};

int main() {
    Number a(1), b(2);
    a = b;
    assert(a.get() == 2 && b.get() == 2);
}
```

执行 `a = b` 时，`tmp` 先分配自己的内存并存入 `2`。交换后，`a` 持有这块新内存，`tmp` 持有原来存着 `1` 的内存。如果拷贝时分配失败，代码还没执行到 `swap`，所以 `a` 仍是 `1`。自赋值 `a = a` 也能正常工作，因为拷贝发生在释放旧资源之前。

## 教程里常见的简写

可以把临时对象写成按值参数，用下面的版本替换上面的 `operator=`。

```cpp
Number& operator=(Number tmp) {
    swap(tmp);
    return *this;
}
```

调用 `a = b` 时，进入函数体之前就会从 `b` 拷贝构造参数 `tmp`。如果类型提供了移动构造，传入右值时也可以移动构造这个参数。实现 `swap` 时直接交换成员，避免用 `std::swap(*this, tmp)` 再次触发赋值。

## 什么时候用

手写 `operator=`，并且赋值既要接住新资源，又要释放旧资源时，可以考虑这两种情况。

- **复制可能失败。** 例如复制缓冲区时需要分配内存。先构造临时副本，成功后再用不会抛异常的 `swap` 替换原值。这样复制失败时，左边的对象还能照常使用。
- **释放旧资源可能使右侧失效。** 例如 `p = p->next`，`next` 是旧节点的成员。先拷贝 `next` 再交换，等旧节点析构时，`p` 已经持有下一个节点。[项目里的赋值测试](../../../Code/C++/智能指针/test_shared_ptr.cpp#L165)

如果逐个给成员赋值就符合需求，例如成员都是 `int`、`std::string` 或 `std::vector`，通常用编译器生成的赋值运算符就够了。对于大缓冲区，Copy-and-Swap 要先构造一份副本，能复用现有内存时可以省掉一次分配。
