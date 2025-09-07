#include <stdio.h>
#include "liba.h"
#include "libb.h"  // 依赖动态库B

void func_a() {
    printf("调用了动态库A中的func_a函数\n");
    printf("动态库A调用动态库B的函数:\n");
    func_b();  // 调用动态库B的函数，B会进一步调用C
}

int execute_a(int x) {
    printf("动态库A: 开始执行，输入值 %d\n", x);
    int result = process_b(x * 3);  // 调用动态库B的函数
    printf("动态库A: 执行完成，最终结果 %d\n", result);
    return result;
}
