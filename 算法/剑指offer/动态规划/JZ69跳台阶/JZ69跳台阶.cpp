class Solution {
    public:
        /**
         * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
         *
         * 
         * @param number int整型 
         * @return int整型
         */
        int jumpFloor(int number) {
            if (number == 1) return 1;
            // write code here
            int last1 = 1, last2 = 1, sum = 0;
            for(int i = 2; i <= number; i++){
                sum = last1 + last2;
                last2 = last1;
                last1 = sum;
            }
    
            return sum;
        }
    };