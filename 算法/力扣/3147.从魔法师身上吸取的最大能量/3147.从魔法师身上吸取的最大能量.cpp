#include <vector>
#include <algorithm>
#include <climits>
using namespace std;


class Solution {
public:
    int maximumEnergy(vector<int>& energy, int k) {
        int maxSun = INT_MIN;
        int n = energy.size();
        for (int i = n - k; i < n; i++) {
            int tempMax = 0;
            for (int j = i; j >= 0; j -= k) {
                tempMax += energy[j];
                maxSun = max(tempMax, maxSun);
            }
        }
        return maxSun;
    }
};