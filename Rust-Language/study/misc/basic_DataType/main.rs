

fn test_int()
{
    /*
     
           | Length  |	Signed | Unsigned
        ---------------------------------
           | 8-bit	 |   i8	   |  u8
           | 16-bit	 |   i16   |  u16
           | 32-bit	 |   i32   |  u32
           | 64-bit	 |   i64   |  u64
           | 128-bit |	i128   |  u128
           | arch	 | isize   |  usize

     */
    let i_num_1: i32 = 10;
    let i_num_2: i32 = 1_0;   // equal to the value 10
    let i_num_3: i32 = 1_00;   // equal to the value 100
    let i_num_4: i32 = 10_0;   // equal to the value 100
    let i_num_5: i32 = 0xFF;
    let i_num_6: i32 = 0b1000; // 0b1000 (in binary represent) = 8(10)
    
    // 0o is a must prefix for an octal number represent
    /***************************************************
       different to The C languaage

    int i_num_7      =   010                             ****************************************************/
    let i_num_7: i32 = 0o10;    // 0o10 (in octal represent) = 8(10)
                                  
    println!("i_num_1 = {i_num_1}");
    println!("i_num_2 = {i_num_2}");
    println!("i_num_3 = {i_num_3}");
    println!("i_num_4 = {i_num_4}");
    println!("i_num_5 = {i_num_5}");
    println!("i_num_6 = {i_num_6}");
    println!("i_num_7 = {i_num_7}");
}

fn test_float()
{
    // Compile Error
/*
error: invalid suffix `f` for float literal
  --> main.rs:42:21
   |
42 |     let float_num = 3.14f;
   |                     ^^^^^ invalid suffix `f`
   |
   = help: valid suffixes are `f32` and `f64`

error: aborting due to previous error
*/

    //let float_num   = 3.14f;
// float float_num  = 3.14f;    // In C Language , it's correct !


	let float_num1: f32   = 3.14;
	let float_num2        = 3.14 as f32;

    let double_num1: f64  = 6.28;
    let double_num2       = 6.28;

    println!("float_num1 = {float_num1}"); 
    println!("float_num2 = {float_num2}"); 
    println!("double_num1 = {double_num1}"); 
    println!("double_num2 = {double_num2}"); 


}

fn test_string()
{
    // s1 is a type of    &str  <==> const char*
    // s1's content is invariable forever
    // But s1 can pointer to other fixed literal string later
    let mut s1 = "ABC";
    let mut s2: String = String::from("DEF");
    println!("0. s1 = \"{s1}\"");
    println!("0. s2 = \"{s2}\"");

    // But s1 can pointer to other fixed literal string in the future
    s1 = "Hello";

    /*
     
        error[E0599]: no method named `push_str` found for reference `&str` in the current scope
          --> main.rs:40:8
           |
        40 |     s1.push_str(" World");
           |        ^^^^^^^^ method not found in `&str`

        error: aborting due to previous error

        For more information about this error, try `rustc --explain E0599`.

    // The following statement will raise a compiler error */
    /* s1.push_str(" World"); */

    println!("1. s1 = \"{s1}\"");
    s2.push_str(" * 2 = DEFDEF");
    println!("1. s2 = \"{s2}\"");

}

fn main() 
{
    test_int();
    println!();

    test_float();
    println!();

    test_string();

}
