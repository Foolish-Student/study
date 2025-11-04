#include <map>
#include <vector>
using namespace std;

class Solution {
public:
    vector<int> findXSum(vector<int>& nums, int k, int x) {
        vector<int> res;
        int n = nums.size();
        for (int i = 0; i < n - k + 1; i++) {
            map<int, int, greater<int>> numMap;
            for (int j = i; j < i + k && j < n; j++) {
                numMap[nums[j]]++;
            }
            int num = 0;
            if (numMap.size() < x) {
                for_each(numMap.begin(), numMap.end(),
                         [&](auto& p) { num += p.first * p.second; });
                res.push_back(num);
                continue;
            }
            for (int j = 0; j < x; j++) {
                int temp = getMaxFrequency(numMap);
                num += numMap[temp] * temp;
                numMap.erase(temp);
            }
            res.push_back(num);
        }
        return res;
    }

    int getMaxFrequency(map<int, int, greater<int>>& numMap) {
        int maxFrequency = 0;
        int maxNum = 0;
        for (auto& [num, frequency] : numMap) {
            if (frequency > maxFrequency) {
                maxFrequency = frequency;
                maxNum = num;
            }
        }
        return maxNum;
    }
};