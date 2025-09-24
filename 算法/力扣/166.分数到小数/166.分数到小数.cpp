#include <string>
#include <unordered_map>
using namespace std;

class Solution {
public:
    string fractionToDecimal(int numerator, int denominator) {

        long long x = numerator;
        long long y = denominator;
        if (x % y == 0) {
            return to_string(x / y);
        }

        string result = "";
        if (x < 0 && y > 0 || x > 0 && y < 0) {
            result = "-";
        }

        x = abs(x);
        y = abs(y);

        result += to_string(x / y) + ".";
        x %= y;
        unordered_map<long long, int> map;

        while (x > 0) {
            map[x] = result.size();
            x *= 10;
            long long t = x % y;
            result += to_string(x / y);
            x %= y;

            if (map.find(t) != map.end()) {
                int index = map[t];
                result.insert(index, "(");
                result += ")";
                return result;
            }
        }

        return result;
    }
};