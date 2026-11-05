
type Kilometers = i32;

fn alias_type0()
{
    println!("------- alias_type0() -------");
    let x: i32        = 5;
    let y: Kilometers = 8;
    println!("x + y = {}", x+y); // x + y = 13
}

type Thunk = Box<dyn Fn() + Send + 'static>; 

fn takes_long_type(f: Thunk) {
    // snip
}

fn returns_long_type() -> Thunk {
    // here   || is a closure with no parameter  
    //        | <0 parameter(s)> | => ||
    //                            rather than Logic Or ( e.g.  a<b || c<d )
    Box::new( || println!("hi") )
}

fn alias_type1() {
    println!("------- alias_type1() -------");
    let f: Thunk = Box::new( || println!("hi") );
}

//use std::io::Error;
use std::fmt;
//  Hardcode the 'Error' as  std::io::Error
//type Result<T> = Result<T, std::io::Error>;
type Result<T> = std::io::Result<T>;

pub trait Write {
  //fn write(&mut self, buf: &[u8])              -> Result<usize, Error>;
    fn write(&mut self, buf: &[u8])              -> Result<usize>;
  //fn flush(&mut self)                          -> Result<(), Error>;
    fn flush(&mut self)                          -> Result<()>;

  //fn write_all(&mut self, buf: &[u8])          -> Result<(), Error>;
    fn write_all(&mut self, buf: &[u8])          -> Result<()>;
  //fn write_fmt(&mut self, fmt: fmt::Arguments) -> Result<(), Error>;
    fn write_fmt(&mut self, fmt: fmt::Arguments) -> Result<()>;
}

/***************************************************************************
// type <Never> use the exclamation mark  '!'
//          !  stands for  'void'  in language C/C++
fn bar() -> ! { 
    // if the function body is empty ,
    // The Rust compiler will return an empty tuple is returned finally
    // e.g.
    //       return  ();
    //
    // ()
}
****************************************************************************/


fn alias_type2() {
    println!("------- alias_type2() -------");
    let guess = "";
    loop {
        let guess: u32 = match guess.trim().parse() {
            Ok(num) => num,
            // here   'continue'  stands for the type <Never> ( a.k.a  ! )
            Err(_)  => continue,
        };
        println!("Finally : the guessed number = {}", guess);
    }
}



// impl<T>  for Option<T> {
//     pub fn unwrap(self) -> T {
//         match self {
//             Some(val)  => val,
/************                panic!(...)  return  ->    '!' ( type <Never> )        ************/
//             None       => panic!("called `Option::unwrap()` on a `None` value"),
//         }
//     }
// }

fn never_type0() {
    println!("------- never_type0() -------");
    loop {
        print!("and ever");
    } // the loop block will never stop , so it return   '!' ( type <Never> )
}

// Use Sized constraint
// fn generic<T>(t: T)          { }
// fn generic<T:  Sized>(t: T)  { }
// fn generic<T: ?Sized>(t: &T) { }

fn main() {
    // println!("Hello, world!");
    alias_type0();
    println!("----------------------------------------------------------------------");
    alias_type1();
    println!("----------------------------------------------------------------------");
    // bar();
    println!("----------------------------------------------------------------------");
    alias_type2();
    println!("----------------------------------------------------------------------");
  //never_type0();


}




