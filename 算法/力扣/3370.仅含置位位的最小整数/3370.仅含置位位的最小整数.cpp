#include <cmath>
using namespace std;

class Solution {
public:
    int smallestNumber(int n) {
        int con = 0;
        while (n > 0) {
            n = n / 2;
            con++;
        }

        int res = 0;
        for (int i = 0; i < con; i++) {
            res += pow(2, i);
        }

        return res;
    }
};