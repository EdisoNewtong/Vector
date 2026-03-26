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

fn hello(name: &str) {
    println!("Hello, {}", name);
}




fn main() {
    hello("Rust 1.");
    println!();

    // m's type   is   MyBox<String>
    let m = MyBox::new(  String::from("Rust 2.") );
    println!("let m = MyBox::new(  String::from(\"Rust 2.\"); ");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 0. &m     &MyBox<String>
    // 1. (&m).deref()    =>    &String
    // 2. (&String).deref()     &str
    // ----------------------------------------------------------------------------
    //             0.  &m     &MyBox<String>
    //                        1.  (&m).deref()    =>    &String             
    //                              2. (&String).deref()     &str            
    //  hello( ( (&m).deref() ).deref());
    //
    // ----------------------------------------------------------------------------
    //
    // <Impliedly> type conversion  :    Deref Coercion (隐式 解引用转化 )
    println!("pass  &m");
    hello(&m); // Compile Passed
    println!();
               
    println!("pass  &m.deref()");
    hello(   &m.deref()  ); // Compile Passed
    println!();

    println!("pass  (&m).deref()");
    hello( (&m).deref()  ); // Compile Passed
    println!();
    println!("pass  &(m.deref())");
    hello(  &(m.deref()) ); // Compile Passed
    println!();

    println!("pass  ((&m).deref()).deref()");
    hello(   ( (&m).deref() ).deref()  ); // Compile Passed
    println!();

    println!("pass  &(((&m).deref()).deref())");
    hello(   &(((&m).deref()).deref())  ); // Compile Passed
    println!();
               
    // => hello(&(*m)[ .. ])
    println!("pass  &(*m)[ .. ]");
    hello(&(*m)[ .. ]   );
    println!();

    println!("pass  &((*m)[ .. ]");
    hello(&((*m)[ .. ]) ); // do (*m)[ .. ] then get the address of  evaluation exp.
    println!();

    // Compile [ERROR]
    // hello((&(*m))[ .. ]));
}




