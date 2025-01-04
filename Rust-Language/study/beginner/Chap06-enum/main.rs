/*********************************************************************************************
 As a Rust naming convention , the enum name should be Capitalized without camel 


  enum ipAddrType { ... } 
       ^^^^^^^^^^ help: convert the identifier to upper camel case (notice the capitalization): `IpAddrType`
*********************************************************************************************/
#[derive(Debug)]
enum IpAddrType {

    V4,  /* = 0    defaultly */   // as a Rust naming convention , the first letter of a enum kind should be uppercase , like C , the value of V4 is a int number start from 0 ( Zero )
    V6,                           // the value of V6 is the interger next to 0    : 1
}


/*
 error[E0382]: borrow of moved value: `var_ipv4`
  --> main.rs:38:33
   |
36 |     let var_ipv4:     IpAddrType = IpAddrType::V4;
   |         -------- move occurs because `var_ipv4` has type `IpAddrType`, which does not implement the `Copy` trait
37 |     let var_ipv4_val: i32        = var_ipv4 as i32;
   |                                    -------- value moved here
38 |     println!("var_ipv4 = {:?}", var_ipv4);
   |                                 ^^^^^^^^ value borrowed here after move
   |
   = note: this error originates in the macro `$crate::format_args_nl` which comes from the expansion of the macro `println` (in Nightly builds, run w
ith -Z macro-backtrace for more info)

error: aborting due to previous error

For more information about this error, try `rustc --explain E0382`.
*/
// fn try_convert_enum_2_int()
// {
//     let var_ipv4:     IpAddrType = IpAddrType::V4;
//     let var_ipv4_val: i32        = var_ipv4 as i32;
//     println!("var_ipv4 = {:?}", var_ipv4);               // TODO : Issue occured Here ? How to solve it ???
//     println!("var_ipv4_val = {}", var_ipv4_val);
// 
// }






/*
     将数据附加到枚举的变体中 , 
or        或者 结构体中 某个字段的类型可以是  枚举类型

e.g.
      //  优点:
      //       - 不需要额外使用 struct   
      //       - 每个变体拥有不同的类型 以及关联的数据量
enum IpAddr_1 {
    V4(String),
    V6(String)
}

enum IpAddr_2 {
    V4(u8, u8, u8, u8),    // e.g.  let addr_v4: IpAddr_2 = IpAddr_2::V4(192.168.1.4);
    V6(String),            //       let addr_v6: IpAddr_2 = IpAddr_2::V6( String::from("2001:0db8:85a3:0000:0000:8a2e:0370:7334") );
}


####################################################################################################
struct IpAddr {
    addr: String,
    type: IpAddrType,   // enum type varible
}


The defination of IpAddr in the Rust Standard Libary 

struct Ipv4Addr {
   //  code snippet of IP-V4
}

struct Ipv6Addr {
   //  code snippet of IP-V6
}

enum IpAddr {
    V4(Ipv4Addr),
    V6(Ipv4Addr),
}

******************************************************************************************************************************************************
// 因此, 可以在 enum 的各个可能性中 嵌入任何类型的 数据类型
// 甚至可以在 enum A 中， 嵌入 enum B 类型的数据





******************************************************************************************************************************************************

*/


#[derive(Debug)]
enum IpAddr1 {
    V4(String),
    V6(String)
}

#[derive(Debug)]
enum IpAddr2 {
    V4(u8, u8, u8, u8),    // e.g.  let addr_v4: IpAddr_2 = IpAddr_2::V4(192.168.1.4);
    V6(String),            //       let addr_v6: IpAddr_2 = IpAddr_2::V6( String::from("2001:0db8:85a3:0000:0000:8a2e:0370:7334") );
}


fn case1_test_special_form_of_enum()
{
    println!();
    println!("----- Special kind of enum : Example case -----");

    let addr_type1_v4: IpAddr1 = IpAddr1::V4( String::from("192,168,1,4") );
    let addr_type1_v6: IpAddr1 = IpAddr1::V6( String::from("2001:0db8:85a3:0000:0000:8a2e:0370:7334") );
    println!("~~~~~ #1 ~~~~~");
    println!("v4 = {:?}", addr_type1_v4);
    println!("v6 = {:?}", addr_type1_v6);
    println!();


    let addr_type2_v4: IpAddr2 = IpAddr2::V4(192,168,1,4);
    let addr_type2_v6: IpAddr2 = IpAddr2::V6( String::from("2001:0db8:85a3:0000:0000:8a2e:0370:7334") );
    println!("~~~~~ #2 ~~~~~");
    println!("v4 = {:?}", addr_type2_v4);
    println!("v6 = {:?}", addr_type2_v6);
}


#[derive(Debug)]
enum Message {
    Quit,                         // default type is i32
    Move { x: i32, y: i32 },      // is a anonymous struct with 2 field whose names are 'x' (i32) and 'y' (i32)
    Write(String),                // single type with String 
    ChangeColor(i32, i32, i32),   // a tuple with 3 fileds
}

impl Message {
    // Apply some certain member functions
    fn response_message(&self) {
    }
}


fn case2_test_special_form_of_enum()
{
    println!();
    println!("----- #2 -----   Special kind of enum : Example case -----");

    //                Quit the Program ?
    let m1: Message = Message::Quit;
    //                                move to the destination position (12, 24)
    let m2: Message = Message::Move  { x:12, y:24 };
    //                                Write some text onto the screen
    let m3: Message = Message::Write ( String::from("Hello World ") );
    //                                change the text's color 
    let m4: Message = Message::ChangeColor(255, 100, 30); // RGB color
    
    // let xval: i32 = (Option::<Message::Move>(m2)).x;
    // let yval: i32 = (Option::<Message::Move>(m2)).y;
    // let yval: i32 = (Some(m2) as Message::Move).y;

    println!("m1 = {:?}", m1);
    println!("m2 = {:?}", m2);
    // println!("m2.detail = {{ x: {}, y:{} }}", xval, yval); 
    println!("m3 = {:?}", m3);
    println!("m4 = {:?}", m4);

    m2.response_message();
}

/*

# Option 枚举
    - 定义于标准库中
    - 在 Prelude ( 预导入模块 ) 中
    - 它描述了 : 某个值 ( 以某种数据类型的样式 存在)  它可能存在  , 或不存在的情况  



# Rust **[没有]** Null 这个概念

- Null 引用      : Billion Dollar Mistake ( 数十亿 美元 的设计错误 )
- Null的问题在于 : 当你尝试 像使用非Null值那样 使用Null值的时候，就会引起某种错误

String str_hello = null;
String str_world = " World"
String hello_world = str_hello + str_world; // Run-time Error


//
// The defination of Option
//
Option<T>
enum Option<T> {
    Some(T),
    None,
}


*/

#[warn(unused_variables)]
fn test_option_case1() 
{
    println!();
    println!("----- Option<T> enum Sample Code 1 -----");
    let some_number = Some(5);              // Option<i32>
    let some_string = Some("A String");     // Option<&str>

    // Core Core Core : Type   'Option<i32>' is a must  
    //    Otherwise the Rust compiler cannot deduce what is the type specified in Option<T>   ( Because 'None' is a possibility  enumeration of Option<T> )  
    let absent_number: Option<i32> = None;  
    println!("some_number = {:?}", some_number);
    println!("some_string = {:?}", some_string);
    println!("absent_number = {:?}", absent_number);
    
}


fn test_option_case2() 
{
    println!();
    println!("----- Option<T> enum Sample Code 2-----");
    let a: i32 = 5;
    let b  =  Some(6);
    let c: i32 = a + b.unwrap();
    println!("c = {}", c);
}

/***************************************************
Keyword 'match'     pairing
强大的控制流运算符 - match
- 允许一个值与一系列模式匹配， 并执行匹配的模式对应的代码
- 模式可以是 字面值、 变量名、 通配符 , ...
- match 匹配必须穷举所有的可能 ( or use      '_'  通配符 ) 否则编译器报错

****************************************************/

enum USACoin {
    Penny,    // 1  便士
    Nickel,   // 5  美分
    Dime,     // 10 美分
    Quarter,  // 25 美分
}


fn value_in_cents(coin: USACoin) -> u8 {
    match coin {
        /********************************************************************
           if the code is more than 1 line , Please use  { 
                 ...
           }


           e.g. 

           USACoin::Penny => {   
              println!("match Penny");
              1 
           }
        *********************************************************************/
        //
        // Core Core Core : Rust will follow the order by your code 
        //
        USACoin::Penny     =>  1,
        USACoin::Nickel    =>  5,
        USACoin::Dime      =>  10,
        USACoin::Quarter   =>  25
    }
}





fn test_match_expression()
{
    println!();
    println!("----- test KeyWord 'match' -----");
    let num1: u8 = value_in_cents(USACoin::Penny);
    let num2: u8 = value_in_cents(USACoin::Nickel);
    let num3: u8 = value_in_cents(USACoin::Dime);
    let num4: u8 = value_in_cents(USACoin::Quarter);
    println!("value of Penny   = {}", num1);
    println!("value of Nickel  = {}", num2);
    println!("value of Dime    = {}", num3);
    println!("value of Quarter = {}", num4);
}

#[derive(Debug)]
enum UsState {
    Alabama,
    Alaska,
}


enum USACoin2 {
    Penny,    // 1  便士
    Nickel,   // 5  美分
    Dime,     // 10 美分
    Quarter(UsState),  // 25 美分
}


fn value_in_cents2(coin: USACoin2) -> u8 {
    match coin {
        //
        // Core Core Core : Rust will follow the order by your code 
        //
        USACoin2::Penny     =>  {
             println!("Penny");
             1
        }, // <== Notes , Here ','    comma is an optional delimiter  : Either { ... }    or  { ... },    is valid
        USACoin2::Nickel    =>  5,
        USACoin2::Dime      =>  10,
        USACoin2::Quarter(state)   =>  {
             println!("State quarter from {:?}", state); 
             25
        }
    }
}

fn test_match_expression2()
{
    println!();
    println!("----- test KeyWord 'match' with binding data -----");
    let c: USACoin2 = USACoin2::Quarter(UsState::Alaska);
    println!("{}", value_in_cents2(c));
}

fn plus_one(x: Option<i32>) -> Option<i32>
{
    match x {
        None => None,
        Some(i) => Some(i+1),
    }
}


fn test_match_expression3()
{
    println!();
    println!("----- test KeyWord 'match' pairing on Option<T> -----");
    let five: Option<i32> = Some(5);
    let six: Option<i32> = plus_one(five);
    let none: Option<i32> = plus_one(None);
    // Core Core Core : use method  unwrap() on enumObject
    println!("six = {}", six.unwrap() );
    // use is_none() on a certain enumObject
    if none.is_none() {
        println!("none = Option<i32>::None" );
    } else {
        println!("none = {}", none.unwrap() );
    }
}



fn test_match_expression4()
{
    println!();
    println!("----- test KeyWord 'match' use '_' wildcard to process some condition not be written in code -----");
    let val: u8 = 123u8;
    match val {
        1 => println!("one"),
        3 => println!("three"),
        5 => println!("five"),
        7 => println!("seven"),
        _ => {
            println!("default match : val = {}",val);
            ()  // return empty tuple
        },
    }
}


/***********************************************************************************

if let
- is a simple version of 'match' block
- 处理只关心一种匹配而忽略其它匹配的情况
- 更少的代码，更少的缩进，更少的模板代码
- 放弃了穷举的可能
- 可以把 if let 看作是 match 的 语法糖


***********************************************************************************/

fn test_if_let()
{
    println!();
    println!("----- test  <if let> expression -----");
    let val1: Option<i32> = Some(3i32);
    let val2: Option<i32> = Some(-123i32);
    if let Some(3) = val1 {
        println!("val#1 is  three");
    }

    if let Some(3) = val2 { 
        println!("val#2 is  three");
    } else {
        println!("in else : val2 = {}", val2.unwrap() );
	}
}

fn main() 
{
    let ipv4: IpAddrType = IpAddrType::V4;
    let ipv6: IpAddrType = IpAddrType::V6;
 
    println!("v4ip = {:?}", ipv4);
    println!("v6ip.value = {}", (ipv6 as i32));  // value = 1

    // TODO : How to solve such kind of compile Error 
    // try_convert_enum_2_int();

    case1_test_special_form_of_enum();

    case2_test_special_form_of_enum();

    test_option_case1();

    test_option_case2();

    test_match_expression();

    test_match_expression2();

    test_match_expression3();
    
    test_match_expression4();

    test_if_let();

}

