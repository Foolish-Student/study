#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;


class Solution {
public:
    int trap(vector<int>& height) {
        int n = height.size();
        int maxHeightLeft[n];
        int maxHeightRight[n];
        memset(maxHeightLeft, 0, sizeof(int) * n);
        memset(maxHeightRight, 0, sizeof(int) * n);

        for (int i = 1; i < n; i++) {
            maxHeightLeft[i] = max(maxHeightLeft[i - 1], height[i - 1]);
        }
        for (int i = n - 2; i >= 0; i--) {
            maxHeightRight[i] = max(maxHeightRight[i + 1], height[i + 1]);
        }

        int result = 0;
        for (int i = 1; i < n - 1; i++) {
            int h = min(maxHeightLeft[i], maxHeightRight[i]);
            if (h > height[i])
                result += h - height[i];
        }

        return result;
    }
};