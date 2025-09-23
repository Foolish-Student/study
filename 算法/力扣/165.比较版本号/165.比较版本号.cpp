#include <string>
#include <vector>
using namespace std;

class Solution {
public:
  vector<int> splitVersion(string version) {
    vector<int> result;
    int start = 0;
    int i = 0;
    for (i = 0; i < version.size(); i++) {
      if (version[i] == '.') {
        result.push_back(stoi(version.substr(start, i - start)));
        start = i + 1;
      }
    }
    result.push_back(stoi(version.substr(start, i - start)));
    return result;
  }


  void removeZero(vector<int> &v, int length) {
    int i = v.size() - 1;
    while (length < v.size() && v[i] == 0) {
        v.erase(v.begin() + i);
        i--;
    }
  }

  int compareVersion(string version1, string version2) {
    vector<int> v1 = splitVersion(version1);
    vector<int> v2 = splitVersion(version2);
    int i = 0;
    while (i < v1.size() && i < v2.size()) {
      if (v1[i] > v2[i]) {
        return 1;
      }
      if (v1[i] < v2[i]) {
        return -1;
      }
      i++;
    }
    // 去掉无用的 0
    removeZero(v1, i);
    removeZero(v2, i);
    
    if (v1.size() > v2.size()) {
      return 1;
    } else if (v1.size() < v2.size()) {
      return -1;
    }else{
        return 0;
    }
  }
};