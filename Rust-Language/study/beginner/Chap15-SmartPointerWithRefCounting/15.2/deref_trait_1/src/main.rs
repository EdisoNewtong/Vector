use std::ops::Deref;

// a tuple struct with only one anonymous filed whose type is generic T
struct MyBox<T>(T);

impl<T> MyBox<T> {
    fn new(x: T) -> MyBox<T> {
        MyBox(x)
    }

}

// implement  Deref trait for  MyBox<T> /*
impl<T> Deref for MyBox<T> {
    // associated type is T
    type Target = T;

    fn deref(&self) -> &T {
        &self.0    // return the 1st element's value (in this example : it's i32)
    }
}

fn test_box()
{
    println!("----- test_box Begin -----");
    let x = 5;
    let y = MyBox::new(x);

    println!("1.   x = {}",  x);
    // Compile ERROR if comment line 13~21
    //   Also see line 13~21 
    // type `MyBox<{integer}>` cannot be dereference
    //    because it hasn't implemented 'defef()' trait 
    println!("1.  *y = {}", *y);  // *y   <==>       *( y.deref() )

    println!("----- test_box End -----");
}

fn test_1()
{
    println!("----- test_1 Begin -----");
    let x = 5;
    let y = &x;

    assert_eq!(5, x);
    assert_eq!(5, *y);
    println!("----- test_1 End -----");
}


fn test_2()
{
    let mut x = 5;
    //  !!! Notes !!!
    //            Here    x    is passed as a value form  rather than reference form
    let     y = Box::new( x );

    println!("1.  x = {}", x);
    println!("1.  y = {}", y);
    println!("1. *y = {}", *y);

    x = 11;
    println!("Changing x from 5 to 11");

    println!("2.  x = {}\n",  x);  // x has been changed to    11
    println!("2.  y = {}",  y);  // y keeps still 5 
    println!("2. *y = {}", *y);  // y keeps still 5 
    println!("2.  y.deref()   = {}",   y.deref());    // require  the statement : use std::ops::Deref;
    println!("2. *y.deref()   = {}",  *y.deref());  // require  the statement : use std::ops::Deref;
    println!("2. *(y.deref()) = {}", *(y.deref()));  // require  the statement : use std::ops::Deref;

    println!("----- test_2 End -----");
}



fn main() {
    test_1();
    println!("\n--------------------------------------------------\n");
    test_2();
    println!();
    test_box();

}



