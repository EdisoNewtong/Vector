/****************************************************************************************************
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Normally code
use hello_macro::HelloMacro;

struct Pancakes;
struct Pizza;

impl HelloMacro for Pancakes {
    fn hello_macro() { println!("Hello, Macro! My name is Pancakes!"); }
}

impl HelloMacro for Pizza {
    fn hello_macro() { println!("Hello, Macro! My name is Pizza!"); }
}

fn main() {
    Pancakes::hello_macro();
    Pizza::hello_macro();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// To implement the following effect
use hello_macro::HelloMacro;
use hello_macro_derive::HelloMacro;

// mark   #[derive(HelloMacro)]    to get the default implementation of HelloMacro
#[derive(HelloMacro)]
struct Pancakes;

fn main() {
    Pancakes::hello_macro();
}

*****************************************************************************************************/
pub trait HelloMacro {
    fn hello_macro();
}


