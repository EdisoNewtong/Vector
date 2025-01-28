
fn panic1()
{
    println!("----- test#1 -----");
    panic!("Program crash and burn"); // <== Crash Here by the hard code 

}


fn panic2()
{
    println!("----- test#1 -----");
    let v: Vec<i32> = vec![1, 2, 3];

    println!("v[99] = {}", v[99]); // <== Crash Here by the runtime unsafed code 

}

fn main() 
{
    panic1();

    println!();
    panic2();


}




