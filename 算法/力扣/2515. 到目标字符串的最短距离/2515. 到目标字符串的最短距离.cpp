// https://leetcode.cn/problems/shortest-distance-to-target-string-in-a-circular-array/description/?envType=daily-question&envId=2026-04-15


#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
using namespace std;

class Solution {
public:
    int closestTarget(vector<string>& words, string target, int startIndex) {
        int n = words.size();
        int ans = n;
        for(int i = 0; i < n; ++i){
            if(words[i] == target){
                int idx = abs(i - startIndex);
                ans = min(ans, min(idx, n - idx));
            }
        }

        return ans == n ? -1 : ans;
    }
};