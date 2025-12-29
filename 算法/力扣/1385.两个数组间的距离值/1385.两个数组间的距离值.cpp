#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    int findTheDistanceValue(vector<int>& arr1, vector<int>& arr2, int d) {
        sort(arr1.begin(), arr1.end());
        sort(arr2.begin(), arr2.end());

        int ans = 0;
        for (int i = 0; i < arr1.size(); i++) {
            int val = arr1[i];
            auto it = lower_bound(arr2.begin(), arr2.end(), val - d);

            if (it == arr2.end() || *it > val + d)
                ans++;
        }

        return ans;
    }
};