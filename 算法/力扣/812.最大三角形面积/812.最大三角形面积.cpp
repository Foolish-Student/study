#include <cmath>
#include <vector>

using namespace std;

double getArea(vector<vector<int>>& points, int i, int j, int k) {
    return 0.5 * abs(points[i][0] * points[j][1] + points[j][0] * points[k][1] +
                     points[k][0] * points[i][1] - points[i][0] * points[k][1] -
                     points[j][0] * points[i][1] - points[k][0] * points[j][1]);
}

class Solution {
public:
    double largestTriangleArea(vector<vector<int>>& points) {
        int n = points.size();
        double maxArea = LONG_LONG_MIN * 1.0;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                for (int k = j + 1; k < n; k++) {
                    maxArea = max(maxArea, getArea(points, i, j, k));
                }
            }
        }
        return maxArea;
    }
};