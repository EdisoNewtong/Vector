use add_one;
use add_two;

fn main() {
    let num = 29;
    println!("Hello World!\n {} + 1 = {} !\n {} + 2 = {} !",
        num,
        add_one::add_one(num),
        num,
        add_two::add_two(num) 
    );
}




