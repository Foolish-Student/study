// https://leetcode.cn/problems/sum-of-distances/?envType=daily-question&envId=2026-04-23


#include <map>
#include <vector>
using namespace std;

class Solution {
public:
    vector<long long> distance(vector<int>& nums) {
        map<int, vector<int>> mp;
        for (int i = 0; i < nums.size(); i++) {
            mp[nums[i]].push_back(i);
        }
        vector<long long> ans(nums.size(), 0);

        for (auto it : mp) {
            vector<int> indices = it.second;
            long long perSum = 0;
            long long sum = 0;

            for (int i = 0; i < indices.size(); i++) {
                sum += indices[i];
            }

            for (int i = 0; i < indices.size(); i++) {
                long long left = (long long)indices[i] * i - perSum;
                long long right = sum - perSum - indices[i] - (long long)indices[i] * (indices.size() - i - 1);
                ans[indices[i]] = left + right;
                perSum += indices[i];
            }
        }
        return ans;
    }
};