#include <string>
#include <set>
#include <vector>
using namespace std;

class Solution {
public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     * 
     * @param str string字符串 
     * @return string字符串vector
     */

    std::set<string> svis;
    vector<string> Permutation(string str) {
        // write code here
        string res;
        bool vis[str.length()];
        memset(vis, 0, sizeof(bool) * str.length());
        dfs(str, 0, res, vis);
        return vector<string>(svis.begin(), svis.end());
    }

    void dfs(const string& str, int n, string res, bool vis[]){
        if(n == str.length()){
            svis.insert(res);
            int c = svis.size();
            return;
        }

        for(int i = 0; i < str.length(); i++){
            if(vis[i] != 0)
                continue;

            vis[i] = 1;
            dfs(str, n + 1, res + str.at(i), vis);
            vis[i]=0;
        }
    }
};