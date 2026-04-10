// https://leetcode.cn/problems/minimum-distance-between-three-equal-elements-i/description/?envType=daily-question&envId=2026-04-10


#include <vector>
#include <climits>
#include <cmath>
using namespace std;


class Solution {
public:
    int minimumDistance(vector<int>& nums) {

        int numlen = nums.size();
        if (numlen < 3)
            return -1;
        int mindistance = INT_MAX;
        for (int i = 0; i < numlen; ++i) {
            for (int j = i + 1; j < numlen; ++j) {
                if (nums[i] != nums[j])
                    continue;
                for (int k = j + 1; k < numlen; ++k) {
                    if (nums[j] == nums[k]) {
                        int distance = abs(i - j) + abs(j - k) + abs(k - i);
                        mindistance =
                            mindistance > distance ? distance : mindistance;
                    }
                }
            }
        }
        return mindistance == INT_MAX ? -1 : mindistance;
    }
};