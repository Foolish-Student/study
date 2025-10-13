#include <algorithm>
#include <string>
#include <vector>
using namespace std;

class Solution {
public:
    vector<string> removeAnagrams(vector<string>& words) {
        vector<string> sorted_words = words;
        int n = words.size();
        for (int i = 0; i < n; i++) {
            sort(sorted_words[i].begin(), sorted_words[i].end());
        }

        vector<string> res;
        int i = 1;
        res.push_back(words[0]);
        while (i < n) {
            if (sorted_words[i] != sorted_words[i - 1]) {
                res.push_back(words[i]);
            }
            i++;
        }
        return res;
    }
};