#include <vector>
using namespace std;

class Solution {
public:
    int minimumOperations(vector<int>& nums) {
        int cont = 0;
        int n = nums.size();
        for(int i = 0; i < n; i++){
            cont += nums[i] % 3 != 0;
        }
        return cont;
    }
};