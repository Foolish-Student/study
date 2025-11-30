#include <climits>
#include <vector>
using namespace std;

class Solution {
public:
    double findMaxAverage(vector<int>& nums, int k) {
        int n = nums.size();
        int ans = INT_MIN;
        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += nums[i];
            if (i < k - 1)
                continue;

            ans = max(ans, sum);
            sum -= nums[i - k + 1];
        }

        return ans * 1.0 / k;
    }
};