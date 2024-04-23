

fn change_value(a:  &mut i32)
{
    *a = 345;
}



fn unchange_string(str : &String)
{
    println!("1.   s = \"{}\"", str);
}


fn change_string(str : &mut String)
{
	// change the data from "Hello" -->  "Hello World"
	*str = String::from("Hello World");
    println!("2.   s = \"{}\"", str);
}



fn main() {
    // println!("Hello, world!");
    // let x = String::from("abc");

	//
	// Change Basic Type
	//
	let mut x = 3; 
	println!("#0. x = {}",x);

	change_value(&mut x); 
	println!("#1. x = {}",x);

	println!("--------------------------------------------------");

	//
	// Change Referenced Type
	//
	let mut s : String = String::from("Hello");
	println!("0.   s = \"{}\"", s);
	println!("0x2  s = \"{}\"", s);
	unchange_string(&s);
	change_string(&mut s);

    println!("3.   s = \"{}\"", s);



}