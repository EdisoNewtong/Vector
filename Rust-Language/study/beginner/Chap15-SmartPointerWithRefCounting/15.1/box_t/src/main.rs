
// Chain List inside C
enum List {
    // Cons is a name of self-defined struct 
    //    who has 2 fields (a tuple with 2 ) : 
    // - The 1st field  is an i32  type
    // - The 2nd field  is an List type
    //    !!! Compile ERROR !!!
    // The Rust compiler must be able to calculate the fixed size of a data type restored in the stack
    //  List inside list   --> inside list  -->  inside list  -->   ... 
    //     !!! Infinate loop !!!
    // Cons(i32, List),   
    Cons(i32, Box<List>),   
    Nil
}



fn main() {
    let b = Box::new(5);
    println!("1.  b = {}",  b); // no need  *b
    println!("2. *b = {}", *b); // *b is OK

    if *b >=4 { // '*' is a must when use getter , otherwisw Compile [ERROR] !
        println!("in if   : *b >= 4"); 
    } else {
        println!("in else : *b <  4"); 
    }


    let _lst1 = List::Cons(1, 
                   Box::new(  List::Cons(2,
                       Box::new( List::Cons(3,
                           Box::new( List::Nil ) )
                               )
                                   )
                            )
                         );


    // easy to read with less expressions 
    use crate::List::{ Cons, Nil };
    let _lst2 = Cons(1, 
                   Box::new(  Cons(2,
                       Box::new( Cons(3,
                           Box::new( Nil ) )
                               )
                                   )
                            )
                         );


}




