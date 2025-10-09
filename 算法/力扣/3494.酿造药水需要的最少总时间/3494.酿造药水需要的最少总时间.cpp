#include <algorithm>
#include <vector>
using namespace std;

class Solution {
public:
    long long minTime(vector<int>& skill, vector<int>& mana) {

        int n = skill.size();
        vector<long long> lastTime(n, 0);

        for (int j = 0; j < mana.size(); j++) {
            long long sum = 0; // 当前索引总时间

            for (int i = 0; i < n; i++) {
                sum = max(sum, lastTime[i]) +
                      skill[i] *
                          mana[j]; // max（当前可以开始时间，上一轮结束时间）+
                                   // 当前药水时间
            }

            lastTime[n - 1] = sum; // 最后一轮结束时间
            // 从后往前遍历，计算每一轮的结束时间
            for (int i = n - 2; i >= 0; i--) {
                lastTime[i] = lastTime[i + 1] - skill[i + 1] * mana[j];
            }
        }
        return lastTime[n - 1];
    }
};