// https://leetcode.cn/problems/xor-after-range-multiplication-queries-i/description/?envType=daily-question&envId=2026-04-08


#include <vector>
using namespace std;


class Solution {
public:
    int xorAfterQueries(vector<int>& nums, vector<vector<int>>& queries) {
        long long exp = 1e9 + 7;
        int queriesLen = queries.size();
        for (int i = 0; i < queriesLen; ++i) {
            int l = queries[i][0];
            int r = queries[i][1];
            int k = queries[i][2];
            int v = queries[i][3];
            int idx = l;
            while (idx <= r) {
                nums[idx] = (1ll * nums[idx] * v) % exp;
                idx += k;
            }
        }
        int numsLen = nums.size();
        int result = 0;
        for (int i = 0; i < numsLen; ++i) {
            result ^= nums[i];
        }
        return result;
    }
};