// https://leetcode.cn/problems/words-within-two-edits-of-dictionary/description/?envType=daily-question&envId=2026-04-22


#include <vector>
#include <string>
using namespace std;


class Solution {
public:
    vector<string> twoEditWords(vector<string>& queries,
                                vector<string>& dictionary) {
        int querLen = queries.size();
        int dicLen = dictionary.size();
        vector<string> ans;
        for (int i = 0; i < querLen; ++i) {
            for (int j = 0; j < dicLen; j++) {
                int con = 0;
                int qLen = queries[i].size();
                int dLen = dictionary[j].size();
                if (qLen != dLen)
                    break;

                for (int q = 0; q < qLen; ++q) {
                    if (queries[i][q] != dictionary[j][q]) {
                        con++;
                    }
                }
                if (con <= 2) {
                    ans.push_back(queries[i]);
                    break;
                }
            }
        }
        return ans;
    }
};