#include <vector>
using namespace std;


class Solution {
public:
    bool hasIncreasingSubarrays(vector<int>& nums, int k) {
        int n = nums.size();

        for (int i = 0; i <= n - k - k; i++) {
            if (intervalIncreasing(nums, i, i + k) &&
                intervalIncreasing(nums, i + k, i + k + k)) {
                return true;
            }
        }
        return false;
    }

    bool intervalIncreasing(vector<int>& nums, int start, int end) {
        for (int i = start + 1; i < end; i++) {
            if (nums[i] <= nums[i - 1]) {
                return false;
            }
        }
        return true;
    }
};