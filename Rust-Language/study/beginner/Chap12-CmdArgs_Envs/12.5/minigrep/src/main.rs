// use std::env::args(); // env::args().collect()  -> vec![]
use minigrep::Config;
use std::env;
use std::process;

/******************************************************************
I:
程序代码块的功能划分不够细致
它既负责命令行的参数解析，又负责读取文件，未来还要负责文件内容中的字符串匹配
多个功能[没有]拆分成多个函数




II:

1. query_str 与 
2. filename 都是程序需要完成功能之前的一些配置参数，

而这些参数，是从 用户的命令行终端的 键盘输入中获取的
他俩是一组关联的数据集合，因此捆绑放在一起，会显示更加清晰，更容易使代码易读、理解


III:
在读取文件发生错误的情况下，只会打印一行错误信息
而这个信息又太笼统了，指向性不够明确

IV:
except最好集中放置
最好使得错误处理的代码集中放置
为了便于日后更好，也更方便地维护代码


*******************************************************************/


fn main() {
    let args: Vec<String> = env::args().collect();
    let config = Config::new(&args).unwrap_or_else(|err| {
        println!("Problem parsing arguments: {}", err);
        // the program will exit immediately
        process::exit(1);
    });

    if let Err(e) = minigrep::run(config) {
        println!("Application error: {}", e);
        process::exit(1);
    }
}



