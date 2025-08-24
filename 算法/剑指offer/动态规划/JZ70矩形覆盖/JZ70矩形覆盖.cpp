class Solution {
public:
    int rectCover(int number) {
        if(number == 0){
            return 0;
        }
        if(number == 1){
            return 1;
        }

        int last1 = 1, last2 = 1;
        for(int i = 2; i <= number; i++){
            int curr = last1 + last2;
            last2 = last1;
            last1 = curr;
        }

        return last1;
    }
};

