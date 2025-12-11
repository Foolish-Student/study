#include <unordered_map>
#include <vector>
using namespace std;

class Solution {
public:
    long long maximumSubarraySum(vector<int>& nums, int k) {

        unordered_map<int, int> countNums;
        long long maxSum = 0;
        long long currentSum = 0;
        int n = nums.size();
        for (int i = 0; i < n; ++i) {
            currentSum += nums[i];
            countNums[nums[i]]++;

            int left = i - k + 1;
            if (left < 0) {
                continue;
            }

            if (countNums.size() == k) {
                maxSum = max(maxSum, currentSum);
            }
            currentSum -= nums[left];
            if (--countNums[nums[left]] == 0) {
                countNums.erase(nums[left]);
            }
        }
        return maxSum;
    }
};