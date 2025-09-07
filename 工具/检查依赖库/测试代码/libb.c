#include <stdio.h>
#include "libb.h"
#include "libc.h"  // 依赖动态库C

void func_b() {
    printf("调用了动态库B中的func_b函数\n");
    printf("动态库B调用动态库C的函数:\n");
    func_c();  // 调用动态库C的函数
}

int process_b(int x) {
    printf("动态库B: 处理输入值 %d\n", x);
    int result = calculate_c(x + 5);  // 调用动态库C的函数
    printf("动态库B: 最终结果 %d\n", result);
    return result;
}
