#include <vector>
using namespace std;


class Solution {
public:
    int maxIncreasingSubarrays(vector<int>& nums) {
        int n = nums.size();
        int res = 0, cont = 1, percont = 0;
        for (int i = 1; i < n; i++) {
            if (nums[i] > nums[i - 1]) {
                cont++;
            } else {
                percont = cont;
                cont = 1;
            }

            res = max(res, min(percont, cont));
            res = max(res, cont / 2);
        }

        return res;
    }
};