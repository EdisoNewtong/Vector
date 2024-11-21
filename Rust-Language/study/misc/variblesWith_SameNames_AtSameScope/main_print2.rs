/****************************************************************************************************

warning: unused variable: `a`
  --> main_print2.rs:17:9
   |
17 |     let a : i32 = 123;
   |         ^ help: if this is intentional, prefix it with an underscore: `_a`
   |
   = note: `#[warn(unused_variables)]` on by default



*****************************************************************************************************/

fn main()
{
    let a : i32 = 123;
    let a : i32 = 456;
    println!("2. a = {}", a); // output : a = 456 , the later declaration of 'a' will take effect  ( Rather than the former 'a' )
}




