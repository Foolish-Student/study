#include <iostream>
#include <stack>
#include <string>
#include <vector>

bool isLeftBracket(char c) {
    return c == '(' || c == '[' || c == '{';
}

bool isRightBracket(char c) {
    return c == ')' || c == ']' || c == '}';
}

bool isMatchingPair(char left, char right){
    return left == '(' && right == ')' ||
           left == '[' && right == ']' ||
           left == '{' && right == '}';
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int precedence(char op) {
    if (op == '*' || op == '/')
        return 2;
    else if (op == '+' || op == '-')
        return 1;
    else
        return 0;
}

std::vector<std::string> infixToPostfix(const std::string& infix){
    std::stack<char> operators;
    std::vector<std::string> postfix;

    int n = infix.size();
    int i = 0;
    while (i < n){
        if(isdigit(infix[i])){
            std::string num;
            while (i < n && isdigit(infix[i])) {
                num += infix[i];
                i++;
            }
            postfix.emplace_back(num);
        } else if (isLeftBracket(infix[i])) {
            operators.push(infix[i++]);
        }else if (isRightBracket(infix[i])) {
            while(!operators.empty() && !isLeftBracket(operators.top())) {
                postfix.emplace_back(std::string(1, operators.top()));
                operators.pop();
            }

            if (!operators.empty()) {
                operators.pop();
            } else {
                std::cout << "括号不匹配" << std::endl;
            }
            i++;
        }else if (isOperator(infix[i])) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(infix[i])) {
                postfix.emplace_back(std::string(1, operators.top()));
                operators.pop();
            }
            operators.push(infix[i]);
            i++;
        }else{
            i++;
            std::cout << "表达式错误" << std::endl;
        }
    }

    while (!operators.empty()) {
        postfix.emplace_back(std::string(1, operators.top()));
        operators.pop();
    }

    return postfix;
}

int main(){
    std::vector<std::string> postfix = infixToPostfix("5+(2+3)*(4+1)+(3+2)");
    for (const auto& token : postfix) {
        std::cout << token << " ";
    }
    return 0;
}