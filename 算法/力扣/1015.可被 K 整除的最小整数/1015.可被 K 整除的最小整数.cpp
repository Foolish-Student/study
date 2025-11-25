#include <set>
using namespace std;

class Solution {
public:
    int smallestRepunitDivByK(int k) {
        int res = 0;
        set<int> s;
        while(s.find(res) == s.end()) {
            s.insert(res);
            res = (res * 10 + 1) % k;
            if(res == 0) {
                return s.size();
            }
        }
        return -1; // 如果找不到，返回 -1
    }
};