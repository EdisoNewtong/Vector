# 14.1
## command line : Cargo build
```Shell
$ cargo build              # Default for                <dev>     profile 
$ cargo build --release    # Explicitly specified for   <release> profile 
```


## Self-Defined build configuration 
1. modify the config file named 'Cargo.toml' 

```toml 
// Add the following lines to config dev profile
[profile.dev]
opt-level = 1           // 0 ( unoptimized ) is the default value for dev whose range is [0,3] 

[profile.release]
opt-level = 3           // 3 is the default value for release [0,3]

```

# 14.2 
## Document comment

```Rust
/* ********************************************************

"Document comment" begins with    3 successive /   

**********************************************************/


/// A line is starts with 3 / is a kind of  "Document comment" 
/// You can use write down some comment by MarkDown syntax
///
/// # Examples
///
/// ```Rust
///    let num1   = 5;
///    let answer = my_crate::add_one(num1);
///    
///    assert_eq!(6, answer);
///
///    let num2    = 6;
///    let answer2 = my_crate::add_one(num2);
///    
///    assert_eq!(6, answer2);
/// ```

pub fn add_one(x: i32) -> i32 {
    x + 1
}

```

You can use the following command line to build a document with html format
```Shell
$ cargo doc           // generate the html document only (it will be located at    target/doc/<my_crate>/index.html )
$ cargo doc --open    // generate the html document and then open it by the default web browser 

# 
///    assert_eq!(6, answer2);  will be failed by the test 
$ cargto test         // it will make a test from the /// document comment


```


## Crate Comment

```Rust
//!  This line is a  'Crate Comment'
//!       line 1 ...
//!       line 2 ...
//!       line 3 ...
//!          ...
//!          ...
//!       line n ...

```


