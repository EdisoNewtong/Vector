pub mod front_of_house {
    pub mod hosting { // the access permission of mod 'hosting' is private by default
		// the access permission of all the function defined inside a mod  is private by default
        pub fn add_to_waitlist() { println!("add_to_waitlist"); }  // 点入等候队列
        // fn seat_at_table()   { println!("seat_at_table");   }  // 入座
    }

    pub mod serving {  // the access permission of mod 'serving' is private by default
		// the access permission of all the function defined inside a mod  is private by default
        pub fn take_order() { println!("take_order"); }    // 点餐
        // fn server_order() { println!("server_order"); }  // 做餐
        // fn take_payment() { println!("take_payment"); }  // 埋单
    }
}

// Core Core Core :
//----------------------------------------------------------------------------------------------------------------------------
//    It's visible for the function named 'add_to_waitlist' defined inside module front_of_house/hosting  
//  because the callee 'add_to_waitlist' and the caller 'eat_at_restaurant' are at the same file scope 
//----------------------------------------------------------------------------------------------------------------------------
pub fn eat_at_restaurant()
{
	// abs path 
    // Core : the module named 'front_of_house' and the function named 'add_to_waitlist' must be qualified by the key word 'pub' ( public )
    //    otherwise the compiler will raise an error for the unavailabled access permission 
	crate::front_of_house::hosting::add_to_waitlist();
	// relative path
	       front_of_house::hosting::add_to_waitlist();
}


pub fn take_order_at_restaurant()
{
	// abs path 
	crate::front_of_house::serving::take_order();
	// relative path
	       front_of_house::serving::take_order();
}


