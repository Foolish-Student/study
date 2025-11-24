#include <vector>
using namespace std;

class Solution {
public:
    vector<bool> prefixesDivBy5(vector<int>& nums) {
        int n = nums.size();
        vector<bool> res(n);
        int curr = 0;
        for (int i = 0; i < n; i++) {
            curr = ((curr << 1) + nums[i]) % 5;
            res[i] = !curr;
        }
        return res;
    }
};