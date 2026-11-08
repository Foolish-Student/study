// ============================================================================
//  shared_ptr.hpp —— 一个「教学版」的 std::shared_ptr 实现
//
//  目标：
//    1. 演示 std::shared_ptr 的基本所有权语义（引用计数、拷贝/移动、删除器）。
//    2. 让控制块的分配和对象的销毁过程尽量直白。
//
//  核心思想：用「控制块」(control block) 单独管理引用计数，
//           多个 shared_ptr 共享同一个控制块，最后一个持有者负责销毁资源。
//
//  和真实 std::shared_ptr 相比，这里做了哪些简化（不影响理解本质）：
//    - 删除器用普通函数指针表达，而不是任意可调用对象。
//    - 暂时不实现 weak_ptr / enable_shared_from_this（那是另一个话题）。
//    - 原子计数允许不同实例共享控制块；同一实例的并发读写需要同步。
// ============================================================================

#ifndef STUDY_SHARED_PTR_HPP
#define STUDY_SHARED_PTR_HPP

#include <atomic>   // std::atomic 原子引用计数
#include <utility>  // std::move、std::forward、std::swap
#include <cstddef>  // std::nullptr_t
#include <cassert>  // 辅助调试
#include <type_traits> // std::enable_if、std::is_convertible

namespace study {

// ----------------------------------------------------------------------------
// 一、控制块：记录所有者数量，以及最后一个所有者应如何释放资源。
//
//      接管裸指针时创建控制块。拷贝出的 shared_ptr 共享同一个控制块。
//      shared_ptr 的访问指针可以是 Base*，控制块仍能保留原始的 Derived*。
// ----------------------------------------------------------------------------
namespace detail {

struct ControlBlock {
    std::atomic<long> ref_count{1};
    virtual ~ControlBlock() = default;
};

template <typename Y>
struct PointerControlBlock final : ControlBlock {
    Y* owned_ptr;
    void (*deleter)(Y*);

    PointerControlBlock(Y* p, void (*del)(Y*)) noexcept
        : owned_ptr(p), deleter(del) {}

    ~PointerControlBlock() override { deleter(owned_ptr); }
};

template <typename Y>
ControlBlock* make_control_block(Y* p, void (*del)(Y*)) {
    try {
        return new PointerControlBlock<Y>(p, del);
    } catch (...) {
        del(p);
        throw;
    }
}

} // namespace detail

// ----------------------------------------------------------------------------
// 二、默认删除器：普通对象用 delete，数组用 delete[]。
// ----------------------------------------------------------------------------
template <typename T>
void DefaultDeleter(T* p) {
    delete p;
}

template <typename T>
void DefaultDeleterArray(T* p) {
    delete[] p;
}

// ----------------------------------------------------------------------------
// 三、shared_ptr 本体。
// ----------------------------------------------------------------------------
template <typename T>
class shared_ptr {
public:
    using element_type = T;

    // ---- 3.1 默认构造：空指针 ----
    shared_ptr() noexcept : ptr_(nullptr), block_(nullptr) {}

    // ---- 3.2 空指针构造 ----
    shared_ptr(std::nullptr_t) noexcept : ptr_(nullptr), block_(nullptr) {}

    // ---- 3.3 从裸指针构造（接管所有权）----
    //     一旦交给 shared_ptr，这个裸指针就归它管了，不要再手动 delete。
    template <typename Y,
              typename std::enable_if<std::is_convertible<Y*, T*>::value, int>::type = 0>
    explicit shared_ptr(Y* p)
        : ptr_(p), block_(detail::make_control_block(p, DefaultDeleter<Y>)) {}

    // ---- 3.4 从裸指针 + 自定义删除器构造 ----
    //     删除器须可调用且不抛异常，即使 p 为 nullptr 也会调用它。
    shared_ptr(T* p, void (*del)(T*))
        : ptr_(p), block_(detail::make_control_block(p, del)) {}

    // ---- 3.5 拷贝构造：引用计数 +1 ----
    shared_ptr(const shared_ptr& other) noexcept
        : ptr_(other.ptr_), block_(other.block_) {
        if (block_) {
            block_->ref_count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    // ---- 3.6 移动构造：直接「偷」走，计数不变 ----
    shared_ptr(shared_ptr&& other) noexcept
        : ptr_(other.ptr_), block_(other.block_) {
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    // ---- 3.7 拷贝赋值：先接住右侧，再由临时对象释放旧资源 ----
    //     右侧可能是旧对象的成员，例如 p = p->next。
    shared_ptr& operator=(const shared_ptr& other) noexcept {
        if (this != &other) {
            shared_ptr(other).swap(*this);
        }
        return *this;
    }

    // ---- 3.8 移动赋值 ----
    shared_ptr& operator=(shared_ptr&& other) noexcept {
        if (this != &other) {
            shared_ptr(std::move(other)).swap(*this);
        }
        return *this;
    }

    // ---- 3.9 析构：引用计数 -1，减到 0 就真正销毁 ----
    ~shared_ptr() {
        if (block_ &&
            block_->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete block_; // 控制块的析构函数会调用保存的删除器
        }
    }

    // ---- 3.10 重置：放弃当前资源（可选地接管一个新指针）----
    void reset() noexcept {
        shared_ptr().swap(*this);
    }

    void reset(std::nullptr_t) noexcept {
        reset();
    }

    template <typename Y,
              typename std::enable_if<std::is_convertible<Y*, T*>::value, int>::type = 0>
    void reset(Y* p) {
        if (p && p == ptr_) { // 保留教学版对 reset(get()) 的保护
            return;
        }
        shared_ptr(p).swap(*this); // 构造成功后才替换当前持有的资源
    }

    // ---- 3.11 交换 ----
    void swap(shared_ptr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    // ---- 3.12 访问器 ----
    T* get() const noexcept { return ptr_; }

    long use_count() const noexcept {
        return block_ ? block_->ref_count.load(std::memory_order_relaxed) : 0;
    }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T& operator*() const noexcept {
        assert(ptr_ != nullptr && "解引用空的 shared_ptr");
        return *ptr_;
    }

    T* operator->() const noexcept {
        assert(ptr_ != nullptr && "访问空的 shared_ptr");
        return ptr_;
    }

private:
    T* ptr_;                       // get() 返回的访问地址
    detail::ControlBlock* block_; // 共享的控制块
};

// ============================================================================
// 四、make_shared：一条龙地「分配对象 + 分配控制块 + 构造 shared_ptr」。
//
//      这里用两次 new（对象一次、控制块一次）。标准库通常合并成一次分配。
// ============================================================================
template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
    return shared_ptr<T>(new T(std::forward<Args>(args)...));
}

// ----------------------------------------------------------------------------
// 五、比较运算符（和 STL 一样，基于「裸指针」比较）。
// ----------------------------------------------------------------------------
template <typename T>
bool operator==(const shared_ptr<T>& a, const shared_ptr<T>& b) noexcept {
    return a.get() == b.get();
}
template <typename T>
bool operator!=(const shared_ptr<T>& a, const shared_ptr<T>& b) noexcept {
    return a.get() != b.get();
}
template <typename T>
bool operator<(const shared_ptr<T>& a, const shared_ptr<T>& b) noexcept {
    return a.get() < b.get();
}
template <typename T>
bool operator==(const shared_ptr<T>& a, std::nullptr_t) noexcept {
    return a.get() == nullptr;
}
template <typename T>
bool operator==(std::nullptr_t, const shared_ptr<T>& a) noexcept {
    return a.get() == nullptr;
}
template <typename T>
bool operator!=(const shared_ptr<T>& a, std::nullptr_t) noexcept {
    return a.get() != nullptr;
}
template <typename T>
bool operator!=(std::nullptr_t, const shared_ptr<T>& a) noexcept {
    return a.get() != nullptr;
}

// ----------------------------------------------------------------------------
// 六、辅助：自定义删除器示例（配合 3.4 的构造）。
// ----------------------------------------------------------------------------
template <typename T>
void ArrayDeleter(T* p) {
    delete[] p;
}

} // namespace study

#endif // STUDY_SHARED_PTR_HPP
