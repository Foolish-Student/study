// https://leetcode.cn/problems/jump-game-iv/description/?envType=daily-question&envId=2026-05-18

#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

class Solution {
public:
    int minJumps(vector<int>& arr) {
        unordered_map<int, vector<int>> idx;
        for (int i = 0; i < arr.size(); ++i) {
            idx[arr[i]].push_back(i);
        }

        vector<bool> visited(arr.size(), false);
        visited[0] = true;
        struct Node {
            int idx, step;
        };
        queue<Node> q;
        q.push(Node{0, 0});
        while(!q.empty()) {
            Node node = q.front();
            q.pop();
            if (node.idx == arr.size() - 1) {
                return node.step;   
            }

            vector<int>& next = idx[arr[node.idx]];
            for (int i : next) {
                if (i >= 0 && i < arr.size() && !visited[i]) {
                    visited[i] = true;
                    q.push(Node{i, node.step + 1});
                }
            }
            next.clear();

            int left = node.idx - 1, right = node.idx + 1;
            if( left >= 0 && left < arr.size() && !visited[left]) {
                visited[left] = true;
                q.push(Node{left, node.step + 1});
            }
            if( right >= 0 && right < arr.size() && !visited[right]) {
                visited[right] = true;
                q.push(Node{right, node.step + 1});
            }
        }

        return 0;
    }
};