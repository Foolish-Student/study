#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;

class Solution {
    public:
      /**
       * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
       *
       *
       * @param prices int整型vector
       * @return int整型
       */
      int maxProfit(vector<int>& prices) {
          // write code here
  
          int n = prices.size();
          if (n < 2)
              return 0;
  
          int dp[n + 1][2];
          memset(dp, 0, (n + 1) * 2);
  
          dp[0][0] = 0; // 不持股
          dp[0][1] = -prices[0]; // 持股
          for (int i = 1; i < n; i++) {
              // 不持股：
              // 1. 昨天不持股，今天不动
              // 2. 昨天持股，今天卖出
              dp[i][0] = std::max(dp[i - 1][0], dp[i - 1][1] + prices[i]);
              // 持股
              // 1. 昨天不持股，今天买入
              // 2. 昨天持股，今天不动
              dp[i][1] = std::max(- prices[i], dp[i - 1][1]);
          }
  
          return dp[n - 1][0];
      }
  };