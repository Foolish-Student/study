// ============================================================================
//  shared_ptr.hpp —— 一个「教学版」的 std::shared_ptr 实现
//
//  目标：
//    1. 行为和 std::shared_ptr 保持一致（引用计数、拷贝/移动、删除器、线程安全）。
//    2. 但结构尽量直白，去掉 STL 里那些难懂的虚函数继承链和模板技巧。
//
//  核心思想：用「控制块」(control block) 单独管理引用计数，
//           多个 shared_ptr 共享同一个控制块，最后一个持有者负责销毁资源。
//
//  和真实 std::shared_ptr 相比，这里做了哪些简化（不影响理解本质）：
//    - 删除器用普通函数指针表达，而不是任意可调用对象。
//    - 暂时不实现 weak_ptr / enable_shared_from_this（那是另一个话题）。
//    - 线程安全只用 std::atomic 保证计数，不实现 STL 的锁优化。
// ============================================================================

#ifndef STUDY_SHARED_PTR_HPP
#define STUDY_SHARED_PTR_HPP

#include <atomic>   // std::atomic 原子计数，保证多线程安全
#include <utility>  // std::move、std::forward
#include <cstddef>  // std::nullptr_t
#include <cassert>  // 辅助调试

namespace study {

// ----------------------------------------------------------------------------
// 一、控制块：负责「记录有多少个 shared_ptr 指向同一个对象」。
//
//      控制块是引用计数的载体。每当 new 一个 shared_ptr，就配套 new 一个
//      控制块，两者「一对一」绑定。所有拷贝出来的 shared_ptr 都共享这同一个
//      控制块，通过里面的 ref_count 知道还剩下多少个自己在用。
// ----------------------------------------------------------------------------
template <typename T>
struct ControlBlock {
    std::atomic<long> ref_count;   // 当前有几个 shared_ptr 指向它
    void (*deleter)(T*);           // 释放资源时调用的删除函数

    explicit ControlBlock(void (*del)(T*)) : ref_count(1), deleter(del) {}
};

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
    explicit shared_ptr(T* p)
        : ptr_(p), block_(p ? new ControlBlock<T>(DefaultDeleter<T>) : nullptr) {}

    // ---- 3.4 从裸指针 + 自定义删除器构造 ----
    shared_ptr(T* p, void (*del)(T*))
        : ptr_(p), block_(p ? new ControlBlock<T>(del) : nullptr) {}

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

    // ---- 3.7 拷贝赋值：先给新目标 +1，再释放旧目标 ----
    shared_ptr& operator=(const shared_ptr& other) noexcept {
        if (this != &other) {
            release();                          // 先放弃当前持有的资源
            ptr_ = other.ptr_;
            block_ = other.block_;
            if (block_) {
                block_->ref_count.fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    // ---- 3.8 移动赋值 ----
    shared_ptr& operator=(shared_ptr&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            block_ = other.block_;
            other.ptr_ = nullptr;
            other.block_ = nullptr;
        }
        return *this;
    }

    // ---- 3.9 析构：引用计数 -1，减到 0 就真正销毁 ----
    ~shared_ptr() {
        release();
    }

    // ---- 3.10 重置：放弃当前资源（可选地接管一个新指针）----
    void reset() noexcept {
        release();
    }

    void reset(T* p) {
        if (p == ptr_) {          // 重置为自己，啥也不做，避免误删
            return;
        }
        release();
        ptr_ = p;
        block_ = p ? new ControlBlock<T>(DefaultDeleter<T>) : nullptr;
    }

    // ---- 3.11 交换 ----
    void swap(shared_ptr& other) noexcept {
        T* tmp_p = ptr_;
        ControlBlock<T>* tmp_b = block_;
        ptr_ = other.ptr_;
        block_ = other.block_;
        other.ptr_ = tmp_p;
        other.block_ = tmp_b;
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
    // 引用计数减 1；减到 0 时，用控制块里记录的删除器销毁对象，再销毁控制块。
    void release() noexcept {
        if (block_ &&
            block_->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            block_->deleter(ptr_);   // 销毁被管理的对象
            delete block_;           // 销毁控制块
        }
        ptr_ = nullptr;
        block_ = nullptr;
    }

    T* ptr_;                 // 被管理的对象指针
    ControlBlock<T>* block_; // 指向共享的控制块
};

// ============================================================================
// 四、make_shared：一条龙地「分配对象 + 分配控制块 + 构造 shared_ptr」。
//
//      这里为了简单，用两次 new（对象一次、控制块一次）。真正的 STL 会把两者
//      合并成一次内存分配以减少开销，但那是性能优化，不是语义，先不管。
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
