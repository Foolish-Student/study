#include <queue>
#include <vector>
using namespace std;

class Solution {
public:
    static int xy[5];
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights) {
        int m = heights.size();
        int n = heights[0].size();
        vector<vector<bool>> pacific(m, vector<bool>(n, false));
        vector<vector<bool>> atlantic(m, vector<bool>(n, false));

        for (int i = 0; i < m; i++) {
            bfs(heights, i, 0, pacific);
            bfs(heights, i, n - 1, atlantic);
        }
        for (int i = 0; i < n; i++) {
            bfs(heights, 0, i, pacific);
            bfs(heights, m - 1, i, atlantic);
        }

        vector<vector<int>> res;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (pacific[i][j] && atlantic[i][j]) {
                    res.push_back({i, j});
                }
            }
        }

        return res;
    }

    void bfs(vector<vector<int>>& heights, int x, int y,
             vector<vector<bool>>& visit) {
        queue<pair<int, int>> q;
        q.push({x, y});
        visit[x][y] = true;

        while (!q.empty()) {
            pair<int, int> p = q.front();
            q.pop();
            for (int i = 0; i < 4; i++) {
                int dx = p.first + xy[i];
                int dy = p.second + xy[i + 1];
                if (dx < 0 || dy < 0 || dx >= heights.size() ||
                    dy >= heights[0].size() || visit[dx][dy] ||
                    heights[dx][dy] < heights[p.first][p.second])
                    continue;
                visit[dx][dy] = true;
                q.push({dx, dy});
            }
        }
    }
};

int Solution::xy[5] = {1, 0, -1, 0, 1};