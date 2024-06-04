
fn test_turple()
{
	// Compile deduce the tup1's type is (i32, f64, u8)
	let tup1                  = (500, 6.4, 1);
	let tup2 : (i32, f64, u8) = (500, 6.4, 1);
	println!("tup1.0 = {}", tup1.0);
	println!("tup2.1 = {}", tup2.1);

	//
	// Struct unbind to each varible e1, e2, e3
	//
	// e1 is a type if i32
	// e2 is a type if f64
	// e3 is a type if u8
	let (e1, e2, e3) = tup1;
	println!("e1 = {e1}, e2 = {e2}, e3 = {e3}");



	let v1 = tup1.0;
	let v2 = tup1.1;
	let v3 = tup1.2;
	println!("v1 = {v1}, v2 = {v2}, v3 = {v3}");


}

fn test_array()
{
	// the array's dataType is a [i32;10] 
	// an array with 10 elements and each of the element is the i32 dataType
	/* 
	Different from C language :

           C use {          ...         }
        Rust use [          ...         ]

	int a1[]   = { 1,2,3,4,5,6,7,8,9,10 };                            */
	let a1     = [ 1,2,3,4,5,6,7,8,9,10 ];
	let mut idx;
	idx = 0;
	for ele in a1.iter() {
		println!("[{idx}].  {ele}");
		idx = idx + 1;
	}

	println!();
    println!("a1[5] = {}", a1[5]);
	println!();


	// use explicit data type annotation
	let a2: [i32;10] = [ 1,2,3,4,5,6,7,8,9,10 ];
	idx = 0;
	for ele in a2.iter() {
		println!("[{idx}].  {ele}");
		idx = idx + 1;
	}

	println!();
    println!("a2[5] = {}", a2[5]);


	// a3 is a kind of [i32;5] array that contain 5 elements and each element is assignment to value 3 =>   [ 3, 3, 3, 3, 3 ]
	let a3 = [3;5];
	idx = 0;
	for ele in a3.iter() {
		println!("[{idx}].  {ele}");
		idx = idx + 1;
	}

	println!();
    println!("a3[2] = {}", a3[2]);


	// a4 is a kind of [f32;5] array that contain 5 elements and each element is assignment to value 3.14 as float
	/*	=>   

		[ 
		  3.14 as f32,  
		  3.14 as f32, 
		  3.14 as f32, 
		  3.14 as f32, 
		  3.14 as f32 
		]
	*/
	let a4 = [3.14 as f32;5];
	idx = 0;
	for ele in a4.iter() {
		println!("[{idx}].  {ele}");
		idx = idx + 1;
	}

	println!();
    println!("a4[2] = {}", a4[2]);

}


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
    
	// '0'   : first  is number 
	//  'o'  : second is alphabet letter ( small case )
	//
    // 0o is a must prefix for an octal number represent
    /***************************************************
       different to The C language

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
    println!();

    test_array();
    println!();

    test_turple();
    println!();

}

