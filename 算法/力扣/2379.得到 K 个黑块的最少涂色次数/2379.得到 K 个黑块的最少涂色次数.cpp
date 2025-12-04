#include <climits>
#include <string>
using namespace std;

class Solution {
public:
    int minimumRecolors(string blocks, int k) {
        int n = blocks.size();
        int res = INT_MAX;
        int black = 0;
        int white = 0;

        for (int i = 0; i < n; i++) {
            if (i < k) {
                if (blocks[i] == 'B') {
                    black++;
                } else if (blocks[i] == 'W') {
                    white++;
                }
                res = white;
                continue;
            }

            if (blocks[i] == 'B') {
                black++;
            } else if (blocks[i] == 'W') {
                white++;
            }

            if (blocks[i - k] == 'B') {
                black--;
            } else if (blocks[i - k] == 'W') {
                white--;
            }

            res = min(res, white);
        }
        return res;
    }
};