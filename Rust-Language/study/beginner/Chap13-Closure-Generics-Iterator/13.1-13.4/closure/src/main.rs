/************************************************************

函数式 编程思想

1. 把函数作为另一个函数的参数，进行传递
2. 把函数作为另一个函数的 返回值
3. 把函数的实现 赋值给一个变量，以备后用

闭包，有点像 C++ 中的匿名函数 ， 也可以被赋值给一个变量进行保存 
同时，闭包，还可以访问 在它之前定义的 变量( 捕获其有效访问权限之内的 任何变量 )
但是，函数却没有闭包的这个能力，因为它需要在公共的区域被调用，需要明确的传入参数的类型


*************************************************************/
use std::thread;
use std::time::Duration;




fn main() {
    let simulated_user_specified_value = 10;
    let simulated_random_number = 7;

    generate_workout( simulated_user_specified_value, simulated_random_number);
}


fn generate_workout(intensity: u32, random_number: u32) {
    let expensive_closure = |num| {
        println!("calculating slowly...");
        thread::sleep( Duration::from_secs(2) ); // sleep for 2 seconds
        num
    };

    if intensity < 25 {
        println!("Today, do {} pushups!", expensive_closure(intensity) );
        println!( "Next, do {} situps!", expensive_closure(intensity) );
    } else {
        if random_number == 3 {
            // ?????????????????????????????????????????????????????????????????????????????
            // Notes: 
            //    How to <Avoid> the calling simulated_expensive_calculation(...) !!! !!!
            //    while intensity>=25 && random_number==3 ???
            //
            // ?????????????????????????????????????????????????????????????????????????????
            println!("Take a break today! Remember to stay hydrated!");
        } else {
            println!( "Today, run for {} minutes!", expensive_closure(intensity) );
        }
    }

}







