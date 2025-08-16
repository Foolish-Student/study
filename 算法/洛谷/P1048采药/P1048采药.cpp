#include <iostream>
#include <algorithm>

int t[1010];
int w[110];
int dp[1010][1010];

int n, m;
int main() {
    std::cin >> n >> m;

    for (int i = 1; i <= m; i++) {
        std::cin>> t[i] >> w[i];
    }

    for (int i = 1; i <= m; i++) { // 药品种类
        for (int j = 1; j <= n; j++) { // 采药的时间
            // 不拿，用之前的重量
            dp[i][j] = dp[i - 1][j];
            // 拿，不拿的价值，跟拿的价值做比较
            if (j - t[i] >= 0)
                dp[i][j] = std::max(dp[i][j], dp[i - 1][j - t[i]] + w[i]);
        }
    }

    std::cout << dp[m][n];
}