class Solution {
public:
    int totalMoney(int n) {
        int mod = n % 7;
        int week = n / 7;
        int sum = 0;
        int sep = 1;
        for (int i = 0; i < week; i++) {
            int last = sep;
            for (int j = 1; j <= 7; j++) {
                sum += last++;
            }
            sep++;
        }

        for (int i = 0; i < mod; i++) {
            sum += sep++;
        }

        return sum;
    }
};