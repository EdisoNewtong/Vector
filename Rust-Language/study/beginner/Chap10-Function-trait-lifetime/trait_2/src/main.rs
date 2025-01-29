// Core Core Core : You can't comment the following line
// otherwise  [ERROR] @ Compile time

/***************************************************************************************
 
No method named 'summarize' found for struct 'trait_1::Tweet' in the current scope 

***************************************************************************************/
use trait_2::Summary;

use trait_2::Tweet;

fn main()
{
    let tweet_obj = Tweet {
        username: String::from("horse_ebooks"),
        content:  String::from("of course, as you probably already know, people"),
        reply: false,
        retweet: false,
    };

    println!("1 new tweet : {}", tweet_obj.summarize() );
}



