#include <vector>
#include <string>
using namespace std;

// @lc code=start

int dx[] = {0, 0, -1, 1};
int dy[] = {1, -1, 0, 0};
class Solution {
  public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     *
     * @param matrix char字符型vector<vector<>>
     * @param word string字符串
     * @return bool布尔型
     */

    int m, n;
    bool hasPath(vector<vector<char> >& matrix, string word) {
        // write code here
        n = matrix.size();
        if ( n <= 0)
            return false;
        m = matrix[0].size();

        vector<vector<int>> vis(n, vector<int>(m, 0));

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {

                if (matrix[i][j] != word[0])
                    continue;
                bool result = dfs(i, j, 0, vis, matrix, word);
                if (result)
                    return true;
            }
        }
        return false;
    }

    bool dfs(int i, int j, int count, vector<vector<int>>& vis,
             vector<vector<char> >& matrix,
             string word) {

        if (count == word.length() - 1)
            return true;

        for (int index = 0; index < 4; index++) {
            int x = j + dx[index];
            int y = i + dy[index];
            if (x < m && x >= 0 && y < n && y >= 0 && !vis[y][x] &&
                    matrix[y][x] == word[count + 1]) {
                vis[y][x] = true;
                if (dfs(y, x, count + 1, vis, matrix, word))
                    return true;
                vis[y][x] = false;
            }
        }

        return false;
    }
};