#include <vector>
using namespace std;

class Solution {
public:
    vector<int> getAverages(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> average = vector<int>(n, -1);
        int m = k * 2 + 1;
        long long sum = 0;
        for (int i = 0; i < n; i++) {
            if (i < m - 1) {
                sum += nums[i];
                continue;
            }

            sum += nums[i];
            average[i - k] = sum / m;
            sum -= nums[i - m + 1];
        }

        return average;
    }
};