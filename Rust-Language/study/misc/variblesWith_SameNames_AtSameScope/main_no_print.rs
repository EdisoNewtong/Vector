/****************************************************************************************************

warning: unused variable: `a`
  --> main_no_print.rs:23:9
   |
23 |     let a : i32 = 123;
   |         ^ help: if this is intentional, prefix it with an underscore: `_a`
   |
   = note: `#[warn(unused_variables)]` on by default

warning: unused variable: `a`
  --> main_no_print.rs:24:9
   |
24 |     let a : i32 = 456;
   |         ^ help: if this is intentional, prefix it with an underscore: `_a`

warning: 2 warnings emitted

*****************************************************************************************************/

fn main()
{
    let a : i32 = 123;
    let a : i32 = 456;
}



