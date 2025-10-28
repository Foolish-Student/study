#include <vector>
using namespace std;

class Solution {
public:
    int countValidSelections(vector<int>& nums) {
        int count = 0;
        int noZeros = 0;
        int n = nums.size();
        for (int i = 0; i < n; i++) {
            if (nums[i] > 0) {
                noZeros++;
            }
        }

        for (int i = 0; i < n; i++) {
            if (nums[i] == 0) {
                if (isValid(nums, noZeros, i, -1)) {
                    count++;
                }
                if (isValid(nums, noZeros, i, +1)) {
                    count++;
                }
            }
        }

        return count;
    }

    bool isValid(vector<int>& nums, int noZeros, int start, int dir) {
        int n = nums.size();
        vector<int> temp(nums);
        int curr = start;

        while (noZeros > 0 && curr >= 0 && curr < n) {
            if (temp[curr] > 0) {
                temp[curr]--;
                dir *= -1;
                if (temp[curr] == 0)
                    noZeros--;
            }

            curr += dir;
        }

        return noZeros == 0;
    }
};