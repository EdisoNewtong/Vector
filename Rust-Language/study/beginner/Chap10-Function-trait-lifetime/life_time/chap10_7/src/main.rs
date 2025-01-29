fn get_longest_string<'a>(s1: &'a str, s2: &'a str) -> &'a str 
{
    if s1.len() > s2.len() {
        s1
    } else {
        s2
    }
}

//   because the return value only get the relativeship with s1 
//   so s2's lifetime can be ignore
fn get_longest_string_v1<'a>(s1: &'a str, s2: &str) -> &'a str 
{
    s1
}

/************************************************** 

error[E0515]: cannot return value referencing local variable `res`
  --> src\main.rs:41:5
   |
41 |     res.as_str()
   |     ---^^^^^^^^^
   |     |
   |     returns a value referencing data owned by the current function
   |     `res` is borrowed here

For more information about this error, try `rustc --explain E0515`.
warning: `chap10_7` (bin "chap10_7") generated 3 warnings
error: could not compile `chap10_7` (bin "chap10_7") due to previous error; 3 warnings emitted

Notes : 
   The return value must be either s1 or s2 because it has the same lifetime 'a with s1 and s2
If the return value is neither s1 nor s2 , 
The <Dangling Reference> will be catched bye the compiler 
**************************************************/
fn get_longest_string_v2<'a>(s1: &'a str, s2: &str) -> &'a str 
{
    let res = String::from("internal_ret_value");
    // [ERROR] @ Compile time
    res.as_str()
}

fn get_longest_string_v3<'a>(s1: &'a str, s2: &'a str) -> &'a str 
{
    let res = "static &str";
    // Compile PASSED
    res 
}

fn get_longest_string_v4<'a>(s1: &'a str, s2: &'a str) -> String
{
    let res = String::from("return local string");
    // Compile PASSED
    res 
}

fn get_longest_string_test1()
{
    println!("----- get_longest_string_test1() -----");
    let s1 = String::from("abcd");
    let s2 = "xyz";

    let s_long = get_longest_string( s1.as_str(), s2 );

    println!("The longest string is {}", s_long );
}

/*
   The field 'part' must live longer than the struct object of ImportantExcept
otherwise if the 'part' reference is expired while the struct object take the reference of 'part' 
the use of  <important_except_object.part>  must be unsafe and the program will be crashed as expected 
*/
struct ImportantExcept<'a> {
    part: &'a str,
}

fn test_struct_lifetime()
{
    let novel = String::from("Call me Ishmael. Some years ago ... ");

    let first_sentence = novel.split('.')
        .next()
        .expect("Could not found a '.'");

    let i = ImportantExcept {  // it will live from                      [86,89)
        // it refers to variable first_sentence ( whose lifetime is from [82,89)   )  
        //     => 'first_sentence' lives longer than 'i'  => Compile PASSED
        part: first_sentence, 
    };
}


//
// No lifetime symbol   'xx . 
//    Why ?
//
// xxxx_v1 will raise a Compile Error at the early version of Rust
//
fn first_word_v1(s: &str) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0 .. i];
        }
    }

    &s[ .. ]
}

//
// Because
//    some special pattern has been write into the compiler's source code
//
// The programmer need not write them again and again
//
fn first_word_v2<'a>(s: &'a str) -> &'a str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0 .. i];
        }
    }

    &s[ .. ]
}

/**********************************************************************
生命周期的省略规则

在 Rust 引用分析中所编入的模式 称为 生命周期省略规则
- 这些规则无需开发者来遵守
- 它们是一些特殊情况， 由编译器来考虑
- 如果你的代码符合这些情况， 那么就无需显式标注生命周期

生命周期的省略规则不会提供完整的推断:
- 如果应用规则后，引用的生命周期仍然模糊不清 =>  编译错误
- 解决办法 : 添加生命周期标注 ，表明引用间的相互关系

输入、输出生命周期
生命周期在
- 函数/方法的参数   :  输入生命周期
- 函数/方法的返回值 :  输出生命周期

生命周期省略的三个规则

编译器使用3个规则在没有显式标注生命周期的情况下，来确定引用的生命周期
- 规则1 应用于[输入]生命周期
- 规则2,3 应用于[输出]生命周期
- 如果编译器应用完3个规则之后， 仍然无法确定生命周期的引用 -> 编译出错
- 这些规则适用于 fn 定义和 impl 块


Rule#1. 每个引用类型的参数都有自己的生命周期
Rule#2. 如果只有1个输入生命周期参数， 那么该生命周期被赋给所有的 输出生命周期参数
Rule#3. 如果有多个输入生命周期参数， 但其中一个是 &self 或 &mut self (是方法) , 
那么self 的生命周期被赋给所有的[输出]生命周期参数

e.g 
1.
    fn first_word(s: &str)  -> &str
 -> fn first_word<'a>(s: &'a str)  -> &str     // use Rule#1
 -> fn first_word<'a>(s: &'a str)  -> &'a str  // use Rule#2


2.
    fn longest(x: &str, y: &str)  -> &str
 -> fn first_word<'a,'b>(x: &'a str, y: &'b str)  -> &str     // use Rule#1

Finally  the compile can not deduce the return value's lifetime
So Compile ERROR


**********************************************************************/
fn main() 
{
    // println!("Hello, world!");
    get_longest_string_test1();

    println!();
    test_struct_lifetime();
}



