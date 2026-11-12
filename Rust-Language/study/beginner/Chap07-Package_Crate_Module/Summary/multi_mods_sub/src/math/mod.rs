//////////////////////////////////////////////////
// src/math/mod.rs
//////////////////////////////////////////////////

pub mod math {
    pub mod math_sub {
        pub fn math_pow(base: i32, exp: i32) -> i32
        {
            let mut cnt: i32 = 0;
            let mut num: i32 = 1;
            let ret: i32 = loop {
                num *= base;
                cnt += 1;

                if cnt == exp {
                    break num; 
                }
            };

            ret
        }


        pub fn math_half(num: f64) -> f64
        {
            num / 3.14
        }

    } // end mod math_sub
} // end mod math


