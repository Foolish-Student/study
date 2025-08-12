#include <iostream>
#include <vector>
#include <algorithm>

const int INF = 1e9;

std::vector<int> dijkstra(std::vector<std::vector<int> > &graph, int s) {
    int len = graph.size();
    std::vector<int> result(len, INF);
    std::vector<bool> used(len, false);
    result[s] = 0; //起点到起点的距离为0

    for (int i = 1; i < len; i++) {
        int min = -1;
        for (int j = 1; j < len; j++) {
            if (!used[j] && (min == -1 || result[min] > result[j])) {
                //第一轮循环min一定等于s
                min = j; //找出最小的权重
            }
        }

        if (min == -1 || result[min] == INF) {
            //如果最小的权重还是INF，说明剩下的点都不可达
            break;
        }

        used[min] = true;

        //更新所有点的权重
        for (int j = 1; j < len; j++) {
            if (graph[min][j] != INF && !used[j]) {
                //如果min到j的权重不是INF，并且j还没有被访问过
                //最小权重=min(当前权重, 新点的权重 + 新点到j点的权重)
                result[j] = std::min(result[j], result[min] + graph[min][j]);
            }
        }
    }

    return result;
}

int main() {
    int n, m, s;
    std::cin >> n >> m >> s;
    std::vector<std::vector<int> > graph(n + 1, std::vector<int>(n + 1, INF));

    for (int i = 0; i < m; i++) {
        int u, v, w;
        std::cin >> u >> v >> w;
        graph[u][v] = w;
    }
    std::vector<int> result = dijkstra(graph, s);

    for (int i = 1; i <= n; ++i) {
        std::cout << (result[i] == INF ? -1 : result[i]) << (i == n ? '\n' : ' ');
    }
    return 0;
}
