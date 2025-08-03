#include <iostream>
#include <string>
using namespace std;

int base;
const char digits[] = "0123456789ABCDEFGHIJ";

// 将数字转换为指定进制并返回字符串（高位在前）
string toBase(int num) {
    if (num == 0) return "0";
    
    string result = "";
    while (num > 0) {
        result = digits[num % base] + result;
        num /= base;
    }
    return result;
}

// 检查字符串是否为回文
bool isPalindrome(const string& s) {
    int len = s.length();
    for (int i = 0; i < len / 2; i++) {
        if (s[i] != s[len - 1 - i]) {
            return false;
        }
    }
    return true;
}

int main() {
    cin >> base;
    
    for (int i = 1; i <= 300; i++) {
        int square = i * i;
        string squareStr = toBase(square);
        
        if (isPalindrome(squareStr)) {
            cout << toBase(i) << " " << squareStr << "\n";
        }
    }
    
    return 0;
}
