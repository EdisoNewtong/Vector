//
// function's name is my_add
//
//varible a's type is   i32
//                  varible b's type is   i32
//                                             return value's type is i32
fn my_add(a:          i32,   // formal parameter  #1 named  'a'
          b:          i32)   // formal parameter  #2 named  'b'
->                    i32    // return value type
{
	/******************************************************************************************************
        CAUTION : the return statement must be end ** without **   ';' 
	******************************************************************************************************/
    //    this syntax is different from other mainstream programming language 
    a + b
}


fn my_func_with_conditional_return(a: i32, b: i32) -> i32
{
    if a < 0 {
		// if the return statement is not at the tail of the function's body,
		//
		// You *** Must *** use the ** return ** keyword to let the compiler know it's a return statement of the corresponing function
		// like the following

		// Both 
		//            return a + b;
		// And
		//            return a + b
		// Are        Valid for the Rust Compiler 

        return a + b;
        // return a + b   // is an alternative format for the return statement


		/*

				42 | |         a + b
				   | |         ^^^^^ expected `()`, found `i32`
		*/ 
		// let _ = a + b;
    }

    a - b
}



fn fn_a() -> i32 { println!("In fn_a() "); 10 }
fn fn_b() -> i32 { println!("In fn_b() "); 20 } 
fn fn_c() -> i32 { println!("In fn_c() "); 30 }

fn test(a: i32,b: i32, c:i32){ 
    println!("a = {a}"); 
    println!("b = {b}"); 
    println!("c = {c}"); 
}



fn main()
{
	let mut x = 3;
	let y = 5;
	println!("1. {} + {} = {}", x,y, my_add(x,y) );

	x = -3;
	println!("2. {} + {} = {}", x,y, my_func_with_conditional_return(x,y) );

	x = 15;
	println!("3. {} - {} = {}", x,y, my_func_with_conditional_return(x,y) );

    println!();
    //--------------------------------------------------
    // the ***[Correct]*** Rust language Evaluate order is   Pascal Call-Convension
    //
    //     1st : fn_a()
    //     2nd : fn_b()
    //     3rd : fn_c()
    //--------------------------------------------------
    //       rather than  Std-Call
    //
    //  1st : fn_c()
    //  2nd : fn_b()
    //  3rd : fn_a()
    test( fn_a(), fn_b(), fn_c() );

}


