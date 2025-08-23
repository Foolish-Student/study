## 第 17 条：使用 “swap 技巧” 除去多余的容量

### 📌 背景

* `vector` / `string` 的底层存储是动态数组。
* **问题**：当删除大量元素后，`size` 变小，但 `capacity`（容量）不会缩小，多余内存不会自动释放，可能导致内存浪费。
* 标准库的 `resize()` 只能改变元素个数，**不影响容量**。
* C++11 引入了 `shrink_to_fit()`，但它是 **non-binding（非强制性的）**，实现可能选择忽略。

---

### 📌 swap 技巧的原理

* 拷贝构造一个临时容器（容量通常等于 `size()`）。
* 用 `swap` 和原容器交换内容。
* 临时对象析构时，释放原来那块“臃肿”的大容量内存。

---

### 📌 使用方法

#### 1. 对 `vector`

```cpp
std::vector<int>(v).swap(v);
```

#### 2. 对 `string`

```cpp
std::string(s).swap(s);
```

---

### 📌 效果对比示例

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v;
    for (int i = 0; i < 1000; i++) v.push_back(i);
    for (int i = 0; i < 900; i++) v.pop_back();

    cout << "before: size=" << v.size() 
         << " capacity=" << v.capacity() << endl;

    std::vector<int>(v).swap(v);

    cout << "after:  size=" << v.size() 
         << " capacity=" << v.capacity() << endl;
}
```

输出可能是：

```
before: size=100 capacity=1024
after:  size=100 capacity=100
```

---

### 📌 注意事项

1. **复杂度**：O(n)，因为需要把元素拷贝/移动到新容器。
2. **迭代器/指针失效**：swap 之后，原来指向容器元素的迭代器和指针都会失效。
3. **现代替代方案**：

   * `shrink_to_fit()`（C++11 起），更直观，但可能不生效。
   * `swap 技巧` 则 100% 有效。

---

### ✅ 总结

* **问题**：容器删除元素后，容量不会自动缩小，可能浪费内存。
* **解决**：使用 `swap 技巧`，让容器的容量收缩到与 size 接近。
* **方法**：

  * `std::vector<T>(v).swap(v);`
  * `std::string(s).swap(s);`
* **收益**：释放多余内存，保证空间利用率。
* **提醒**：开销 O(n)，迭代器会失效。
