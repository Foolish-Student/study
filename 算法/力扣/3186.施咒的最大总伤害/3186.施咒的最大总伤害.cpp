#include <unordered_map>
#include <vector>
using namespace std;

class Solution {
public:
    long long maximumTotalDamage(vector<int>& power) {
        unordered_map<int, int> m;
        int n = power.size();
        for (int i = 0; i < n; i++) {
            m[power[i]]++;
        }
        vector<int> v;
        for (auto& p : m) {
            v.push_back(p.first);
        }
        sort(v.begin(), v.end());
        vector<long long> dp(v.size() + 1, 0);
        int vSize = v.size();
        for (int i = 0, j = 0; i < vSize; i++) {
            int key = v[i];
            int value = m[key];
            while (j < vSize && v[j] < key - 2) {
                j++;
            }
            dp[i + 1] = max(dp[i], dp[j] + 1LL * key * value);
        }
        return dp[vSize];
    }
};