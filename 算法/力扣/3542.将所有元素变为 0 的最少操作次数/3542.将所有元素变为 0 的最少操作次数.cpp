#include <vector>
using namespace std;

class Solution {
public:
    int minOperations(vector<int>& nums) {
        vector<int> s;
        int res = 0;
        for (int num : nums) {
            while (!s.empty() && s.back() > num) {
                res++;
                s.pop_back();
            }

            if ((s.empty() || s.back() < num) && num != 0) {
                s.push_back(num);
            }
        }
        return res + (int)s.size();
    }
};