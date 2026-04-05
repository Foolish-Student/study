// https://leetcode.cn/problems/robot-return-to-origin/description/?envType=daily-question&envId=2026-04-05

#include <string>
using namespace std;

class Solution {
public:
    bool judgeCircle(string moves) {
        int leftright = 0;
        int topbottom = 0;
        int strlen = moves.length();
        for (int i = 0; i < strlen; ++i) {
            char c = moves[i];
            if (c == 'R') {
                leftright += 1;
            } else if (c == 'L') {
                leftright -= 1;
            } else if (c == 'U') {
                topbottom += 1;
            } else if (c == 'D') {
                topbottom -= 1;
            }
        }

        return leftright == 0 && topbottom == 0;
    }
};