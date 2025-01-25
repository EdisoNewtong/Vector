////////////////////////////////////////////////////
//
//     src : snippet_2.rs
//
////////////////////////////////////////////////////

mod back_of_house {
    /*************************************************** ****************************************************
        Notes :
           if delete the 'pub' keyword in front of 'Breakfast'
           It will be unavailable for the outside source code who want to refer to  'Breakfast'

    ********************************************************************************************************/
    pub struct Breakfast {
        /*************************************************** ****************************************************
           Notes: 
                although the struct is defined as public , the field of the struct is unavailable by default
            unless it's qualified by the keyword 'pub'
        ********************************************************************************************************/
        pub toast:          String,  //  切片面包
            seasonal_fruit: String,  //  季节性水果
    }


    impl Breakfast {
        pub fn summer(toast: &str) -> Breakfast {
            Breakfast {
                toast:  String::from(toast),
                seasonal_fruit: String::from("peach"),
            }
        }
    }
}


pub fn eat_at_restaurant()
{
    let mut meal = back_of_house::Breakfast::summer("Rye"); // 黑麦做的  切片面包
    meal.toast = String::from("Wheat"); // =>  改品种了 =>  小麦做的 切片面包
    println!("I'd like {} toast please", meal.toast);

    // [ERROR] Compile-time 
    // because field 'seasonal_fruit' is unavailable(private) for the outside code
    meal.seasonal_fruit = String::from("Blue Berries"); // 从 桃子 =>  蓝莓
}




fn main()
{
    eat_at_restaurant();
}
