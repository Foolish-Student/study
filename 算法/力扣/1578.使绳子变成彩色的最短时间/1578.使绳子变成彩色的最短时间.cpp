#include <string>
#include <vector>
using namespace std;

class Solution {
public:
    int minCost(string s, vector<int>& t) {
        int n = s.size();
        int ans = 0;
        int sum = t[0], mx = t[0];
        for (int i = 1; i < n; ++i) {
            if (s[i] == s[i - 1]) {      // 同色段继续
                sum += t[i];
                mx = max(mx, t[i]);
            } else {                      // 段结束，结算
                ans += sum - mx;
                sum = t[i];
                mx  = t[i];
            }
        }
        ans += sum - mx;                  // 最后一段
        return ans;
    }
};
    