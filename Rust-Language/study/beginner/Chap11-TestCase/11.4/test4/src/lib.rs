pub struct Guess {
    value: u32
}

impl Guess {
    pub fn new(value: u32) -> Guess {
        if value < 1 || value > 100 {
            panic!("Guess value must be between 1 and 100, But real value got {}", value)
        }

        // else return the valid object
        Guess { value: value }
    }

    pub fn new_in_detail(value: u32) -> Guess {
        if value < 1 {
            panic!("Guess value must be no less than 1, got {}", value)
        } else if value > 100 {
            panic!("Guess value must be no greater than 100, got {}", value)
        }

        // else return the valid object
        Guess { value }
    }

}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    /*****************************************************************
      #[should_panic] 这个修饰符描述如下 : 

       当 test 函数能否捕获到一些 特殊的 panic 错误 ( 无论是传入了错误的参数引起的，还是函数体内部逻辑错误 引起的 )

       如果[能] 捕获到 panic , 那么测试 Passed
                        否则 ,     测试 Failed

    *****************************************************************/
    // #[should_panic] Comment
    // expected a panic occur, 
    //     if a panic occurs , Test Passed
    //     otherwise , Test Failed
    #[should_panic]
    fn greater_than_100() {
        // 200 is not inside range [1,100] 
        //    so the function should_panic (it will enter line:8   panic!(...) )
        //         test PASSED
        Guess::new(200);
    }



    #[test]
    #[should_panic(expected = "Guess value must be no greater than 100")]
    fn greater_than_100_2() {
        // 200 is not inside range [1,100] 
        //    so the function should_panic (it will enter line:8   panic!(...) )
        //         test PASSED
        Guess::new_in_detail(200);
    }


    #[test]
    #[should_panic(expected = "Guess value must be no greater than 100")]
    fn greater_than_100_3() {
        // 200 is not inside range [1,100] 
        //    so the function should_panic (it will enter line:8   panic!(...) )
        //         test PASSED
        Guess::new_in_detail(0);
    }
}


