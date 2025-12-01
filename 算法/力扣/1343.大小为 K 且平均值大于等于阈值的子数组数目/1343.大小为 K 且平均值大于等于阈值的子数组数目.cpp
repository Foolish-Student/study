#include <vector>
using namespace std;

class Solution {
public:
    int numOfSubarrays(vector<int>& arr, int k, int threshold) {
        int n = arr.size();
        int ans = 0;
        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += arr[i];
            if (i < k - 1)
                continue;

            int avg = sum / k;
            if (avg >= threshold)
                ans++;

            sum -= arr[i - k + 1];
        }
        return ans;
    }
};