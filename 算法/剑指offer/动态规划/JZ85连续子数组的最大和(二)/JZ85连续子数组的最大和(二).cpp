#include <vector>
using namespace std;

class Solution {
    public:
      /**
       * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
       *
       *
       * @param array int整型vector
       * @return int整型vector
       */
      vector<int> FindGreatestSumOfSubArray(vector<int>& array) {
          // write code here
          int lastLeft = 0, lastRight = 0, curLeft = 0, curRight = 0, sum = -999, curSum = -999;
          int arraySize = array.size();
          for (int i = 0; i < arraySize; i++) {
              if (curSum + array[i] < array[i]) {
                  curSum = array[i];
                  curLeft = i;
                  curRight = i;
              } else {
                  curSum += array[i];
                  curRight++;
              }
  
              if (curSum > sum) {
                  sum = curSum;
                  lastLeft = curLeft;
                  lastRight = curRight;
              } else if (curSum == sum && curRight - curLeft > lastRight - lastLeft) {
                  lastLeft = curLeft;
                  lastRight = curRight;
              }
          }
  
          std::vector<int> result(array.begin() + lastLeft,
                                  array.begin() + (lastRight + 1));
          return result;
      }
  };