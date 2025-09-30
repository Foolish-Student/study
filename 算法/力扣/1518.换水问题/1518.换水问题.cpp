

class Solution {
public:
    int numWaterBottles(int numBottles, int numExchange) {
        int count = numBottles;

        while (numBottles >= numExchange) {
            int exchangeCount = numBottles / numExchange;
            numBottles = exchangeCount + numBottles % numExchange;
            count += exchangeCount;
        }

        return count;
    }
};