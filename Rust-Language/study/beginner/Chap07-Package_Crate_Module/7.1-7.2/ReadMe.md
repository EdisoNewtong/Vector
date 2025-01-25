# The usage of module 

**==7.1 ~ 7.2 shows how to invoke the function at main.rs which is defined in lib.rs==**

**==module==** is a kind of strong encapsulation mechanism 

If there is no 'pub' keyword to qualify the mod or the function , it will invisible the the outside scope. 
You can define a public method to export some one of the feature which can be available for the other source code to be invoke. 

e.g.

```Rust
/***************************************************
It's unavailable of the following three things in the view of other source code
1. mod front_of_house 
2. mod hosting
3. function add_to_waitlist

Forturnately you can define a function to export some of the feature like function 'eat_at_restaurant'

****************************************************/

mod front_of_house {
    mod hosting {
        fn add_to_waitlist();
        fn cook_for_the_order();
    }
}


//
// It only export the feature add_to_waitlist() with out cook_for_the_order()
//
pub fn eat_at_restaurant()
{
	// Version #1 : absolute path
	crate::front_of_house::hosting::add_to_waitlist();

	// Version #2 : relative path
           front_of_house::hosting::add_to_waitlist();
}


/******************************************************************************************

Verson #1 or #2 ?

It depends on the way of refactor  :
- If you will move both the mod and the funtion call ,   #2 is a better choice because of #1 is too verbose
- If you will move only the mod part                 ,   #1 is a because choice because absolute path will keep valid no matter whereever the mod moved to

******************************************************************************************/


```

