#include <vector>
using namespace std;

int xx[] = {0, 0, -1, 1};
int yy[] = {-1, 1, 0, 0};
class Solution {
  public:
    int getNumberSum(int num) {
        int sum = 0;
        while (num > 0) {
            sum += num % 10;
            num /= 10;
        }
        return sum;
    }
    int movingCount(int threshold, int rows, int cols) {
        if(threshold == 0)
            return 1;

        vector<vector<bool>> visited(rows, vector<bool>(cols, false));
        int maxCount = 1;
        visited[0][0] = true;
        dfs(threshold, rows, cols, 0, 0, visited, maxCount);
        return maxCount;
    }

    void dfs(int threshold, int rows, int cols, int x, int y,
             vector<vector<bool>>& visited, int& count) {
        for (int i = 0; i < 4; i++) {
            int dx = x + xx[i];
            int dy = y + yy[i];
            if (dx >= 0 && dx < cols && dy >= 0 && dy < rows && !visited[dy][dx] &&
                    getNumberSum(dx) + getNumberSum(dy) <= threshold) {
                visited[dy][dx] = true;
                count++;
                dfs(threshold, rows, cols, dx, dy, visited, count);
            }
        }
    }
};