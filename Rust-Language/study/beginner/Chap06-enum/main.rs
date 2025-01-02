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


fn case2_test_special_form_of_enum()
{
    println!();
    println!("----- #2 -----   Special kind of enum : Example case -----");

    let m1: Message = Message::Quit;
    let m2: Message = Message::Move  { x:12, y:24 };
    let m3: Message = Message::Write ( String::from("Are you sure to quit Word without saving the changed document ? ") );
    let m4: Message = Message::ChangeColor(255, 100, 30); // RGB color
    
    // let xval: i32 = (Option::<Message::Move>(m2)).x;
    // let yval: i32 = (Option::<Message::Move>(m2)).y;
    // let yval: i32 = (Some(m2) as Message::Move).y;

    println!("m1 = {:?}", m1);
    println!("m2 = {:?}", m2);
    // println!("m2.detail = {{ x: {}, y:{} }}", xval, yval); 
    println!("m3 = {:?}", m3);
    println!("m4 = {:?}", m4);
}





fn main() {
    let ipv4: IpAddrType = IpAddrType::V4;
    let ipv6: IpAddrType = IpAddrType::V6;
 
    println!("v4ip = {:?}", ipv4);
    println!("v6ip.value = {}", (ipv6 as i32));  // value = 1

    // TODO : How to solve such kind of compile Error 
    // try_convert_enum_2_int();

    case1_test_special_form_of_enum();
    case2_test_special_form_of_enum();

}



