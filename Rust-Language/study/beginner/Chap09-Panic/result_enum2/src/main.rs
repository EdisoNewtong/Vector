use std::fs::File;
use std::io;
use std::io::Read; // read_to_string is wrapped inside this crate
                   



fn read_username_from_file_1() -> Result<String, io::Error> 
{
    println!("----- in fn read_username_from_file_1() -----");
    let f = File::open("hello.txt");

    let mut f = match f {
        Ok(file) => file,
        Err(e)   => return Err(e),   // use the 'return' keyword to return from this function
    };

    let mut s: String = String::new();
    match f.read_to_string(&mut s) {
                        Ok(_)  => Ok(s),
        // 1st error type : 
// core::result::Result::Err
//2nd error type: std::io::Error::Err
                        Err(e) => Err(e),
    }
}



// the '?' (shortcut) operator
fn read_username_from_file_2() -> Result<String, io::Error> 
{
    println!("----- in fn read_username_from_file_2() -----");
    let mut f = File::open("hello.txt")?;
    /**************************************************
     ***** Same with the previous code *****
     **************************************************
    
    let mut f = match File::open("hello.txt") {
        Ok(file) => file,
        Err(e) => return Err(e),
    };

    ***************************************************/

    let mut s = String::new();
    f.read_to_string(&mut s)?;
    Ok(s)
}

/**************************************************
 * extra knowledge from     '?' expression : ( ? with   fn  'from' )
 * if i want to convert Error-A  => Error-B
 * It's unpossible that not each Error-A can be convert from Error-B
 *

####################################################################################################
 //
 // It's the Error-A that must implement a function named 'from()' and it's return value is Error-B
 // e.g.
 
 struct Error-A { ... }
 
 impl Error-A   {
     fn from(&self) -> Error-B {
         ...
     }
 }

####################################################################################################

**************************************************/

// The code is easier and shorter than    fn  read_username_from_file_2() { ... }
fn read_username_from_file_3() -> Result<String, io::Error> 
{
    println!("----- in fn read_username_from_file_3() -----");

    // it's same as   read_username_from_file_2() but to short
    let mut s = String::new();
    File::open("hello.txt")?.read_to_string(&mut s)?;
    Ok(s)

}

fn main() 
{
    let res1 = read_username_from_file_1();

    println!();
    let res2 = read_username_from_file_2();

    println!();
    let res3 = read_username_from_file_3();
}

/***************************************************
 
  the '?' operator can return only the type is Result<T, E> or Option<T,E> 
otherwise error message like the following :   

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    cannot use the `?` operator in a function that returns `()` 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

But fn main()  return void ( an empty tuple like this ->  () ) by default


the following code can pass the compiler.

***************************************************/

//--------------------------------------------------
// use std::error::Error as stdErr; // for fn main ... Box<dyn stdErr> 
//--------------------------------------------------
// fn main() -> Result<(), Box<dyn stdErr>> {
//     let f = File::open("hello.txt")?;
//     Ok(())
// }


