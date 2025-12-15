// use std::env::args(); // collect -> vec![]
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();

    // 如果命令行参数中带有 非法的Unicode 字符时, env::args() 无法处理
    // 因此当接收此类字符时，程序会 panic!(...)
    //  此时需要使用 :   env::args_os()  // OsString
    // println!("{:?}", args);

    let query_str = &args[1];
    let filename = &args[2];
    println!("Search for {}", query_str);
    println!("In file {}", filename);

}


