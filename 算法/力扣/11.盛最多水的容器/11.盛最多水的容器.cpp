#include <vector>
#include <algorithm>
using namespace std;

class Solution {
public:
    int maxArea(vector<int>& height) {
        int n = height.size();
        if (n == 2)
            return min(height[0], height[1]);

        int resArea = 0;
        int curL = 0;
        int curR = n - 1;
        while (curL < curR) {
            resArea =
                max(resArea, (curR - curL) * min(height[curR], height[curL]));

            if (height[curL] < height[curR])
                curL++;
            else
                curR--;
        }

        return resArea;
    }
};