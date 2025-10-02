class Solution {
public:
    int maxBottlesDrunk(int numBottles, int numExchange) {
        int result = numBottles;
        int emptyBot = numBottles;
        while (emptyBot >= numExchange) {
            emptyBot -= numExchange; // 兑换
            emptyBot++;              // 喝水
            result++;                // 记录
            numExchange++;           // 累加
        }
        return result;
    }
};