// https://leetcode.cn/problems/jump-game-iii/description/?envType=daily-question&envId=2026-05-17

#include <vector>

using namespace std;


class Solution {
public:
    bool canReach(vector<int>& arr, int start) {
        if (arr[start] == 0){
            return true;
        }
        int arrSize = arr.size();
        vector<bool> visited(arrSize, false);
        return dfs(arr, visited, start);
    }

    bool dfs(vector<int>& arr, vector<bool>& visited, int index){
        if (index < 0 || index >= arr.size() || visited[index]){
            return false;
        }
        visited[index] = true;
        if (arr[index] == 0){
            return true;
        }
        return dfs(arr, visited, index + arr[index]) || dfs(arr, visited, index - arr[index]);
    }
};