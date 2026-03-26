// use std::mem::drop;

struct CustomSmartPointer {
    data: String,
}

impl Drop for CustomSmartPointer {
    fn drop(&mut self) {
        println!("Dropping CustomSmartPointer with data '{}'!", self.data );
    }
}


fn main() {
    let _c = CustomSmartPointer { data: String::from("stuff c-1"), };
    // _c.drop();   // !! Compile Time [ERROR] !!

    /****************************************************
    
      Don't worry about   double-free pointer   issue.

         Rust will handle it safely.

    *****************************************************/
    // destructor _c    in advance
    // drop(_c);   // drop(...) is inside prelude , so no need to call   "use   std::mem::drop;"
              
    let _d = CustomSmartPointer { data: String::from("stuff d-2"), };
    let _e = CustomSmartPointer { data: String::from("stuff e-3"), };

    println!("CustomSmartPointer created.");
    // destructor order is reversed   in comparison with the constructor order
    // 1. _e        1st
    // 2. _d        2nd
    // 3. _c       last 
    /******************************************

          Stack

     
        |   _e   |   <-- stack.top
        ---------
        |   _d   |
        ---------
        |   _c   |   <-- stack.bottom
        ---------

     *******************************************/
}



