#include <vector>
using namespace std;

class Solution {
public:
    int lower_bound(const vector<int>& nums, int target) {
        int left = 0;
        int right = nums.size() - 1; // [left, right]
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

    vector<int> searchRange(vector<int>& nums, int target) {
        int first = lower_bound(nums, target);
        int end = lower_bound(nums, target + 1) - 1;
        if (first == nums.size() || nums[first] != target) {
            return {-1, -1};
        }
        return {first, end};
    }
};