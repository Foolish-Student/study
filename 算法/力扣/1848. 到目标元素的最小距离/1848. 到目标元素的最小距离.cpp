// https://leetcode.cn/problems/minimum-distance-to-the-target-element/description/?envType=daily-question&envId=2026-04-13

#include <vector>
#include <climits>
using namespace std;

class Solution {
public:
    int getMinDistance(vector<int>& nums, int target, int start) {
        int numslen = nums.size();
        int minNum = INT_MAX;
        for(int i = 0; i < numslen; ++i){
            if(nums[i] == target){
                minNum = min(minNum, abs(i - start));
            }
        }

        return minNum;
    }
};