# ldd linux 依赖库检查工具

## 概述
`ldd` 是 linux 系统自带的依赖库检查工具，用于查看可执行文件和动态库的依赖关系，检查是否缺失依赖库。

## 基本用法

### 语法
```bash
ldd <可执行文件/动态库>
```

### 使用示例
```bash
$ ldd liba.so
        linux-vdso.so.1 (0x00007fff8d1fe000)
        libb.so => ./libb.so (0x00007f8b2c4a0000)
        libc.so => ./libc.so (0x00007f8b2c490000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8b2c2a0000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f8b2c4b0000)
```

### 输出说明
- **库名 => 路径 (内存地址)**：找到的依赖库及其位置
- **库名 => not found**：缺失的依赖库
- **linux-vdso.so.1**：虚拟动态共享对象，由内核提供
- **/lib64/ld-linux-x86-64.so.2**：动态链接器

## 常见问题排查

### 缺失依赖示例
```bash
$ ldd ./my_program
        libmissing.so => not found
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8b2c2a0000)
```

## 总结
`ldd` 是 linux 下检查依赖库的核心工具：
- ✅ 快速检查程序依赖关系
- ✅ 识别缺失的动态库
- ✅ 排查运行时库加载问题
- ✅ 验证库的版本兼容性