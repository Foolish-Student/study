#include <vector>
using namespace std;

class Solution {
public:
    int countUnguarded(int m, int n, vector<vector<int>>& guards, vector<vector<int>>& walls) {
        vector<vector<int>> grid(m, vector<int>(n, 0));
        for(vector<int>& wall : walls) {
            grid[wall[0]][wall[1]] = -1;
        }
        for(vector<int>& guard : guards) {
            grid[guard[0]][guard[1]] = -1;
        }

        for(vector<int>& guard : guards) {
            int y = guard[0];
            int x = guard[1];
            // 向右走
            for(int i = x + 1; i < n; i++) {
                if(grid[y][i] != -1) {
                    grid[y][i] = 1;
                } else {
                    break;
                }
            }
            // 向左走
            for(int i = x - 1; i >= 0; i--) {
                if(grid[y][i] != -1) {
                    grid[y][i] = 1;
                } else {
                    break;
                }
            }
            // 向下走
            for(int i = y + 1; i < m; i++) {
                if(grid[i][x] != -1) {
                    grid[i][x] = 1;
                } else {
                    break;
                }
            }
            // 向上走
            for(int i = y - 1; i >= 0; i--) {
                if(grid[i][x] != -1) {
                    grid[i][x] = 1;
                } else {
                    break;
                }
            }
        }

        int count = 0;
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                if(grid[i][j] == 0) {
                    count++;
                }
            }
        }
        return count;
    }
};