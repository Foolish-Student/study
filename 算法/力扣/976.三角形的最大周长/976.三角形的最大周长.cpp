#include <algorithm>
#include <vector>
using namespace std;

class Solution {
public:
    int largestPerimeter(vector<int>& nums) {
        int result = 0;
        int n = nums.size();
        sort(nums.begin(), nums.end());
        for (int i = n - 1; i >= 2; i--) {
            if (nums[i] < nums[i - 1] + nums[i - 2]) {
                result = nums[i] + nums[i - 1] + nums[i - 2];
                break;
            }
        }
        return result;
    }
};