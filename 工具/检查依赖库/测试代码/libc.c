#include <stdio.h>
#include "libc.h"

void func_c() {
    printf("调用了动态库C中的func_c函数\n");
}

int calculate_c(int x) {
    printf("动态库C: 计算 %d * 2 = %d\n", x, x * 2);
    return x * 2;
}
