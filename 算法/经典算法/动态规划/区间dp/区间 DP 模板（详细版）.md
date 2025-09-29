# 区间 DP 模板（详细版）

```cpp
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

// 示例：区间 DP 模板，求最小值问题（可以改为求最大值）
int intervalDP(const vector<int>& values) {
    int n = values.size();

    // dp[i][j] 表示区间 [i, j] 的最优值（最小值或最大值）
    vector<vector<int>> dp(n, vector<int>(n, 0));

    // 区间长度 len >= 2，因为单个元素区间通常没有意义
    for (int len = 2; len < n; ++len) {
        // 左端点 i
        for (int i = 0; i + len < n; ++i) {
            int j = i + len;          // 右端点 j

            // 初始化 dp[i][j] 为极值
            dp[i][j] = INT_MAX;

            // 枚举分割点 k（分割区间 [i, j]）
            // k 在 i 和 j 之间，i < k < j
            for (int k = i + 1; k < j; ++k) {
                // 根据问题定义，计算分割后的结果
                int cost = dp[i][k] + dp[k][j] + values[i]*values[j]*values[k];

                // 更新 dp[i][j] 的最优值（最小值）
                dp[i][j] = min(dp[i][j], cost);
            }
        }
    }

    // 返回整个区间 [0, n-1] 的最优值
    return dp[0][n-1];
}
```

---

## ✅ 逐行讲解

1. **定义 DP 数组**

```cpp
vector<vector<int>> dp(n, vector<int>(n, 0));
```

* `dp[i][j]` 表示区间 `[i, j]` 的最优解。
* 对于最小值问题，通常初始化为 0 或 `INT_MAX`（根据需要）。

2. **循环顺序**

```cpp
for (int len = 2; len < n; ++len)  // 区间长度
    for (int i = 0; i + len < n; ++i)  // 区间左端点
```

* **区间长度优先**：保证每次计算 `dp[i][k]` 和 `dp[k][j]` 时，它们已经计算完毕。
* `len` 表示区间长度，`i` 是左端点，`j = i + len` 是右端点。

3. **初始化区间值**

```cpp
dp[i][j] = INT_MAX;
```

* 为了安全地使用 `min` 函数。
* 对于最大值问题，初始化为 `INT_MIN`。

4. **枚举分割点**

```cpp
for (int k = i + 1; k < j; ++k)
```

* `k` 是区间 `[i, j]` 内的分割点。
* 分割点可以理解为将大问题拆成两个子问题：`[i, k]` 和 `[k, j]`。

5. **更新 DP 值**

```cpp
dp[i][j] = min(dp[i][j], dp[i][k] + dp[k][j] + values[i]*values[j]*values[k]);
```

* 根据具体问题，自行替换 `values[i]*values[j]*values[k]` 这部分。
* 对最小化问题用 `min`，最大化问题用 `max`。

6. **返回结果**

```cpp
return dp[0][n-1];
```

* 整个区间 `[0, n-1]` 的最优值就是问题答案。

---

## 🔑 小技巧

1. **区间长度优先是关键**

   * 保证所有依赖的子区间已经计算完成。
2. **初始化极值**

   * 避免和默认值冲突导致错误结果。
3. **分割点枚举**

   * 区间 DP 的核心，通常 `i < k < j`。
4. **灵活修改**

   * 对最大值问题：`dp[i][j] = INT_MIN; dp[i][j] = max(dp[i][j], ...)`
   * 对求和、最小成本等问题：`dp[i][j] = min(dp[i][j], ...)`

---

## ⚡ 模板用途

* 多边形三角剖分的最低得分
* 合并石头最小成本
* 最大矩形面积相关问题
* 区间最优策略问题（博弈、硬币、合并卡牌）
