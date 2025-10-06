#include <vector>
#include <queue>
#include <tuple>
using namespace std;

class Solution {
public:
    int swimInWater(vector<vector<int>>& grid) {

        int m = grid.size();
        priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> pq; // 优先队列
        pq.push({grid[0][0], 0, 0});

        vector<vector<bool>> visit(m, vector<bool>(m, false)); // 访问数组
        visit[0][0] = true;

        vector<vector<int>> distTo(m, vector<int>(m, INT_MAX)); // 距离数组
        distTo[0][0] = grid[0][0];

        int xy[] = {-1, 0, 1, 0, -1};

        while(!pq.empty()) {
            tuple<int, int, int> t = pq.top();
            pq.pop();
            int g = get<0>(t);
            int x = get<1>(t);
            int y = get<2>(t);
            if(x == m - 1 && y == m - 1) {
                return g;
            }

            for(int i = 0; i < 4; i++) {
                int dx = x + xy[i];
                int dy = y + xy[i + 1];

                if(dx < 0 || dy < 0 || dx >= m || dy >= m || visit[dx][dy]) {
                    continue;
                }

                int dHeight = grid[dx][dy];
                if(max(distTo[x][y], dHeight) < distTo[dx][dy]) { 
                    // max(当前点的最大高度, 下一个点的高度） < 下一个点的最大高度； 下一个点没有计算过，那么应该是INT_MAX，如果计算过，就说明不是最短路径
                    distTo[dx][dy] = max(distTo[x][y], dHeight);
                    pq.push({distTo[dx][dy], dx, dy});
                    visit[dx][dy] = true;
                }
            }
        }

        return -1;
    }
};