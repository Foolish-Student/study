#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;

class Solution {
public:
    int maxFrequency(vector<int> &nums, int k, int numOperations) {
        sort(nums.begin(), nums.end());

        unordered_map<int, int> numMap;
        int res = 0;
        int n = nums.size();
        int lastIndex = 0;
        for (int i = 0; i < n; i++) {
            if (nums[i] != nums[lastIndex]) {
                numMap[nums[lastIndex]] = i - lastIndex;
                res = max(res, i - lastIndex);
                lastIndex = i;
            }
        }
        numMap[nums[lastIndex]] = n - lastIndex;
        res = max(res, n - lastIndex);


        for (int i = nums.front(); i <= nums.back(); i++) {
            int lnum = i - k;
            int rnum = i + k;

            // 找到 [lnum, rnum] 范围内的所有元素
            auto l = lower_bound(nums.begin(), nums.end(), lnum); // 第一个 >= lnum 的位置
            auto r = upper_bound(nums.begin(), nums.end(), rnum); // 第一个 > rnum 的位置

            int w = (int)(r - l);
            int con = numMap.count(i) ? numMap[i] : 0;
            res = max(res, min(w , con + numOperations));
        }
        return res;
    }
};
    