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



