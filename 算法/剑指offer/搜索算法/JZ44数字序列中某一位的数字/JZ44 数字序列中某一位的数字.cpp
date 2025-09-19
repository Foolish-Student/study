#include <string>
using namespace std;

class Solution {
    public:
      /**
       * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
       *
       *
       * @param n int整型
       * @return int整型
       */
      int findNthDigit(int n) {
          // write code here
          long sum = 9;
          long start = 1; // 记录当前区间的起始值
          int digit = 1; // 记录位数
          while (n > sum) {
              n -= sum;
              start *= 10;
              digit++;
              sum = 9 * start * digit;
          }
  
          int indexNum = start + (n - 1) / digit; // 定位到第几个数字
          int index = (n - 1) % digit;  // 定位数字的第几位
  
          return to_string(indexNum)[index] - '0';
      }
  };