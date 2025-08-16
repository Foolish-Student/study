#include <algorithm>
#include <vector>
using namespace std;

class Solution {
  public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     *
     * @param array int整型vector
     * @return int整型
     */
    int FindGreatestSumOfSubArray(vector<int>& array) {
        // write code here
        int dp[200005];
        int maxNum = -101;
        dp[0] = array[0];
        int arrayLen = array.size();
        for (int i = 0; i < arrayLen; i++) {
            dp[i] = std::max(array[i], dp[i - 1] + array[i]);
            if (dp[i] > maxNum)
                maxNum = dp[i];
        }

        return maxNum;
    }
};