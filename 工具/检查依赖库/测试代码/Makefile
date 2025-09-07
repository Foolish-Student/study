# 编译器和编译选项
CC = gcc
CFLAGS = -Wall -fPIC -g
LDFLAGS = -shared

# 目标文件
TARGETS = libc.so libb.so liba.so main

# 默认目标
all: $(TARGETS)

# 编译动态库C（最底层，无依赖）
libc.so: libc.c libc.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ libc.c
	@echo "动态库C编译完成"

# 编译动态库B（依赖C）
libb.so: libb.c libb.h libc.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ libb.c -L. -lc
	@echo "动态库B编译完成"

# 编译动态库A（依赖B，间接依赖C）
liba.so: liba.c liba.h libb.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ liba.c -L. -lb -lc
	@echo "动态库A编译完成"

# 编译主程序（依赖A）
main: main.c liba.so
	$(CC) $(CFLAGS) -o $@ main.c -L. -la -lb -lc -ldl
	@echo "主程序编译完成"

# 运行测试
run: main
	@echo "设置动态库搜索路径并运行测试..."
	export LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH && ./main

# 检查动态库依赖关系
check-deps: $(TARGETS)
	@echo "=== 检查动态库依赖关系 ==="
	@echo "动态库C的依赖:"
	-ldd libc.so
	@echo ""
	@echo "动态库B的依赖:"
	-ldd libb.so
	@echo ""
	@echo "动态库A的依赖:"
	-ldd liba.so
	@echo ""
	@echo "主程序的依赖:"
	-ldd main

# 使用otool检查依赖（macOS）
check-deps-mac: $(TARGETS)
	@echo "=== 检查动态库依赖关系 (macOS) ==="
	@echo "动态库C的依赖:"
	-otool -L libc.so
	@echo ""
	@echo "动态库B的依赖:"
	-otool -L libb.so
	@echo ""
	@echo "动态库A的依赖:"
	-otool -L liba.so
	@echo ""
	@echo "主程序的依赖:"
	-otool -L main

# 清理编译产物
clean:
	rm -f *.so main *.o
	@echo "清理完成"

# 重新编译
rebuild: clean all

# 帮助信息
help:
	@echo "可用的make目标:"
	@echo "  all          - 编译所有目标"
	@echo "  libc.so      - 编译动态库C"
	@echo "  libb.so      - 编译动态库B"
	@echo "  liba.so      - 编译动态库A"
	@echo "  main         - 编译主程序"
	@echo "  run          - 运行测试程序"
	@echo "  check-deps   - 检查依赖关系 (Linux)"
	@echo "  check-deps-mac - 检查依赖关系 (macOS)"
	@echo "  clean        - 清理编译产物"
	@echo "  rebuild      - 重新编译"
	@echo "  help         - 显示此帮助信息"

.PHONY: all run check-deps check-deps-mac clean rebuild help
