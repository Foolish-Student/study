#include <climits>
#include <numeric>
#include <vector>
using namespace std;

class Solution {
public:
    int maxScore(vector<int>& cardPoints, int k) {
        int n = cardPoints.size();
        int sum = accumulate(cardPoints.begin(), cardPoints.end(), 0);

        int window = n - k;
        if (window == 0)
            return sum; // k == n，全拿

        int minsum = INT_MAX;
        int currsum = 0;

        for (int i = 0; i < n; ++i) {
            currsum += cardPoints[i];
            int left = i - window + 1;
            if (left < 0)
                continue;

            minsum = min(minsum, currsum);
            currsum -= cardPoints[left];
        }
        return sum - minsum;
    }
};
