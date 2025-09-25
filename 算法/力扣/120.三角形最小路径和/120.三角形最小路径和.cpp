#include <vector>
using namespace std;

class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
        int n = triangle.size();
        int m = triangle[n - 1].size();

        for (int i = n - 2; i >= 0; i--) {
            for (int j = 0; j < triangle[i].size(); j++) {
                triangle[n - 1][j] =
                    triangle[i][j] +
                    min(triangle[n - 1][j], triangle[n - 1][j + 1]);
            }
        }

        return triangle[n - 1][0];
    }
};