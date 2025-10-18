#include <vector>
using namespace std;

class Solution {
public:
    int maxDistinctElements(vector<int>& nums, int k) {
        sort(nums.begin(), nums.end());
        int lastNum = INT_MIN;
        int con = 0;
        for (int& num : nums) {
            int n = min(max(lastNum + 1, num - k), num + k);
            if (n > lastNum) {
                lastNum = n;
                con++;
            }
        }
        return con;
    }
};