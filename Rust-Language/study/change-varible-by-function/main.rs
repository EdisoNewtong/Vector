

fn change_value(a:  &mut i32)
{
    *a = 345;
}

fn main() {
    println!("Hello, world!");
    // let x = String::from("abc");

	let mut x = 3; 
	println!("#0. x = {}",x);

	change_value(&mut x); 
	println!("#1. x = {}",x);
}
