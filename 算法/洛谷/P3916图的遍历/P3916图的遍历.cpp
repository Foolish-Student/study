#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

int N, M;
vector<int>vec[100010];
bool vis[100010];
int res[100010];
int dfs(int x) {
    vis[x] = 1;
    res[x] = x;
    for (int i = 0; i < vec[x].size(); i++) {
        int next = vec[x][i];
        if (!vis[next]) {
            dfs(next);
        }
        res[x] = max(res[x], res[next]);
    }
    return res[x];
}
int main()
{
    cin >> N >> M;
    while (M--)
    {
        int u, v;
        cin >> u >> v;
        vec[v].push_back(u);
    }

    for (int i = 1; i <= N; i++) {
        if (!vis[i])
            dfs(i);
    }

    for (int i = 1; i <= N; i++)
        cout << res[i] << " ";

    return 0;
}