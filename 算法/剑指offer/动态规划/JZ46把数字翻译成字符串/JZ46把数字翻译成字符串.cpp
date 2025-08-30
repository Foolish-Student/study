#include <vector>
#include <string>
using namespace std;

class Solution {
  public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     * 解码
     * @param nums string字符串 数字串
     * @return int整型
     */
    int solve(string nums) {
        // write code here
        if (nums == "0")
            return 0;
        else if (nums == "10" || nums == "20")
            return 1;

        int len = nums.length();
        for(int i = 1; i < len; i++)
        {
            if(nums.at(i) == '0' && (nums.at(i - 1) != '1' && nums.at(i - 1) != '2'))
            {
                return 0;
            }
        }

        vector<int>dp;
        dp.reserve(len + 1);
        dp[0] = 1;
        dp[1] = 1;

        for (int i = 2; i <= len; i++) {
            // 11-19, 21-26有2种处理方式
            if ((nums.at(i - 2) == '1' && nums.at(i - 1) > '0') ||
                    (nums.at(i - 2) == '2' && nums.at(i - 1) > '0' && nums.at(i - 1) < '7')) {
                dp[i] = dp[i - 1] + dp[i - 2];
            } else {
                dp[i] = dp[i - 1];
            }
        }

        return dp[len];
    }
};