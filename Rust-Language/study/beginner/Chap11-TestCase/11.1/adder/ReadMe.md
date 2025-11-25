# 关于 test 函数
**== 被 #[test] 修饰的函数就是 测试函数==**

```Rust
#[cfg(test)]
mod tests {
    /**************************************************
       Attribute marked with #[test] is a test function 

       Use the following command line : 
       $ cargo test 

    ***************************************************/
    #[test]
    fn test_func_2() {
        ...
    }

    #[test]
    fn test_func_1() {
        ...
    }
}

```

# 使用以以命令执行 所有被标注为 #[test]  的测试函数
```Shell
$ cargo test
```


Also see the following ScreenShot
![PNG](ScreenShot.png)



# Attention Please 
注意 : 
1. 同一个文件，**==[并不是]==**先定义的函数，测试执行时，它就先执行的
在这次的测试过程中 
    1. test_func_1() 先执行 ( 虽然 test_func_1() 的实现在文件的行数中，落后于 test_func_2() )
    1. test_func_2() 后执行

1. println!(...) 在测试过程中，不会显示在 控制台的 输出内容中 


