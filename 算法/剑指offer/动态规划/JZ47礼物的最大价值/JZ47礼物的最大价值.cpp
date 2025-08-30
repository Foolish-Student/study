#include <vector>
#include <algorithm>
using namespace std;


class Solution {
    public:
      /**
       * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
       *
       *
       * @param grid int整型vector<vector<>>
       * @return int整型
       */
      int maxValue(vector<vector<int> >& grid) {
          // write code here
          int i = 0, j = 0;
          int m = grid.size();
          int n = grid[0].size();
          if (m == 1 && n == 1)
              return grid[0][0];
  
          for (int i = 0; i < m; i++) {
              for (int j = 0; j < n; j++) {
                  if (i == 0) {
                      grid[i][j] += grid[i][j - 1];
                  } else if (j == 0) {
                      grid[i][j] += grid[i - 1][j];
                  } else {
                      grid[i][j] += std::max(grid[i - 1][j], grid[i][j - 1]);
                  }
  
              }
          }
  
          return grid[m - 1][n - 1];
      }
  };