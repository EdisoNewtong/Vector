/****************************************************************************************************


warning: unused variable: `a`
  --> main_print1.rs:19:9
   |
19 |     let a : i32 = 456;
   |         ^ help: if this is intentional, prefix it with an underscore: `_a`
   |
   = note: `#[warn(unused_variables)]` on by default


*****************************************************************************************************/

fn main()
{
    let a : i32 = 123;
    println!("1. a = {}", a); // output : a = 123
    let a : i32 = 456;
}




