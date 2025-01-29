use std::fmt::Display;

struct ImportantExcept<'a> {
    part: &'a str,
}

impl<'a> ImportantExcept<'a> {
    fn level(&self) -> i32 {
        3
    }

    // Assume that 
    // &self's lifetime is      'a   ,               &'a str  ->  &'a str   #  use the Rule#3 in Chap 10.7
    fn announce_and_return_part(&self, announcement: &str)    ->  &str {
        println!("Attention please: {}", announcement);
        self.part 
    }
}

fn test_struct_lifetime()
{
    let novel = String::from("Call me Ishmael. Some years ago ... ");

    let first_sentence = novel.split('.')
        .next()
        .expect("Could not found a '.'");

    let i = ImportantExcept {  // it will live from                      [86,89)
        // it refers to variable first_sentence ( whose lifetime is from [82,89)   )  
        //     => 'first_sentence' lives longer than 'i'  => Compile PASSED
        part: first_sentence, 
    };
}

fn static_life_time()
{
    // any string literal is a kind of  'static   lifetime
    let s:&'static str = "hello World";
}


fn longest_with_announcement<'a, T>
   (x: &'a str, y:&'a str, ann: T) -> &'a str
where
    T: Display,
{
    println!("Announcement! {}", ann);
    if x.len() > y.len() {
        x
    } else {
        y
    }


}

fn main() 
{
    test_struct_lifetime();

    println!();
    static_life_time();

    let s1 = String::from("abcd");
    let s2 = String::from("xyz");
    let msg = String::from("Hello everyone :)");
    longest_with_announcement(s1.as_str(), s2.as_str(), msg);

}




