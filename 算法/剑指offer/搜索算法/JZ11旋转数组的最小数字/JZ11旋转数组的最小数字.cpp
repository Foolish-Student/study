#include <vector>
using namespace std;

class Solution {
    public:
      /**
       * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
       *
       *
       * @param nums int整型vector
       * @return int整型
       */
      int minNumberInRotateArray(vector<int>& nums) {
          // write code here
          int left = 0, right = nums.size() - 1;
          while (left < right) {
              int mid = left + (right - left) / 2;
              if(nums[mid] > nums[right]){
                  left = mid + 1;
              }else if(nums[mid] < nums[right]){
                  right = mid;
              }else{
                  right--;
              }
          }
          return nums[right];
      }
  };