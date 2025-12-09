#include <map>
#include <vector>
using namespace std;

class Solution {
public:
    long long maxSum(vector<int>& nums, int m, int k) {
        int n = nums.size();
        map<int, int> countMap;
        long long maxSum = 0;
        long long currentSum = 0;
        for (int i = 0; i < n; ++i) {

            currentSum += nums[i];
            countMap[nums[i]]++;

            int left = i - k + 1;
            if (left < 0) {
                continue;
            }

            if (countMap.size() >= m) {
                maxSum = max(maxSum, currentSum);
            }

            currentSum -= nums[left];
            if (--countMap[nums[left]] == 0) {
                countMap.erase(nums[left]);
            }
        }

        return maxSum;
    }
};