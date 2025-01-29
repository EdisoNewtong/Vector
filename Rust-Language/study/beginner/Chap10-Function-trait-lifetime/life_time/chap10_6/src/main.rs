/****************************************************************************************************************************************
  There is a lifetime named 'a ,  
  Both lifetime of s1 and s2 in the parameters should no less than lifetime  'a 
  the lifetime of the result value should no less than lifetime  'a

  'a will not change the lifetime of passed real arguments and return value

  'a will be replaced as the intersection part of lifetime of 2 real arguments s1 and s2  ( or the minimum one of s1 and s2 )
****************************************************************************************************************************************/
fn get_longest_string<'a>(s1: &'a str, s2: &'a str) -> &'a str 
{
    if s1.len() > s2.len() {
        s1
    } else {
        s2
    }
}

fn get_longest_string_test1()
{
    println!("----- get_longest_string_test1() -----");
    let s1 = String::from("abcd");
    let s2 = "xyz";

    let s_long = get_longest_string( s1.as_str(), s2 );

    println!("The longest string is {}", s_long );
}

fn get_longest_string_test2()
{
    println!("----- get_longest_string_test2() -----");
    let s1 = String::from("abcd");
    {
        let s2 = "xyz"; // static lifetime  : it is also live during the program's running period
        let s_long = get_longest_string( s1.as_str(), s2 );
        println!("The longest string is {}", s_long );
    }
}



/****************************************************************************************************************************************

error[E0597]: `s2` does not live long enough
  --> src\main.rs:70:51
   |
68 |         let s2 = String::from("xyz");
   |             -- binding `s2` declared here
69 |         // [ERROR] @ Compile time , because it's possible to return s2 as the return value
70 |         s_long = get_longest_string( s1.as_str(), s2.as_str() );
   |                                                   ^^ borrowed value does not live long enough
71 |     } // if s2 is the return value, it will be droped here
   |     - `s2` dropped here while still borrowed
72 |     println!("The longest string is {}", s_long ); // and then there maybe a reference of the expired string s2 , so compile error
   |                                          ------ borrow later used here

For more information about this error, try `rustc --explain E0597`.
error: could not compile `chap10_6` (bin "chap10_6") due to previous error

****************************************************************************************************************************************/
fn get_longest_string_test3()
{
    println!("----- get_longest_string_test3() -----");
    let s1 = String::from("abcd");
    let s_long;
    { // 'a is the overlapped lifetime of (s1 and s2) =>  yield s2  ( the mimimum one of s1 and s2 =>  because s2's lifetime is shorter than s1 )
        let s2 = String::from("xyz");
        // [ERROR] @ Compile time , because it's possible to return s2 as the return value
        s_long = get_longest_string( s1.as_str(), s2.as_str() );
    } // if s2 is the return value, it will be droped here
    println!("The longest string is {}", s_long ); // and then there maybe a reference of the expired string s2 (or s1) , so compile error
}



fn main() 
{
    get_longest_string_test1();

    println!();
    get_longest_string_test2();

    println!();
    get_longest_string_test3();
}



