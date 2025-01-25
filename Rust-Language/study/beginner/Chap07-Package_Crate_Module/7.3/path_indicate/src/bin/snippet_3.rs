////////////////////////////////////////////////////
//
//     src : snippet_3.rs
//
////////////////////////////////////////////////////


mod back_of_house {
    #[derive(Debug)]
    pub enum Appetizer {
        Soup, // Notes : the enum item is public by default
        Salad,
    }
}


fn main()
{
    let a: back_of_house::Appetizer = back_of_house::Appetizer::Soup;
    let b: back_of_house::Appetizer = back_of_house::Appetizer::Salad;
    println!("Today's appetizer is {:?}", a);
    println!("Tommorrow's appetizer is {:?}", b);
    
}



