// https://leetcode.cn/problems/walking-robot-simulation/description/?envType=daily-question&envId=2026-04-06


#include <unordered_set>
#include <vector>
using namespace std;

class Solution {
public:
    enum direction { north = 0, east = 1, south = 2, west = 3 };

    int robotSim(vector<int>& commands, vector<vector<int>>& obstacles) {
        int steps[][2] = {
            {0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // 北、东、南、西的移动方向
        int commandsSize = commands.size();
        int obstaclesSize = obstacles.size();
        int x = 0, y = 0;
        int maxDistance = 0;
        int currentDirection = north;

        unordered_set<int> obstacleSet;
        for (const auto& obstacle : obstacles) {
            int ox = obstacle[0];
            int oy = obstacle[1];
            obstacleSet.insert(ox * 60001 + oy); // 将障碍物坐标映射到一个整数上
        }

        for (int i = 0; i < commandsSize; ++i) {
            int move = commands[i];
            if (move == -2) {
                currentDirection = (currentDirection + 3) % 4;
            } else if (move == -1) {
                currentDirection = (currentDirection + 1) % 4;
            } else {
                for (int j = 0; j < move; ++j) {
                    int nextX = x, nextY = y;
                    nextX += steps[currentDirection][0];
                    nextY += steps[currentDirection][1];

                    if (obstacleSet.find(nextX * 60001 + nextY) ==
                        obstacleSet.end()) {
                        x = nextX;
                        y = nextY;
                        maxDistance = max(maxDistance, x * x + y * y);
                    } else {
                        break; // 遇到障碍物，停止前进
                    }
                }
            }
        }
        return maxDistance;
    }
};