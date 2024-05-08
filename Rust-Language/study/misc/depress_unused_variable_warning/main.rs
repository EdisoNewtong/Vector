

fn test1()
{
	/****************************************************************************************************

	   Use  underline  +  a small case varible name to depress the unused variable warning raised by the `rustc` compiler 

	     e.g. :
	 
	 
	           let _unused_variable: i32 = 5;
	
	*****************************************************************************************************/

    let _a : i32 = 123;
    let _b : i32 = 456;
}



fn test2()
{
	// it only apply on the variable named   'a'  exclude   'b'
	#[allow(unused)]
    let a : i32 = 123;

	#[allow(unused)]
    let b : i32 = 456;

}


// it apply on the entire function lifecycle until meet the end symbol of the function     }
#[allow(unused)]
fn test3()
{
    let a : i32 = 123;
    let b : i32 = 456;
    let c : i32 = 789;
} // end symbol  of the function test3() during its lifecycle





#[allow(unused)] // to depress the unused warning from the variable named 'a'    by the `rustc`  compiler
fn my_add1(a: i32, b: i32, c: i32) -> i32
{
    b + c
}


//    add '_'  as a prefix for the passed argument to depress the unused warning by the `rustc`  compiler
fn my_add2(_a: i32, b: i32, c: i32) -> i32
{
    b + c
}




fn main() 
{
    test1();
    println!();

    test2();
    println!();

    test3();
    println!();

    let ret1: i32 = my_add1(11, 22, 33);
    println!("ret1 = {ret1}");

    let ret2: i32 = my_add2(11, 22, 33);
    println!("ret2 = {ret2}");

}
