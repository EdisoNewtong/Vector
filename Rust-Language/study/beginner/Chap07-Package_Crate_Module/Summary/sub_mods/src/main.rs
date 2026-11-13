//////////////////////////////////////////////////
// main.rs
//////////////////////////////////////////////////
mod garden;

fn t1() 
{
    println!("---------- t1() Begin ----------");
    garden::f1();
    garden::vegetable::f1();
    println!("---------- t1() End ----------");
}

fn t2() 
{
    println!("---------- t2() Begin ----------");
    use crate::garden::f1;  // uncomment this line will raise a compiler warning :     the item `garden` is already defined here  ( @line 2 )
    f1();
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    use crate::garden::vegetable::f1 as f1_1;
    f1_1();
    println!("---------- t2() End ----------");
}

fn main() {
    t1();
    println!();
    t2();
}

