#include <vector>

using namespace std;

class Solution {
public:
    vector<int> numSmallerByFrequency(vector<string>& queries,
                                      vector<string>& words) {
        vector<int> q(queries.size(), 0);
        for (int i = 0; i < queries.size(); i++) {
            sort(queries[i].begin(), queries[i].end());
            q[i] = upper_bound(queries[i].begin(), queries[i].end(),
                               queries[i][0]) -
                   queries[i].begin();
        }

        vector<int> w(words.size(), 0);
        for (int i = 0; i < words.size(); i++) {
            sort(words[i].begin(), words[i].end());
            w[i] = upper_bound(words[i].begin(), words[i].end(), words[i][0]) -
                   words[i].begin();
        }

        sort(w.begin(), w.end());
        for (int i = 0; i < q.size(); i++) {
            int index = w.end() - upper_bound(w.begin(), w.end(), q[i]);
            q[i] = index;
        }

        return q;
    }
};