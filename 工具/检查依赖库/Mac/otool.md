# otool -L：macOS 依赖库检查工具

## 概述
`otool -L` 是 macOS 系统自带的依赖库检查工具，用于查看可执行文件和动态库的依赖关系，检查是否缺失依赖库。

## 基本用法

### 语法
```bash
otool -L <可执行文件/动态库>
```

### 使用示例
```bash
huiyuanai@192 动态库依赖 % otool -L liba.so
liba.so:
        liba.so (compatibility version 0.0.0, current version 0.0.0)
        libb.so (compatibility version 0.0.0, current version 0.0.0)
        libc.so (compatibility version 0.0.0, current version 0.0.0)
        /usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1351.0.0)
```

## 总结
`otool -L` 是 macOS 下检查依赖库的核心工具：
- ✅ 快速检查程序依赖关系
- ✅ 识别缺失的动态库
- ✅ 排查运行时库加载问题
- ✅ 验证库的版本兼容性