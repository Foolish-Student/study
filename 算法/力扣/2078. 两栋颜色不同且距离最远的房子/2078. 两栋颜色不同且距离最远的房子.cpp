// https://leetcode.cn/problems/two-furthest-houses-with-different-colors/description/?envType=daily-question&envId=2026-04-20

#include <vector>
#include <climits>
#include <cmath>
using namespace std;

class Solution {
public:
    int maxDistance(vector<int>& colors) {
        int colorlen = colors.size();
        int maxcon = INT_MIN;
        for (int i = 0; i < colorlen; ++i) {
            for (int j = colorlen - 1; j > i; --j) {
                if (colors[i] != colors[j])
                    maxcon = max(maxcon, abs(j - i));
            }
        }

        return maxcon;
    }
};