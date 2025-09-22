#include <algorithm>
#include <unordered_map>
#include <vector>
using namespace std;

class Solution {
public:
  int maxFrequencyElements(vector<int> &nums) {
    unordered_map<int, int> vis;
    for (auto &num : nums)
      vis[num]++;

    auto it =
        max_element(vis.begin(), vis.end(), [](const auto &a, const auto &b) {
          return a.second < b.second;
        });

    int count = count_if(vis.begin(), vis.end(),
                         [&](const auto &p) { return p.second == it->second; });

    return count * it->second;
  }
};