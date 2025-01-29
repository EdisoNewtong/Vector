use std::fmt::Display;
use std::fmt::Debug;


// 
// The 'trait' keyword is like the keyword 'Interface' in other programming language
// 
pub trait Summary {
    // 'pub' is not necessary to qualify the function 'summarize'
    // There is no function implementation code of the function 'summarize'
    fn summarize(&self) -> String;

    // default implement
    fn summarize_default(&self) -> String {
        String::from("Internal implmentation by default\nYou can overwrite this fn 'summarize_default' ! ")
    }

    fn summarize_default_with_other_unimplemented_trait(&self) -> String {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Core Core Core :
        //     You can unimplemented function  'summarize' (it will be implmentated by a certain struct who is intrested in this trait
        //
        format!("Internal implmentation by default\nYou can overwrite this fn 'summarize_default' from {} ! ", self.summarize() )
    }
}

// pub struct NewsArticle   who impl   trait  Summary
// pub struct Tweet         who impl   trait  Summary

pub struct NewsArticle {
    pub headline: String,
    pub location: String,
    pub author:   String,
    pub content:  String,
}


//
// grammer :  
//  impl  <trait_name>   for   <some_one_who_implement_the_trai> 
//
impl Summary for NewsArticle {
    fn summarize(&self) -> String {
        // Notes (Core Core Core) : 
        // You can not call the default fn named 'summarize_default' inside the implementation part fn's body
        format!("{}, by {} ({})",  self.headline, self.author, self.location )
    }
}


/********************************************************************
 
//
// Empty body is Valid
//    Because  fn summarize_default() has already been implemented inside trait 'Summary'
//
impl Summary for NewsArticle 
{
   // leave empty
}

***********************************/

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,  // is forward from other original tweet or not ?
}

impl Summary for Tweet {
    fn summarize(&self) -> String {
        format!("{}: {}", self.username, self.content)
    }
}

//
// use trait object as a function's parameter
//    fn function_name( arg_name: impl <trait_name> ) { ... }
pub fn notify1(item: impl Summary) 
{
    println!("Breaking news! {}", item.summarize() );
}

//
// use 
//     'trait bound'   syntax
//
pub fn notify2<T: Summary>(item: T) 
{
    println!("Breaking news! {}", item.summarize() );
}

/*
// The following  syntax is too verbose
pub fn notify3(item1: impl Summary, item2: impl Summary, item3: impl Summary) */
pub fn notify3<T: Summary>(item1: T, item2: T, item3: T) 
{
    println!("Breaking news!\n1. {}\n2. {}\n3. {}", item1.summarize(), item2.summarize(), item3.summarize() );
}



/*
How to write a certain object which impl multi traits ?
don't write ( only 1 impl is enough )
pub fn notify4(item1: impl Summary + impl Display) { ... } */
pub fn notify4(item: impl Summary + Display)
{
    println!("Breaking news! {}", item.summarize() );
}


pub fn notify5<T: Summary + Display>(item: T)
{
    println!("Breaking news! {}", item.summarize() );
}



// use 'where' ( a keyword in Rust ) sub clause
pub fn notify6<T,U,V>(item1: T, item2: U, item3: V) 
     -> i32 
where 
    T: Summary  + Display,
    U: Clone    + Debug,
    V: Display  + Clone
{
    println!("Breaking news! {}", item1.summarize() );
    100
}



pub fn notify_as_return_value_1(con: &str)  -> impl Summary
{
    NewsArticle {
        headline: String::from("Trump win the U.S.A election"),
        location: String::from("New York City"),
        author:   String::from("A.B.C News Company"),
        content:  con.to_string(),
    }
}

/***************************************************************
// Core Core Core : !!!!! [ Runtime 'polymorphic' is not allowed for different objects that impl the same trait ] !!!
error[E0308]: `if` and `else` have incompatible types
   --> src\lib.rs:203:9
    |
195 | /       if i32flag == 1 {
196 | | /         NewsArticle {
197 | | |             headline:  String::from("That's a big day "),
198 | | |             location: String::from("New York City"),
199 | | |             author:   String::from("A.B.C News Company"),
200 | | |             content: String::from("Trump win the U.S.A election"),
201 | | |         }
    | | |_________- expected because of this
202 | |       } else {
203 | | /         Tweet {
204 | | |             username: String::from("horse_ebooks"),
205 | | |             content:  String::from("of course, as you probably already know, people"),
206 | | |             reply: false,
207 | | |             retweet: false,
208 | | |         }
    | | |_________^ expected `NewsArticle`, found `Tweet`
209 | |       }
    | |_______- `if` and `else` have incompatible types
    |
help: you could change the return type to be a boxed trait object
    |
193 | pub fn notify_as_return_value_2(i32flag: i32)  -> Box<dyn Summary>
    |                                                   ~~~~~~~        +
help: if you change the return type to expect trait objects, box the returned expressions
    |
196 ~         Box::new(NewsArticle {
197 |             headline:  String::from("That's a big day "),
  ...
200 |             content: String::from("Trump win the U.S.A election"),
201 ~         })
202 |     } else {
203 ~         Box::new(Tweet {
204 |             username: String::from("horse_ebooks"),
  ...
207 |             retweet: false,
208 ~         })
    |

For more information about this error, try `rustc --explain E0308`.
error: could not compile `trait_2` (lib) due to previous error

***************************************************************/
pub fn notify_as_return_value_2(i32flag: i32)  -> impl Summary
{
    if i32flag == 1 {
        NewsArticle {
            headline:  String::from("That's a big day "),
            location: String::from("New York City"),
            author:   String::from("A.B.C News Company"),
            content: String::from("Trump win the U.S.A election"),
        }
    } else {
        Tweet {
            username: String::from("horse_ebooks"),
            content:  String::from("of course, as you probably already know, people"),
            reply: false,
            retweet: false,
        }
    }
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
fn largest_number_fix<T: PartialOrd + Copy>(list: &[T]) -> T 
{
    let mut l_num = list[0];

    for &item in list.iter() {
        if item > l_num { // std::cmp::PartialOrd
            l_num = item;
        }
    }

    l_num
}

//
// Compile Passed
//
fn get_largest_number_test_1()
{
    let lst1 = vec![34, 50, 25, 100, 65];
    let r1   = largest_number_fix(&lst1);
    println!("The largest number inside list#1 {:?} is : {}", lst1, r1 );

    let char_lst = vec!['y', 'm', 'a', 'q' ];
    let r2       = largest_number_fix(&char_lst);
    println!("The largest number inside list#2 {:?} is : {}", char_lst, r2 );
}



fn largest_string_fix_1<T: PartialOrd + Clone>(list: &[T]) -> T 
{
    let mut l_num = list[0].clone();

    for item in list.iter() {
        if item > &l_num { // std::cmp::PartialOrd
            l_num = item.clone();
        }
    }

    l_num
}

fn largest_string_fix_2<T: PartialOrd + Clone>(list: &[T]) -> &T 
{
    let mut l_num = &list[0];

    for item in list.iter() {
        if item > &l_num { // std::cmp::PartialOrd
            l_num = item;
        }
    }

    l_num
}


fn get_largest_string_test_1()
{
    let lst1 = vec![String::from("hello"), String::from("world") ];
    let r1   = largest_string_fix_1(&lst1);
    println!("The largest number inside list#1 {:?} is : {}", lst1, r1 );

}

fn get_largest_string_test_2()
{
    let lst2 = vec![String::from("hello"), String::from("world") ];
    let r2   = largest_string_fix_2(&lst2);
    println!("The largest number inside string list#2 {:?} is : {}", lst2, r2 );

}









//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Pair<T> {
    x: T,
    y: T,
}


impl<T> Pair<T> {
    // Core Core Core :
    //    any of the type T has a static member function    Pair::new(xVal, yVal) 
    fn new(x: T, y: T) -> Self {
        Self { x, y }
    }
}

impl<T: Display + PartialOrd> Pair<T> {
    // Core Core Core :
    //
    //    Only if T who implement both Display and PartialOrd   trait
    // Can call fn cmp_display(...)
    //
    fn cmp_display(&self) {
        if self.x >= self.y {
            println!("The largest member is x = {}", self.x);
        } else {
            println!("The largest member is y = {}", self.y);
        }
    }
}


/*
    T 类型它已经实现了 fmt::Display 这个 trait,
现在可以使 T 类型，再添加一个 名为 ToString 的 trait

Sample code from     rust's   std::library

pub trait ToString {
    fn to_string(&self) -> String;
}


impl<T: fmt::Display> ToString for T {
    fn to_string(&self) -> String {
        ...
    }
}


e.g.

fn main()
{
   //
   //  because i32 has implemented  Display   and     ToString
   //
   let str_number = 3.to_string();
}

*/


