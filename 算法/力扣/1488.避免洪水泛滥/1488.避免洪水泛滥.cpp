#include <set>
#include <unordered_map>
#include <vector>
using namespace std;

class Solution {
public:
    vector<int> avoidFlood(vector<int>& rains) {
        int n = rains.size();
        vector<int> res(n, 1);           // 记录结果
        set<int> lakeSet;                // 记录每个湖泊晴天的索引
        unordered_map<int, int> lakeMap; // 记录已经下雨的湖泊

        for (int i = 0; i < n; i++) {
            int lake = rains[i];

            if (lake == 0) {
                lakeSet.insert(i); // 晴天，记录索引
                continue;
                ;
            }

            auto it = lakeMap.find(lake);
            if (it != lakeMap.end()) {
                int value = it->second;
                auto sit = lakeSet.lower_bound(value);
                if (sit == lakeSet.end()) {
                    return {};
                }

                res[*sit] = lake; // 晴天时，放干湖泊
                lakeSet.erase(sit);
            }

            lakeMap[lake] = i;
            res[i] = -1; // 下雨天，记录-1
        }

        return res;
    }
};