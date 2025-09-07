#include <stdio.h>
#include <dlfcn.h>  // 用于动态加载库
#include "liba.h"

int main() {
    printf("=== 动态库依赖关系测试 ===\n");
    printf("依赖关系: 主程序 -> 动态库A -> 动态库B -> 动态库C\n\n");
    
    // 测试1: 直接调用函数
    printf("测试1: 直接调用动态库A的函数\n");
    printf("----------------------------------------\n");
    func_a();
    printf("\n");
    
    // 测试2: 调用带参数的函数
    printf("测试2: 调用带参数的函数\n");
    printf("----------------------------------------\n");
    int result = execute_a(10);
    printf("主程序: 最终返回结果 = %d\n\n", result);
    
    // 测试3: 动态加载库（可选）
    printf("测试3: 动态加载库测试\n");
    printf("----------------------------------------\n");
    
    // 加载动态库A
    void* handle_a = dlopen("./liba.so", RTLD_LAZY);
    if (!handle_a) {
        printf("无法加载动态库A: %s\n", dlerror());
        return 1;
    }
    
    // 获取函数指针
    void (*dynamic_func_a)() = dlsym(handle_a, "func_a");
    if (!dynamic_func_a) {
        printf("无法找到函数func_a: %s\n", dlerror());
        dlclose(handle_a);
        return 1;
    }
    
    printf("通过动态加载调用func_a:\n");
    dynamic_func_a();
    
    // 清理
    dlclose(handle_a);
    
    printf("\n=== 测试完成 ===\n");
    return 0;
}
