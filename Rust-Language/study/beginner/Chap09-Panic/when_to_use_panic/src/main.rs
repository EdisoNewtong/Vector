use std::net::IpAddr;

/*************************************************

如果你写的代码中，你认为, 你自己可以替代 调用你代码的人 来决定，某些情况是不可恢复时
即 某些错误是 [不可恢复] 时，你就可以使用 panic


如果你写的代码 ( Callee ) ，返回Result<T,E> 
此时，你就将错误的处理权 ，交给了 你的代码的调用者 ( Caller )
调用者，可以根据实际情况，来决定 是否要 恢复这个错误，当然也可以立即 panic

*************************************************/

fn test_1()
{
    println!("---------- in test_1() ----------");
    // the str "127.0.0.1"  must be a valid ip address
    let home: IpAddr = "127.0.0.1".parse().unwrap();
}


pub struct Guess {
    // the field 'value' is a private field
    value: i32,
}

impl Guess {
    pub fn new(value: i32) -> Guess {
        if value < 1 || value > 100 {
            panic!("Guess value must be between 1 and 100, got {}", value);
        }

        Guess { value }
    }

    // like the getter ( because the value inside the struct Guess is a private field of outside code
    pub fn value(&self) -> i32 {
        self.value
    }
}


fn test_2()
{
    println!("---------- in test_2() ----------");
    loop {
        let guess = "32";
        let guess: i32 = match guess.trim().parse() {
            Ok(num) => num,
            Err(_) => continue,
        };

        // if the input number can pass the validator , it's a number between 1~100
        // otherwise   do panic to alert the user that the number he/she input is an invalid number
        let guess = Guess::new(guess);
        // exit the program with  Ctrl+C
    }
}


fn main() 
{
    test_1();

    println!();
    test_2();
}



