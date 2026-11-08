// test_shared_ptr.cpp —— 验证教学版 shared_ptr 行为是否正确
#include "shared_ptr.hpp"
#include <iostream>
#include <string>
#include <cassert>

using std::cout;
using std::endl;

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
        sp = sp;                               // 自赋值不应崩溃
        assert(sp.use_count() == 1);
        auto& ref = sp;
        sp = std::move(ref);                   // 自移动也不应崩溃
        assert(sp.use_count() == 1);
    }
    assert(Widget::alive == 0);
    cout << "7. 自赋值 / 自移动安全性            OK" << endl;

    cout << "\n全部测试通过！" << endl;
    return 0;
}
