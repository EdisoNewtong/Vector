
mod front_of_house {
    pub mod hosting {
        pub fn add_to_waitlist() {
            println!("call function front_of_house::hosting::add_to_waitlist()");
        }


		 pub fn some_fn() {
		 // /*private*/ fn some_fn() {
			println!("call function front_of_house::hosting::some_fn()");
		}
    }
}

/******************************************************************************************************

Import the mod    front_of_house::hosting     into the current scope

    It's similar to the soft symbol-link for a directory

相当于这个 hosting 模块在  crate root 中，能够被用了( 能够直接被访问到 )


*******************************************************************************************************/


/****************************************************************************************************/
//
// #1 Absolute path
//
/****************************************************************************************************/
use crate::front_of_house::hosting;
/*
   like the following code 


   mod hosting {
       pub fn add_to_waitlist() {
	      ...
	   }
   }
*/


//
//  OR USE  
//
/****************************************************************************************************/
//
// #2 Relative path ,   either of them ( not both ) is valid bsolute path
//
/****************************************************************************************************/
// #2 Relative path ,   either of them ( not both ) is valid
// use front_of_house::hosting;






//
//  OR USE  
//
/****************************************************************************************************/
//
// #3  pointer to the  function layer
// But it's not recommended , because it's ambiguous when the source code is too long
//
// You will lose the way that whether the function 'add_to_waitlist' had been imported by 'use' or  from somewhere else
//
// As a convention :  
// - We import the mod just up to parent mod of the function's  in order to make it easy to read the source code
// - struct / enum ,  we import to the path itself
//  
/****************************************************************************************************/

/*
use front_of_house::hosting::add_to_waitlist;

pub fn eat_at_restaurant() {
    add_to_waitlist();
}
*/




pub fn eat_at_restaurant()
{
	hosting::add_to_waitlist();

    // [ERROR] during compiling    for the access permission unreachable 
	hosting::some_fn();
}





use std::collections::HashMap;
fn create_dummy_hashmap()
{
    let mut map = HashMap::new();
	map.insert(1,2);
}



use std::fmt;
use std::io;
fn f1() -> fmt::Result {  }
fn f2() ->  io::Result {  }

/****************************************************************************************************

    The Keyword   'as'

*****************************************************************************************************/

use std::fmt::Result;
use std::io::Result    as IoResult;
fn f3() -> Result { }     // same as std::fmt::Result
fn f4() -> IoResult { }   // same as std::io::Result



