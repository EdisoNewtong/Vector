/**************************************************

error[E0384]: cannot assign twice to immutable variable `num`
  --> main.rs:25:5
   |
23 |     let num = 5;
   |         ---
   |         |
   |         first assignment to `num`
   |         help: consider making this binding mutable: `mut num`
24 |     println!("1st. num = {}", num );
25 |     num = 6;
   |     ^^^^^^^ cannot assign twice to immutable variable

error: aborting due to previous error

For more information about this error, try `rustc --explain E0384`.

**************************************************/

fn variables_is_defaultly_immutable()
{
    let num = 5;
    println!("1st. in variables_is_defaultly_immutable() num = {}", num );
    /* num = 6;
    println!("2nd. num = {}", num );
    */
}

fn variables_define_with_explicit_type()
{
    let num: i32 = 5;
    println!("1st. in variables_define_with_explicit_type() num = {}", num );
}


fn variables_shadowing()
{
    let num = 5;
    println!("1st. in variables_shadowing() num = {}", num );

    // This statement is valid in Rust-Language, re-defined varibles behavior is allowed in Rust-Language  
    //                                  value from the 1st num
    let num = num * 2;  // the 2nd num =                5       * 2 = 10
    println!("2nd. in variables_shadowing() num = {}", num );
}


fn main()
{
    variables_is_defaultly_immutable();

    variables_define_with_explicit_type();

    variables_shadowing();
}




