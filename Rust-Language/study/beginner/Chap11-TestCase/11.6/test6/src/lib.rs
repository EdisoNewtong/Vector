pub fn add(a: i32, b: i32) -> i32 {
    a + b
}

/************************************************************************
 
( 默认为并行执行, 被测试函数可能执行的先后顺序，
  由于某些中间状态或者环境变量问题，导致测试出现非预期的情况
  可能由于被测函数之间存在某些联系，而非相互独立 )

// 多线程执行 
$ cargo test --test-threads=<number> 



************************************************************************/
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        println!("inside function   it_works(...)");
        let result = add(2, 2);
        assert_eq!(result, 4);
    }

    #[test]
    fn it_failed() {
        println!("inside function   it_failed(...)");
        let result = add(2, 2);
        assert_eq!(result, 5);
    }
}
