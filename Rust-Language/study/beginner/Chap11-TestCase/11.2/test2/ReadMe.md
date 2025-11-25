# assert!(...)    // bool expression

# equal and not_equal
1. assert_eq!(...) // ==
1. assert_ne!(...) // !=

e.g. 
```Rust

    assert_eq!(5, 2+3); // PASSED
    assert_ne!(5, 4+1); // Test FAILED  left: 5, right:5   equal

```

以上2个函数会在 test 失败时，打印 左值的具体数值 和 右值的具体数值，方式追溯错误的原因 


