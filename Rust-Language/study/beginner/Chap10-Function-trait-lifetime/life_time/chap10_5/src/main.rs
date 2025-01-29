/******************************************************************************
| The Borrow Checker | play an important role inside Rust compiler

The Rust compiler has a borrow checker that compares scopes to determine whether all borrows are valid. 

error[E0597]: `x` does not live long enough
  --> src\main.rs:39:13
   |
33 |         let x = 5;                                  // -----+-- 'b' start   |
   |             - binding `x` declared here
...
39 |         r = &x;                                     //      |               |
   |             ^^ borrowed value does not live long enough
40 |     }                                               // -----+-- 'b' end     |
   |     - `x` dropped here while still borrowed
41 |     println!("r = {}",r);                           //                      |
   |                       - borrow later used here

For more information about this error, try `rustc --explain E0597`.
error: could not compile `chap10_5` (bin "chap10_5") due to previous error


******************************************************************************/
/*  

    Core Core Core : lifetime of 'b is shorter than lifetime 'a
Unfortunately 'a got the address existed in 'b   =>  [Error] @ Compile time  
Key Error : The lifetime of 'x' which is referred by r , is shorter than the lifetime of 'r' ( reference owner )
*/
fn main() {
    let r;                                          // ---------------------+-- 'a start
    {                                               //                      |
        let x = 5;                                  // -----+-- 'b' start   |
        /*                                          //      |               |
          r pointer to x's address                  //      |               |
          which will be expired comming soon        //      |               |
          at line: 26                               //      |               |
        */                                          //      |               |
        r = &x;                                     //      |               |
    }                                               // -----+-- 'b' end     |
    println!("r = {}",r);                           //                      |
}                                                   //----------------------+-- 'a end 


/***************************************************
// How to fix it ?
// use the following code 
fn main() 
{
    let x = 5;
    let r = &x;
    println!("r = {}",r);
}
*/

/********************************************************************

error[E0106]: missing lifetime specifier
  --> src\main.rs:83:41
   |
83 | fn cmp_2_strings(s1: &str, s2: &str) -> &str
   |                      ----      ----     ^ expected named lifetime parameter
   |
   = help: this function's return type contains a borrowed value, but the signature does not say whether it is borrowed from `s1` or `s2`
help: consider introducing a named lifetime parameter
   |
83 | fn cmp_2_strings<'a>(s1: &'a str, s2: &'a str) -> &'a str
   |                 ++++      ++           ++          ++

For more information about this error, try `rustc --explain E0106`.
error: could not compile `chap10_5` (bin "chap10_5") due to previous error

*********************************************************************/
fn cmp_2_strings_test()
{
    let string1 = String::from("abcd");
    let string2 = "xyz";

    let long_str = cmp_2_strings(string1.as_str(), string2);
    println!("The longest string is {}", long_str );
}

fn cmp_2_strings(s1: &str, s2: &str) -> &str 
{
    if s1.len() > s2.len() {
        s1
    } else {
        s2
    }
}

/***************************************************
// How to fix it ?  <'a'> is a kind of name of lifetime 
// Commonly it only take one lowercase letter after  the lifetime  symbol '    in Rust
fn cmp_2_strings<'a>(s1: &'a str, s2: &'a str) -> &'a str 
{
    if s1.len() > s2.len() {
        s1
    } else {
        s2
    }
}


fn cmp_2_strings(s1: &str, s2: &str) -> &str 
{
    // the change of previous code doesn't work , so the compile-time error take no relationship of the implementation of function's body
    // it's only relevant to the signature of the function 
    // it can not change the lifetime of the real passed arguments
    // it only tell the compiler the lifetime relativeship between the paramenters and the return value

    s1   
}
***************************************************/
