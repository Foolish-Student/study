// https://leetcode.cn/problems/detect-cycles-in-2d-grid/?envType=daily-question&envId=2026-04-26

#include <vector>
using namespace std;

class Solution {
public:
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    bool containsCycle(vector<vector<char>>& grid) {
        int n = grid.size(), m = grid[0].size();
        vector<vector<bool>> visited(n, vector<bool>(m, false));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (!visited[i][j]) {
                    if (dfs(grid, visited, i, j, -1, -1)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool dfs(vector<vector<char>>& grid, vector<vector<bool>>& visited, int x,
             int y, int parentX, int parentY) {

        visited[x][y] = true;

        for (int i = 0; i < 4; ++i) {
            int newX = x + directions[i][0];
            int newY = y + directions[i][1];

            if (newX < 0 || newX >= grid.size() || newY < 0 || newY >= grid[0].size()) {
                continue;
            }

            if (grid[newX][newY] != grid[x][y]) {
                continue;
            }

            if (!visited[newX][newY]) {
                if (dfs(grid, visited, newX, newY, x, y)) {
                    return true;
                }
            } else {
                if (newX != parentX || newY != parentY) {
                    return true;
                }
            }
        }

        return false;
    }
};