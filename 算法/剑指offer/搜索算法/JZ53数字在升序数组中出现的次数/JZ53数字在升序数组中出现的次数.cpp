#include <vector>
using namespace std;


class Solution {
    public:
      /**
       * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
       *
       *
       * @param nums int整型vector
       * @param k int整型
       * @return int整型
       */
      int FirstIndex(vector<int>& nums, int k) {
          int right = nums.size() - 1;
          int left = 0;
          int mid = 0;
          while (left <= right) {
              mid = (right + left) / 2;
  
              if ( nums[mid] < k) {
                  left = mid + 1;
              } else {
                  right = mid - 1;
              }
          }
          return left;
      }
      int LastIndex(vector<int>& nums, int k) {
          int right = nums.size() - 1;
          int left = 0;
          int mid = 0;
          while (left <= right) {
              mid = (right + left) / 2;
              if (nums[mid] <= k) {
                  left = mid + 1;
              } else {
                  right = mid - 1;
              }
          }
          return right;
      }
      int GetNumberOfK(vector<int>& nums, int k) {
          // write code here
          int first = FirstIndex(nums, k);
          int last = LastIndex(nums, k);
          if (first > last)
              return 0;
  
          return  last - first + 1;
      }
  };