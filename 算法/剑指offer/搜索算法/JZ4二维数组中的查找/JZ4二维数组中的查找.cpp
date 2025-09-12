#include <vector>
using namespace std;


class Solution {
    public:
        /**
         * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
         *
         * 
         * @param target int整型 
         * @param array int整型vector<vector<>> 
         * @return bool布尔型
         */
        bool Find(int target, vector<vector<int> >& array) {
            // write code here
    
            int n = array.size();
            if(n <= 0){
                return false;
            }
            int m = array[0].size();
    
            int r = 0;
            for(int i = n - 1; i >= 0; i--)
            {
                while(true){
                    if(r >= m)
                        return false;
    
                    int temp = array[i][r];
                    if(temp > target){
                        break;
                    }else if (temp < target){
                        r++;
                    }else{
                        return true;
                    }
                }
            }
    
            return false;
        }
    };