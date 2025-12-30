#include <vector>

using namespace std;

class Solution {
public:
    vector<int> answerQueries(vector<int>& nums, vector<int>& queries) {
        sort(nums.begin(), nums.end());
        partial_sum(nums.begin(), nums.end(), nums.begin());
        for (int& q : queries) {
            q = upper_bound(nums.begin(), nums.end(), q) - nums.begin();
        }

        return queries;
    }
};