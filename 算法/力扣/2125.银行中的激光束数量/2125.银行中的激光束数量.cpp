#include <string>
#include <vector>
using namespace std;

class Solution {
public:
    int numberOfBeams(vector<string>& bank) {
        int result = 0;
        int lastCount = 0;
        int n = bank.size();
        for (int i = 0; i < n; i++) {
            int con = count_if(bank[i].begin(), bank[i].end(),
                               [](char c) { return c == '1'; });
            if (i == 0) {
                lastCount = con;
            } else if (con != 0) {
                result += lastCount * con;
                lastCount = con;
            }
        }
        return result;
    }
};