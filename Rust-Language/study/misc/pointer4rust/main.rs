// use std::ptr;

/*********************************************************************************************************************************************************

----------------------------------------------------------------------------------------------------
#1
----------------------------------------------------------------------------------------------------
error[E0277]: `*const i32` doesn't implement `std::fmt::Display`
   --> main.rs:126:23
    |
126 |     println!(" pa = {}", pa);   // ----> HERE <---- COMPILE ERROR  , FIX :    println!(" pa = {:p}", pa);
    |                          ^^ `*const i32` cannot be formatted with the default formatter
    |
    = help: the trait `std::fmt::Display` is not implemented for `*const i32`
    = note: in format strings you may be able to use `{:?}` (or {:#?} for pretty-print) instead
    = help: the trait `std::fmt::Display` is implemented for `i32`
    = note: this error originates in the macro `$crate::format_args_nl` which comes from the expansion of the macro `println` (in Nightly builds, run with -Z macro-backtrace for more info)

error: aborting due to previous error

For more information about this error, try `rustc --explain E0277`.

----------------------------------------------------------------------------------------------------
#2
----------------------------------------------------------------------------------------------------
error[E0133]: dereference of raw pointer is unsafe and requires unsafe function or block
   --> main.rs:130:23
    |
130 |     println!(" pa = {}", *pa); // ----> HERE <---- COMPILE ERROR  , FIX :  unsafe { println!(" pa = {}", *pa); }
    |                          ^^^ dereference of raw pointer
    |
    = note: raw pointers may be null, dangling or unaligned; they can violate aliasing rules and cause data races: all of these are undefined behavior
    = note: this error originates in the macro `$crate::format_args_nl` which comes from the expansion of the macro `println` (in Nightly builds, run with -Z macro-backtrace for more info)

error: aborting due to previous error

For more information about this error, try `rustc --explain E0133`.



----------------------------------------------------------------------------------------------------
#3
----------------------------------------------------------------------------------------------------
error[E0594]: cannot assign to `*pa`, which is behind a `*const` pointer
   --> main.rs:146:5
    |
146 |     *pa = 300; // ----> HERE <---- COMPILE ERROR
    |     ^^^^^^^^^ `pa` is a `*const` pointer, so the data it refers to cannot be written
    |
help: consider changing this to be a mutable pointer
    |
139 |     let pa: *const i32      = &mut a;
    |                                +++

error[E0133]: dereference of raw pointer is unsafe and requires unsafe function or block
   --> main.rs:146:5
    |
146 |     *pa = 300; // ----> HERE <---- COMPILE ERROR
    |     ^^^^^^^^^ dereference of raw pointer
    |
    = note: raw pointers may be null, dangling or unaligned; they can violate aliasing rules and cause data races: all of these are undefined behavior

error: aborting due to 2 previous errors

Some errors have detailed explanations: E0133, E0594.
For more information about an error, try `rustc --explain E0133`.

**********************************************************************************************************************************************************/


fn code0()
{
    use             std::ptr;
    let pa: *const i32 = ptr::null();

    if pa.is_null() { 
        println!("in if : pa is null"); // <--- execute this line
    } else {
        println!("in if : pa is not null");
    }

}

fn code01()
{
    use             std::ptr::null;
    let pb: *const i32 = null();

    if pb.is_null() { 
        println!("in if : pb is null"); // <--- execute this line
    } else {
        println!("in if : pb is not null");
    }
}

fn code02()
{
    // use             std::ptr::null;
    let pc: *const i32 = std::ptr::null();

    if pc.is_null() { 
        println!("in if : pc is null"); // <--- execute this line
    } else {
        println!("in if : pc is not null");
    }
}




fn code1()
{
	let a: i32  =  3;
	let pa      = &a;
	println!("  a = {}",  a);
	println!(" pa = {}", pa);
	println!("*pa = {}", *pa);
}

fn code2()
{
	let a:         i32      =  3;
	let pa: *const i32      = &a;
	println!("  a = {}",  a);

	// println!(" pa = {}", pa);   // ----> HERE <---- COMPILE ERROR  , FIX :    println!(" pa = {:p}", pa);
	println!(" pa = {:p}", pa); // Correct
                                

	// println!(" pa = {}", *pa); // ----> HERE <---- COMPILE ERROR  , FIX :  unsafe { println!(" pa = {}", *pa); }
    unsafe {
	    println!(" *pa = {}", *pa);  // Correct
    }
}

fn code3()
{
	let a:         i32      =  3;
	let pa: *const i32      = &a;
	println!("  a = {}",  a);

    unsafe {
	    println!("0.  a = {}",  *pa);
    }

    // *pa = 300; // ----> HERE <---- COMPILE ERROR  
    unsafe {
	    println!("1.  a = {}",  *pa);
    }
}

fn code3_ok()
{
	let mut a:   i32      =  3;
	let pa: *mut i32      = &mut a;
	println!("0.  a = {}",  a);

    unsafe {
	    println!("0.  a = {}",  *pa);
    }

    unsafe {
        *pa = 300;
	    println!("1. *pa = {}",  *pa);
    }
	println!("1.   a = {}",    a);
}




fn main() 
{
    code0();
    println!();

    code01();
    println!();

    code02();
    println!();

    // code03();
    // println!();


    code1();
    println!();

    code2();
    println!();

    code3();
    println!();

    code3_ok();
    println!();

}



