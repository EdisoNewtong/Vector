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



fn main()
{
	let mut x = 3;
	let y = 5;
	println!("1. {} + {} = {}", x,y, my_add(x,y) );

	x = -3;
	println!("2. {} + {} = {}", x,y, my_func_with_conditional_return(x,y) );

	x = 15;
	println!("3. {} - {} = {}", x,y, my_func_with_conditional_return(x,y) );
}