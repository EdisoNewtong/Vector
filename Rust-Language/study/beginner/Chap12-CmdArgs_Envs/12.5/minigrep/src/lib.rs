use std::error::Error;
use std::fs;

pub fn run(config: Config) -> Result<(), Box<dyn Error>> {
    let contents = fs::read_to_string(config.filename)?;
        
    println!("With text: \n{}", contents);
    Ok(())
}

pub struct Config {
    pub query: String,
    pub filename: String,
}

impl Config {
    //                                         错误信息字符串是一个静态的字面量，所有带 'static 生命周期是一个 str
    pub fn new(args: &[String]) -> Result<Config, &'static str> {
        if args.len() < 3 {
            // panic!("not enough arguments");
            return Err("not enough arguments");
        }
        let query = args[1].clone();
        let filename  = args[2].clone();
    
        // return value
        Ok(Config { query, filename })
    }    
}


