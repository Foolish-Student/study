#include <vector>
using namespace std;
const int MOD = 1e9 + 7;

class Solution {
public:
    int numberOfPaths(vector<vector<int>>& grid, int k) {
        int m = grid.size();
        int n = grid[0].size();
        vector<vector<vector<int>>> dp(
            m, vector<vector<int>>(n, vector<int>(k, 0)));

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 && j == 0) {
                    int r = grid[i][j] % k;
                    dp[i][j][r] = 1;
                    continue;
                }

                int valueMod = grid[i][j] % k;
                for (int r = 0; r < k; r++) {
                    int leftMod = 0;
                    int topMod = 0;

                    int prevMod = (r - valueMod + k) % k;
                    if (i > 0) {
                        topMod = dp[i - 1][j][prevMod];
                    }
                    if (j > 0) {
                        leftMod = dp[i][j - 1][prevMod];
                    }

                    dp[i][j][r] = (topMod + leftMod) % MOD;
                }
            }
        }

        return dp[m - 1][n - 1][0];
    }
};