#include <string>
#include <vector>
using namespace std;

class Solution {
public:
    string findLexSmallestString(string s, int a, int b) {
        int n = s.size();
        vector<int> vis(n);          // 记录“从哪个起点 i 切子串”已经看过
        string res = s;              // 当前已知的最小答案（先设为原串）
        
        s = s + s;                   // 拼接成 2n，方便从任意 i 取长为 n 的轮转结果

        // 依次尝试起点 i = 0, b, 2b, 3b, ... (mod n)
        // 一旦遇到已经访问过的 i，说明开始循环了，停止
        for (int i = 0; vis[i] == 0; i = (i + b) % n) {
            vis[i] = 1;

            // j：对“奇数下标位”加 a 的次数（0..9）
            for (int j = 0; j < 10; j++) {
                // 如果 b 是偶数，偶数位永远没机会变成奇位 → 偶数位不能加，只能 k=0
                // 如果 b 是奇数，偶数位也能轮到奇位 → 也能加，k=0..9
                int k_limit = (b % 2 == 0 ? 0 : 9);

                for (int k = 0; k <= k_limit; k++) {
                    // 每次都先重新取一次“右移得到的串 t”
                    string t = s.substr(i, n);

                    // 先改“奇数下标”的位：1,3,5,...
                    for (int p = 1; p < n; p += 2) {
                        int digit = t[p] - '0';                 // 转成数字
                        digit = (digit + j * a) % 10;           // 加 j 次 a，取模 10
                        t[p] = char('0' + digit);               // 还原成字符
                    }
                    // 再改“偶数下标”的位：0,2,4,...
                    for (int p = 0; p < n; p += 2) {
                        int digit = t[p] - '0';
                        digit = (digit + k * a) % 10;           // 加 k 次 a，取模 10
                        t[p] = char('0' + digit);
                    }

                    // 维护最小值
                    if (t < res) res = t;
                }
            }
        }
        return res;
    }
};
    