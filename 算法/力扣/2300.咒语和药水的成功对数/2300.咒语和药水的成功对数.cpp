#include <algorithm>
#include <vector>
using namespace std;

class Solution {
public:
    vector<int> successfulPairs(vector<int>& spells, vector<int>& potions,
                                long long success) {
        int n = spells.size();
        vector<int> res(n, 0);
        sort(potions.begin(), potions.end());
        for (int i = 0; i < n; i++) {
            int spell = spells[i];
            long long spet =
                (success + spell - 1) / spell; // 向上取整：(a + b - 1) / b
            auto it = lower_bound(potions.begin(), potions.end(), spet);
            res[i] = potions.end() - it;
        }
        return res;
    }
};