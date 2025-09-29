#include <vector>
using namespace std;

class Solution {
public:
    int minScoreTriangulation(vector<int>& values) { return useDFS(values); }

    int useDp(vector<int>& values) {
        int n = values.size();
        vector<vector<int>> dp(n, vector<int>(n, 0));
        for (int i = n - 3; i >= 0; i--) {          // 起点
            for (int len = 2; i + len < n; len++) { // 长度
                int j = i + len;
                for (int k = i + 1; k < j; k++) { // 分割点
                    if (dp[i][j] == 0)
                        dp[i][j] = values[i] * values[j] * values[k] +
                                   dp[i][k] + dp[k][j];
                    else
                        dp[i][j] =
                            min(dp[i][j], values[i] * values[j] * values[k] +
                                              dp[i][k] + dp[k][j]);
                }
            }
        }
        return dp[0][n - 1];
    }

    int useDFS(vector<int>& values) {
        int n = values.size();
        vector<vector<int>> visit(n, vector<int>(n, -1));

        auto dfs = [&](auto dfs, int i, int j) -> int {
            if (i + 1 == j)
                return 0;

            if (visit[i][j] != -1)
                return visit[i][j];

            int res = INT_MAX;
            for (int k = i + 1; k < j; k++) {
                res = min(res, values[i] * values[j] * values[k] +
                                   dfs(dfs, i, k) + dfs(dfs, k, j));
            }
            visit[i][j] = res;
            return res;
        };

        return dfs(dfs, 0, n - 1);
    }
};