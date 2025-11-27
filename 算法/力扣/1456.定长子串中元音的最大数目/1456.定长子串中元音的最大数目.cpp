#include <set>
#include <string>
using namespace std;

class Solution {
public:
    int maxVowels(string s, int k) {
        set<char> vowels = {'a', 'e', 'i', 'o', 'u'};

        int n = s.length();
        int l = 0;
        int maxCount = 0;
        int tempCount = 0;
        for (int r = k - 1; r < n; r++, l++) {
            if (l == 0) {
                for (int i = 0; i < k; i++) {
                    if (vowels.find(s[i]) != vowels.end()) {
                        maxCount++;
                    }
                    tempCount = maxCount;
                }
            } else {
                if (vowels.find(s[r]) != vowels.end()) {
                    tempCount++;
                }
                if (vowels.find(s[l - 1]) != vowels.end()) {
                    tempCount--;
                }
            }
            maxCount = max(maxCount, tempCount);
        }
        return maxCount;
    }
};