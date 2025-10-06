// 包含输入输出流库，用于cin和cout操作
#include <iostream>
// 包含向量容器库，用于动态数组操作
#include <vector>
// 包含算法库，用于min函数等算法操作
#include <algorithm>

// 定义无穷大常量，表示两点之间不可达的距离
const int INF = 1e9;

// Dijkstra算法函数：计算从起点s到所有其他点的最短距离
// 参数：graph - 邻接矩阵表示的图，s - 起点编号
// 返回值：从起点到各点的最短距离数组
std::vector<int> dijkstra(std::vector<std::vector<int> > &graph, int s) {
    int len = graph.size(); // 获取图中节点的总数（包括索引0）
    std::vector<int> result(len, INF); // 初始化距离数组，所有距离设为无穷大
    std::vector<bool> used(len, false); // 初始化访问标记数组，所有节点标记为未访问
    result[s] = 0; // 起点到自身的距离设为0

    // 主循环：需要进行n-1轮迭代（起点已在初始化时处理）
    for (int i = 1; i < len; i++) {
        int min = -1; // 初始化最小距离节点的索引为-1（表示还未找到）
        
        // 在所有未访问的节点中找到距离起点最近的节点（节点编号从1开始）
        for (int j = 1; j < len; j++) {
            // 如果节点j未被访问过，且（min还未设置 或 j的距离比当前min的距离更小）
            if (!used[j] && (min == -1 || result[min] > result[j])) {
                min = j; // 更新最小距离节点为j
            }
        }

        // 如果没找到可达的未访问节点，说明剩余节点都不可达
        if (min == -1 || result[min] == INF) {
            break; // 提前结束算法
        }

        used[min] = true; // 将找到的最近节点标记为已访问

        // 松弛操作：通过新加入的节点min更新其他未访问节点的最短距离
        for (int j = 1; j < len; j++) {
            // 如果存在从min到j的边（权重不为INF）且j未被访问过
            if (graph[min][j] != INF && !used[j]) {
                // 尝试通过min节点更新到j的最短距离
                // 新距离 = 起点到min的距离 + min到j的距离
                result[j] = std::min(result[j], result[min] + graph[min][j]);
            }
        }
    }

    return result; // 返回从起点到所有节点的最短距离数组
}

int main() {
    int n, m, s; // n-节点数，m-边数，s-起点编号
    std::cin >> n >> m >> s; // 读入图的基本信息
    
    // 创建邻接矩阵，大小为(n+1)×(n+1)，初始值为INF
    // 使用n+1是为了让节点编号从1开始，忽略索引0
    std::vector<std::vector<int> > graph(n + 1, std::vector<int>(n + 1, INF));

    // 读入m条边的信息并构建邻接矩阵
    for (int i = 0; i < m; i++) {
        int u, v, w; // u-起点，v-终点，w-权重
        std::cin >> u >> v >> w; // 读入一条边的信息
        graph[u][v] = w; // 在邻接矩阵中记录这条边（有向图）
    }
    
    // 调用Dijkstra算法计算最短距离
    std::vector<int> result = dijkstra(graph, s);

    // 输出结果：从起点s到每个节点的最短距离
    for (int i = 1; i <= n; ++i) {
        // 如果距离为INF则输出-1（表示不可达），否则输出实际距离
        std::cout << (result[i] == INF ? -1 : result[i]) << (i == n ? '\n' : ' ');
    }
    return 0; // 程序正常结束
}
