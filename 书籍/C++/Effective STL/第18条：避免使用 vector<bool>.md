## 第 18 条：避免使用 `vector<bool>`

### 📌 背景

* `std::vector<T>` 通常是一个 **真正的动态数组**，每个元素是一个 `T`，支持随机访问，返回对元素的 **引用（T&）**。
* 但 `std::vector<bool>` 是 **一个特化版本**，它的行为和普通 `vector<T>` 不一样。

---

### 📌 `vector<bool>` 的问题

1. **存储方式特殊**

   * 为了节省空间，`vector<bool>` 不是用 1 字节存一个布尔值，而是用 **1 bit 存一个 bool**。
   * 导致它不是一个“真正的 `vector<bool>`”，而更像一个 **位图 (bitset)**。

2. **引用问题**

   * 普通 `vector<T>` 的 `operator[]` 返回 `T&`。
   * 但 `vector<bool>` 由于压缩成 bit，只能返回一个 **代理对象 (proxy object)**，而不是 `bool&`。
   * 这让一些通用模板代码（依赖 `T&`）无法正确编译。

   示例：

   ```cpp
   std::vector<bool> v = {true, false};
   bool* p = &v[0]; // ❌ 编译失败：返回的不是 bool&
   ```

3. **性能与语义差异**

   * 因为用位操作模拟引用，读写操作比真正的数组慢。
   * 一些 STL 泛型算法在 `vector<bool>` 上可能表现异常。

4. **可移植性差**

   * `vector<bool>` 的实现细节依赖编译器，行为不可预测。

---

### 📌 替代方案

1. **用 `std::deque<bool>`**

   * 不做 bit 压缩，行为和普通容器一致。
   * 支持 `bool&` 引用，避免 `vector<bool>` 的怪异行为。

2. **用 `std::vector<char>` 或 `std::vector<uint8_t>`**

   * 如果主要目的是存储布尔数组，可以用 1 字节表示一个 bool。
   * 内存占用比 `vector<bool>` 大，但简单可靠。

3. **用 `std::bitset<N>`**

   * 如果大小在编译期已知，用 `bitset` 更高效。

4. **用 `boost::dynamic_bitset`**

   * 如果需要运行期可变大小的位图，Boost 提供了更完善的实现。

---

### ✅ 总结

* **问题**：`vector<bool>` 是个特殊化版本，不是标准 `vector` 的正常语义：

  * 压缩存储成位图；
  * 不能返回 `bool&`，只返回代理对象；
  * 性能差、语义怪异、可移植性差。
* **建议**：**避免使用 `vector<bool>`**。
* **替代方案**：

  * 要容器 → `deque<bool>` / `vector<char>` / `vector<uint8_t>`；
  * 要位图 → `bitset` / `boost::dynamic_bitset`。
