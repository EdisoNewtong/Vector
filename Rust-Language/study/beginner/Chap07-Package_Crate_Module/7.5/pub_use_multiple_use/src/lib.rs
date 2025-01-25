mod front_of_house {
    pub mod hosting {
        pub fn add_to_wait_list() { println!("front_of_house::hosting::add_to_wait_list");  }
    }
}


/*********************************************************************************

   It's only available in the current file scope rather than outside scope

*********************************************************************************/
//           use crate::front_of_house::hosting;


/*********************************************************************************

    it's available outside

*********************************************************************************/
pub       use crate::front_of_house::hosting;


//  by the following use directive
// use crate::front_of_house::hosting;
pub fn eat_at_restaurant()
{
    // hosting::add_to_wait_list();
}

/*********************************************************************************
##############
   e.g #1
##############

use std::cmp::Ordering;
use std::io;

How to make it simple to get rid of multiple std ?
Answer : 
use std::{ cmp::Ordering , io };


##############
   e.g #2
##############

use std::io;
use std::io::Write;


How to make it simple to get rid of multiple std ?
Answer : 
use std::{ self , Write };

############################
   e.g #3 wildcard
############################
// !!! [CAUSION] !!!   
//  You'd better seriously use * , because it export the entire permissions to the outside code
//
// - import *   @ test module
// - be used as   prelude module 
use std::collections::*;


*********************************************************************************/

// use std::{ cmp::Ordering , io };
// use std::{ self , write };
// use std::collections::*;

