#include <vector>
using namespace std;

class Solution {
    public:
        bool isBalance(int x) {
            vector<int> con(10, 0);
            while (x > 0) {
                int temp = x % 10;
                con[temp]++;
                x = x / 10;
            }
            for (int i = 0; i < 10; i++) {
                if (con[i] != 0 && con[i] != i) {
                    return false;
                }
            }
            return true;
        }
        int nextBeautifulNumber(int n) {
            for (int i = n + 1; i <= 1224444; i++) {
                if (isBalance(i))
                    return i;
            }
            return -1;
        }
    };