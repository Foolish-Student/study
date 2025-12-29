#include <vector>
using namespace std;

class Solution {
public:
    int lower_bound(const vector<int>& nums, int target) {
        int left = 0;
        int right = nums.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] >= target) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        return left;
    }

    int maximumCount(vector<int>& nums) {
        int n = nums.size();
        int neg = lower_bound(nums, 0);
        int pos = lower_bound(nums, 1);
        pos = n - pos;

        return max(pos, neg);
    }
};