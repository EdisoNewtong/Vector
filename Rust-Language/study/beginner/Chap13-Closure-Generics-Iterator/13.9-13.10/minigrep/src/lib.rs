use std::error::Error;
use std::fs;
use std::env;

pub fn run(config: Config) -> Result<(), Box<dyn Error>> {
    let contents = fs::read_to_string(config.filename)?;
    let results = if config.case_sensitive {
        search(&config.query, &contents)
    } else {
        search_case_insensitive(&config.query, &contents)
    };

    for line in results {
        println!("{}", line);
    }
        
    // println!("With text: \n{}", contents);
    Ok(())
}

pub struct Config {
    pub query: String,
    pub filename: String,
    pub case_sensitive: bool,
}

impl Config {
    //                                         错误信息字符串是一个静态的字面量，所有带 'static 生命周期是一个 str
    // pub fn new(args: &[String]) -> Result<Config, &'static str> 
    pub fn new(mut args: std::env::Args) -> Result<Config, &'static str> {
        if args.len() < 3 {
            // panic!("not enough arguments");
            return Err("not enough arguments");
        }
        /**************************************************************************************************** 
           clone() method will raise performance consumption
           How to improve the following code ?
        *****************************************************************************************************/
        // let query = args[1].clone();
        // let filename  = args[2].clone();
        // Core Core Core [skip]!!! the 1st argument whose value is Program's name
        args.next(); 
        let query = match args.next() {
            Some(arg) => arg,
            None      => return Err("Didn't get a query string"),
        };
        let filename  = match args.next() {
            Some(arg) => arg,
            None      => return Err("Didn't get a file name"),
        };

        let case_sensitive  = env::var("CASE_INSENSITIVE").is_err();
    
        // return value
        Ok(Config { query, filename, case_sensitive })
    }    
}

/* 
   声明一个生命周期，名为'a
   由于要找的结果是从 contents 这个字符串中截取相匹配的行，所以需要指定生命周期为 'a
   另外函数的返回值vec<&str> 是从 contents 就找到某1行或者某n行的结果，所以与 contents 的生命周期一致 

*/
pub fn search<'a>(query: &str, contents:&'a str) -> Vec<&'a str> {
    /***********************************************************
       用迭代器写法，有以下好处
       1. 代码量更加短，更易读
       2. 没有额外的临时的容器变量  results ( Vec<&str> )
       3. 消除 results 的可变修饰符，使得 未来多线程 编程过程中，不用对变量进行上锁、解锁等同步操作 
          ( 使得 并发 更加容易实现 )

      使用迭代器的好处，是为了
    让 开发者更加 专注于高层的业务逻辑
    而不必陷入 写循环 ，维护临时变量的状态，这些细节工作 
      使用迭代器，能使重点的逻辑，更加突出 

    ***********************************************************/

    /************************************************************
    let mut results = Vec::new();
    for line in contents.lines() {
        if line.contains(query) {
            results.push(line);
        }
    }

    results
    ************************************************************/

    contents.lines()
            .filter(|line| line.contains(query)  )
            .collect()
}

pub fn search_case_insensitive<'a>(query: &str, contents:&'a str) -> Vec<&'a str> {
    /**********************************************************
    let mut results = Vec::new();
    let query = query.to_lowercase();
    for line in contents.lines() {
        if line.to_lowercase().contains(&query) {
            results.push(line);
        }
    }

    results
    **********************************************************/

    contents.lines()
            .filter(|line| line.to_lowercase().contains( &query.to_lowercase() )  )
            .collect()
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn case_sensitive() {
        let query   = "duct";
        let contents = "\
Rust:
safe, fast, productive.
Pick three
Duct tape.";


        assert_eq!(vec!["safe, fast, productive."], search(query, contents) )
    }

    #[test]
    fn case_insensitive() {
        let query   = "ruST";
        let contents = "\
Rust:
safe, fast, productive.
Pick three.
Trust me.";

        assert_eq!(vec!["Rust:", "Trust me."], search_case_insensitive(query, contents) )
    }
}

