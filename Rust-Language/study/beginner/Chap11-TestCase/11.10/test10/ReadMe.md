# Notes
在 与 src/ 目录同一级，创建 tests 目录
Rust 的 $ cargo test 会根据此默认的目录名，
为 tests/ 目录下的所有源文件(仅tests/下, 而**==非递归==** )，生成一个 test-case 的二进制可执行文件

所有在 tests 目录下的 Rust 代码源文件都被视为 Test Case
以下命令会将以下源代码
1. another_integration_test.rs
1. integration_test.rs
1. common.rs

生成 3个 可执行的测试文件

如果**==不希望如此==** ，只希望 common.rs 是一个公共的帮助函数
被 another_integration_test.rs / integration_test.rs 调用，
那么 可以在 tests/ 目录下，**== 再新建一个新的目录==** (e.g   tests/util )
将 common.rs 的代码移动到 util/mod.rs 中去，然后就**==可以删除==** common.rs 文件了


```Shell
$ cargo test
```

# 目录树型结构 
<ProjectName>-
             |-- src
             |-- **==tests==**
                         |--- another_integration_test.rs
                         |--- integration_test.rs
                         |--- common.rs
                         |--- util
                                |---- mod.rs



