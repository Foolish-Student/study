# 常见 Sanitizers 一览
| 名称                                     | 作用                          | 怎么用（最小命令）                                  | 备注                                                   |
| -------------------------------------- | --------------------------- | ------------------------------------------ | ---------------------------------------------------- |
| **ASan** (AddressSanitizer)            | 越界、UAF、double free、栈/堆溢出    | `-fsanitize=address -g -O0`                | 不能和 TSan/MSan 同时用；可与 UBSan 同用                        |
| **LSan** (LeakSanitizer)               | 内存泄漏                        | （随 ASan/TSan 在 x86\_64 Linux 上**自动启用**）    | 控制用 `ASAN_OPTIONS=detect_leaks=1` 或 `LSAN_OPTIONS=…` |
| **TSan** (ThreadSanitizer)             | 数据竞争、锁误用（部分）                | `-fsanitize=thread -g -O1`                 | **不要**与 ASan 同编；开销较大                                 |
| **UBSan** (UndefinedBehaviorSanitizer) | 未定义行为（越界、移位、除 0、对齐、空指针、溢出等） | `-fsanitize=undefined -g -O1`              | 和 ASan 很搭；也可只开子集（见下）                                 |
| **MSan** (MemorySanitizer)             | 使用未初始化内存（UUM）               | `-fsanitize=memory -g -O1`                 | 需要**所有依赖库都带 MSan**；与 ASan/TSan 互斥                    |
| **HWASan** (Hardware-assisted ASan)    | 更低内存开销的 ASan（AArch64 主场）    | `-fsanitize=hwaddress -g -O1`              | 主要在 64 位 ARM/Android                                 |
| **DFSan** (DataFlowSanitizer)          | 污点跟踪（自定义标签传播）               | `-fsanitize=dataflow`                      | 研究/安全场景，需写标签逻辑                                       |
| **CFI** (Control Flow Integrity)       | 控制流完整性（间接调用/虚表保护）           | `-fsanitize=cfi -flto -fvisibility=hidden` | 需 LTO，主要 Clang                                       |
| **SafeStack / ShadowCallStack**        | 保护返回地址/安全栈                  | `-fsanitize=safe-stack`（Clang）             | 更像防护特性，不是典型 runtime 报错器                              |
