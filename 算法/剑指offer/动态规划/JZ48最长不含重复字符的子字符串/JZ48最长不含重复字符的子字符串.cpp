#include <unordered_map>
#include <string>
using namespace std;

class Solution {
  public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     *
     * @param s string字符串
     * @return int整型
     */
    int lengthOfLongestSubstring(string s) {
        // write code here

        unordered_map<char, int> countChar;
        int maxCount = 0;
        int l = 0;
        for (int i = 0; i < s.length(); i++) {
            char curChar = s.at(i);
            countChar[curChar]++;

            while (countChar[curChar] > 1) {
                countChar[s.at(l++)]--;
            }
            maxCount = std::max(maxCount, i - l + 1);

        }
        return maxCount;
    }
};