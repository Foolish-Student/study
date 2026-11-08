// test_shared_ptr.cpp —— 验证教学版 shared_ptr 行为是否正确
#include "shared_ptr.hpp"
#include <iostream>
#include <string>
#include <cassert>
#include <cstdlib>
#include <new>

using std::cout;
using std::endl;

namespace {
bool fail_next_allocation = false;
}

// 只在测试程序中替换分配函数，以便稳定触发控制块分配失败。
void* operator new(std::size_t size) {
    if (fail_next_allocation) {
        fail_next_allocation = false;
        throw std::bad_alloc();
    }
    if (void* p = std::malloc(size ? size : 1)) {
        return p;
    }
    throw std::bad_alloc();
}

void operator delete(void* p) noexcept { std::free(p); }
#ifdef __cpp_sized_deallocation
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
#endif

template <typename Operation>
void expect_allocation_failure(Operation operation) {
    fail_next_allocation = true;
    try {
        operation();
    } catch (const std::bad_alloc&) {
        assert(!fail_next_allocation);
        return;
    }
    std::abort();
}

// 用于观察构造/析构次数的对象
struct Widget {
    static int alive;
    std::string name;
    explicit Widget(std::string n) : name(std::move(n)) { ++alive; }
    ~Widget() { --alive; }
};
int Widget::alive = 0;

// 自定义删除器（用一个标志位确认它真的被调用）
struct DtorFlag {
    static bool called;
};
bool DtorFlag::called = false;

void custom_delete(Widget* p) {
    DtorFlag::called = true;
    delete p;
}

struct Node {
    study::shared_ptr<Node> next;
};

struct Base {};

struct Derived : Base {
    static int destroyed;
    ~Derived() { ++destroyed; }
};
int Derived::destroyed = 0;

int main() {
    using study::shared_ptr;
    using study::make_shared;

    // 1. 基本用法 + use_count + 拷贝
    {
        auto sp1 = make_shared<Widget>("a");
        assert(sp1.use_count() == 1);
        auto sp2 = sp1;                       // 拷贝
        assert(sp1.use_count() == 2);
        assert(sp2.use_count() == 2);
        assert(sp1.get() == sp2.get());       // 指向同一对象
        assert(sp1 == sp2);
        assert(Widget::alive == 1);
    }
    assert(Widget::alive == 0);               // 出作用域后正确析构
    cout << "1. 拷贝 / 引用计数 / 析构         OK" << endl;

    // 2. 移动语义
    {
        auto sp1 = make_shared<Widget>("b");
        shared_ptr<Widget> sp2 = std::move(sp1);   // 移动构造
        assert(!sp1);                              // sp1 被掏空
        assert(sp2.use_count() == 1);
        assert(Widget::alive == 1);
    }
    assert(Widget::alive == 0);
    cout << "2. 移动构造                       OK" << endl;

    // 3. reset / operator-> / operator*
    {
        auto sp = make_shared<Widget>("c");
        assert(sp->name == "c");
        assert((*sp).name == "c");
        sp.reset();                            // 释放
        assert(!sp);
        assert(Widget::alive == 0);
        sp.reset(new Widget("d"));             // 接管新对象
        assert(Widget::alive == 1);
    }
    assert(Widget::alive == 0);
    cout << "3. reset / -> / *                 OK" << endl;

    // 4. 自定义删除器
    {
        {
            shared_ptr<Widget> sp(new Widget("e"), custom_delete);
            assert(sp.use_count() == 1);
        }
        assert(DtorFlag::called);              // 自定义删除器确实被调用
        DtorFlag::called = false;
        assert(Widget::alive == 0);
    }
    cout << "4. 自定义删除器                    OK" << endl;

    // 5. 空指针 / 与 nullptr 比较
    {
        shared_ptr<Widget> sp;
        assert(!sp);
        assert(sp == nullptr);
        assert(nullptr == sp);
        assert(sp.use_count() == 0);
        assert(sp.get() == nullptr);
    }
    cout << "5. 空指针 / nullptr 比较            OK" << endl;

    // 6. swap
    {
        auto sp1 = make_shared<Widget>("x");
        auto sp2 = make_shared<Widget>("y");
        sp1.swap(sp2);
        assert(sp1->name == "y" && sp2->name == "x");
    }
    assert(Widget::alive == 0);
    cout << "6. swap                            OK" << endl;

    // 7. 自赋值安全性
    {
        auto sp = make_shared<Widget>("z");
        auto& ref = sp;
        sp = ref;                              // 自赋值不应崩溃
        assert(sp.use_count() == 1);
        sp = std::move(ref);                   // 自移动也不应崩溃
        assert(sp.use_count() == 1);
    }
    assert(Widget::alive == 0);
    cout << "7. 自赋值 / 自移动安全性            OK" << endl;

    // 8. 赋值右侧是当前对象的成员。替换 p 会销毁该成员。
    {
        auto p = make_shared<Node>();
        p->next = make_shared<Node>();
        Node* next = p->next.get();
        p = p->next;
        assert(p.get() == next && p.use_count() == 1);

        p->next = make_shared<Node>();
        next = p->next.get();
        p = std::move(p->next);
        assert(p.get() == next && p.use_count() == 1);
    }
    cout << "8. 从所持对象的成员拷贝 / 移动赋值    OK" << endl;

    // 9. 分配控制块失败时清理新对象，保留 reset 之前的所有权。
    {
        Widget* raw = new Widget("constructor");
        expect_allocation_failure([&] { shared_ptr<Widget> p(raw); });
        assert(Widget::alive == 0);

        raw = new Widget("custom deleter");
        expect_allocation_failure([&] { shared_ptr<Widget> p(raw, custom_delete); });
        assert(Widget::alive == 0 && DtorFlag::called);
        DtorFlag::called = false;

        auto p = make_shared<Widget>("original");
        Widget* original = p.get();
        Widget* replacement = new Widget("replacement");
        expect_allocation_failure([&] { p.reset(replacement); });
        assert(p.get() == original && p.use_count() == 1);
        assert(p->name == "original" && Widget::alive == 1);
    }
    assert(Widget::alive == 0);
    cout << "9. 控制块分配失败时的清理与回滚       OK" << endl;

    // 10. 访问类型是 Base，但销毁时使用接管时的 Derived*。
    {
        shared_ptr<Base> p(new Derived);
        p.reset(new Derived);
        assert(Derived::destroyed == 1);
    }
    assert(Derived::destroyed == 2);
    cout << "10. 无虚析构基类持有派生对象          OK" << endl;

    // 11. 空的访问地址仍可以对应一个控制块。
    {
        shared_ptr<Widget> empty(nullptr);
        assert(!empty && empty.use_count() == 0);

        Widget* raw_null = nullptr;
        shared_ptr<Widget> p(raw_null);
        assert(!p && p.use_count() == 1);
        p.reset(nullptr);
        assert(!p && p.use_count() == 0);
        p.reset(raw_null);
        assert(!p && p.use_count() == 1);

        shared_ptr<Widget> with_deleter(nullptr, custom_delete);
        assert(!with_deleter && with_deleter.use_count() == 1);
        with_deleter.reset(raw_null);
        assert(DtorFlag::called && !with_deleter && with_deleter.use_count() == 1);
    }
    cout << "11. 空指针的所有权与删除器           OK" << endl;

    cout << "\n全部测试通过！" << endl;
    return 0;
}
