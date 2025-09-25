#include <vector>
#include <algorithm>
using namespace std;

class Solution {
public:
  int triangleNumber(vector<int> &nums) {

    int n = nums.size();
    if(n < 3) return 0;
    sort(nums.begin(), nums.end());

    int s = 0, e = 1;
    int res = 0;
    while(s < n - 2 && e < n - 1){
        int k = nums[s] + nums[e];
        int index = lower_bound(nums.begin() + e + 1, nums.end(), k) - nums.begin();
        res += index - e - 1;
        e++;
        if(e == n - 1) {
            s++;
            e = s + 1;
        }
    }

    return res;
  }
};