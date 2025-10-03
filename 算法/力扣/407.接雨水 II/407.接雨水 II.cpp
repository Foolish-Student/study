#include <queue>
#include <vector>
using namespace std;

class Solution {
public:
    int trapRainWater(vector<vector<int> > &heightMap) {
        int m = heightMap.size();
        int n = heightMap[0].size();
        if (m <= 2 || n <= 2)
            return 0;

        int res = pq_bfs(heightMap, m, n);

        return res;
    }

    function<bool(const vector<int> &, const vector<int> &)> cmp =
            [](const vector<int> &a, const vector<int> &b) {
        return a[2] > b[2];
    };

    int pq_bfs(vector<vector<int>> &heightMap, int m, int n) {
        int xy[] = {-1, 0, 1, 0, -1};
        priority_queue<vector<int>, vector<vector<int> >,
            function<bool(const vector<int> &, const vector<int> &)> > pq(cmp);

        vector<vector<bool> > visit(m, vector<bool>(n, false));

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 || i == m - 1 || j == 0 || j == n - 1) {
                    pq.push({i, j, heightMap[i][j]});
                    visit[i][j] = true;
                }
            }
        }

        int res = 0;
        while (!pq.empty()) {
            vector<int> curr = pq.top();
            pq.pop();
            for (int i = 0; i < 4; i++) {
                int dx = curr[0] + xy[i];
                int dy = curr[1] + xy[i + 1];
                if (dx < 0 || dx >= m || dy < 0 || dy >=n || visit[dx][dy])
                    continue;

                int dHeight = heightMap[dx][dy];
                if (dHeight < curr[2]) // 能接雨水
                    res += curr[2] - dHeight;
                visit[dx][dy] = true;

                heightMap[dx][dy] = max(heightMap[curr[0]][curr[1]], dHeight);
                pq.push({dx, dy, heightMap[dx][dy]});
            }
        }

        return res;
    }


    int bfs(vector<vector<int> > &heightMap, int m, int n) {
        // 上下左右
        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};
        int maxHeight = -1;
        for (int i = 0; i < m; i++) {
            maxHeight = max(maxHeight, *max_element(heightMap[i].begin(), heightMap[i].end()));
        }
        vector<vector<int> > waterHeight(m, vector<int>(n, maxHeight));
        queue<pair<int, int> > q;
        // 处理边界
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (i > 0 && i < m - 1 && j > 0 && j < n - 1) {
                    continue;
                }
                waterHeight[i][j] = min(waterHeight[i][j], heightMap[i][j]); // 边界处的水位等于地形高度
                q.push({i, j});
            }
        }

        int xArray[] = {0, 0, -1, 1};
        int yArray[] = {-1, 1, 0, 0};
        while (!q.empty()) {
            pair<int, int> p = q.front();
            int x = p.first;
            int y = p.second;
            q.pop();
            for (int i = 0; i < 4; i++) {
                int dx = x + xArray[i];
                int dy = y + yArray[i];
                if (dx < 0 || dx >= m || dy < 0 || dy >= n) {
                    continue;
                }

                // 如果当前水位小于邻居水位且大于邻居地形高度，则更新邻居水位
                if (waterHeight[x][y] < waterHeight[dx][dy] && waterHeight[dx][dy] > heightMap[dx][dy]) {
                    waterHeight[dx][dy] = max(waterHeight[x][y], heightMap[dx][dy]);
                    q.push({dx, dy});
                }
            }
        }

        int ans = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                ans += waterHeight[i][j] - heightMap[i][j];
            }
        }


        return ans;
    }
};
