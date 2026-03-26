
enum List {
    Cons(i32, Box<List>),
    Nil
}

// in Rust , The type "Rc<T>"   is not a prelude module 
use std::rc::Rc;

enum ListRc {
    Cons(i32, Rc<ListRc>),
    Nil
}



use crate::List::{ Cons, Nil };
use crate::ListRc::Cons as RcCons;
use crate::ListRc::Nil  as RcNil;

fn test_1() 
{
    let a = Cons(5,
               Box::new( Cons(10,
                  Box::new(Nil) ) ) );

    let _b = Cons(3, Box::new(a) );
    /**************************************************
     
       Compile [ERROR]   

       a has been move to varible 'b' already 
       c has no chance to get the ownership of 'a' again.

    ***************************************************/
    // let c = Cons(4, Box::new(a) );
}


fn test_2() 
{
    let a = Rc::new(     RcCons(5, 
                Rc::new( RcCons(10, Rc::new(RcNil)))) );
    // output a.rc = 1 ( 0+1 )
    println!("count after creating a ,   a.rc = {}", Rc::strong_count(&a) );


    //           Rc::clone(&a)   perform a shallow copy rather than deep copy
    //           Rc::clone(&a) only increase the reference counting number +1
    //
    // instead   a.clone()       perform a copy copy 
    let _b = RcCons(3, Rc::clone(&a) );
    // output a.rc = 2 ( 1+1 )
    println!("count after creating b ,   a.rc = {}", Rc::strong_count(&a) );

    {
      let _c = RcCons(4, Rc::clone(&a) );
      // output a.rc = 3 ( 2+1 )
      println!("count after creating c ,   a.rc = {}", Rc::strong_count(&a) );
    }

    // output a.rc = 2 ( 3-1 )
    println!("count after c goes out of scope ,   a.rc = {}", Rc::strong_count(&a) );

    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Key Key Key :
    //   then drop &a inside _b's reference   a.rc = 1 ( 2-1 )
    //   then drop  a itself                  a.rc = 0 ( 1-1 )
    // [Core Core Core] : once a.rc changes to 0 , Rust recycle the real memory of a immediately. 
    // 
    //////////////////////////////////////////////////////////////////////////////////////////////
}


fn main() {
    test_1();

    println!();
    test_2();
}



