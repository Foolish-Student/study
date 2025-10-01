#include <vector>
using namespace std;


class Solution {
  public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     *
     * @param numbers int整型vector
     * @return int整型
     */
    int duplicate(vector<int>& numbers) {
        // write code here
        int n = numbers.size();
        int i = 0;
        while (i < n) {
            int number = numbers[i];
            if (number == i) {
                i++;
                continue;
            } else {
                if (number == numbers[number]) {
                    return number;
                } else {
                    swap(numbers[i], numbers[number]);
                }
            }
        }
        return -1;
    }
};