//
// function's name is my_add
//
//varible a's type is   i32
//                  varible b's type is   i32
//                                             return value's type is i32
fn my_add(a:           i32, b:            i32) ->                     i32
{
	/******************************************************************************************************
        CAUTION : the return statement must be end without   ';' 
	******************************************************************************************************/
    //    this syntax is different from other mainstream programming language 
    a + b
}



fn main()
{
	let x = 3;
	let y = 5;
	println!("{} + {} = {}", x,y, my_add(x,y) );
}

