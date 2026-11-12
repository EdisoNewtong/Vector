//////////////////////////////////////////////////
//  src/util/mod.rs
//////////////////////////////////////////////////

pub fn my_mathpow(base: i32, exp: i32) -> i32
{
    use crate::math;
    math::math_pow(base,exp)
}

pub fn my_mathhalf(num: f64) -> f64
{
    use crate::math;
    math::math_half(num)
}



