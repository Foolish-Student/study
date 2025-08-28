## 第 19 条：理解相等（equality）和等价（equivalence）的区别

### 📌 背景

* 在 STL 中，不同容器和算法对“元素相同”的定义并不完全一致。
* **equality（相等）**：通常由 `operator==` 定义。
* **equivalence（等价）**：通常由 **排序准则（例如比较函数 `comp`）** 定义，不一定等同于 `==`。

---

### 📌 区别

1. **相等（equality）**

   * 用 `operator==` 来判断两个对象是否“相同”。
   * 例如：`std::find`、`std::remove`、`std::count` 都依赖 `==`。
   * 示例：

     ```cpp
     std::vector<int> v = {1, 2, 3};
     auto it = std::find(v.begin(), v.end(), 2); // 使用 ==
     ```

2. **等价（equivalence）**

   * 用比较函数（如 `<`）定义，常见于有序容器（`set`、`map`）。
   * 如果 `!(a < b) && !(b < a)`，则认为 `a` 和 `b` 等价。
   * 注意：等价不要求 `a == b` 为真。
   * 示例：

     ```cpp
     struct CaseInsensitive {
         bool operator()(const std::string& a, const std::string& b) const {
             return strcasecmp(a.c_str(), b.c_str()) < 0;
         }
     };
     std::set<std::string, CaseInsensitive> s;
     s.insert("Hello");
     s.insert("hello"); // 与 "Hello" 等价（按大小写忽略规则），不会插入
     ```

---

### 📌 常见问题

1. **混淆 equality 与 equivalence**

   * 在 `map` / `set` 中，判断“是否存在元素”依赖 **等价（equivalence）**，而不是 `==`。
   * 可能导致 `find` 与 `count` 的行为和直觉不符。

2. **违反一致性**

   * 如果 `operator==` 与比较函数（`<` 或自定义 `comp`）定义的等价关系不一致，会导致容器行为异常。
   * 例如：`set` 可能认为两个元素“等价”，但 `==` 却返回 `false`。

---

### 📌 最佳实践

1. **保持一致性**

   * 确保 `operator==` 和比较函数的等价关系保持一致。
   * 如果容器使用大小写不敏感的比较，那么最好 `operator==` 也实现为大小写不敏感。

2. **理解容器语义**

   * **无序容器**（`unordered_set`、`unordered_map`）：基于哈希和 `==`。
   * **有序容器**（`set`、`map`）：基于比较函数的等价关系。
   * **算法**（`find`、`count`）：默认使用 `==`，除非传入自定义谓词。

3. **避免意外情况**

   * 若 `==` 与比较准则不同，要在文档/注释中明确说明。
   * 或者避免混用可能造成混淆的函数（如同时用 `find` 和 `map::find`）。

---

### ✅ 总结

* **相等（equality）**：由 `operator==` 定义，STL 算法默认使用它。
* **等价（equivalence）**：由比较函数（通常 `<`）决定，STL 有序容器依赖它。
* **关键**：保持 `==` 与比较准则的一致性，否则会出现难以察觉的 bug。
