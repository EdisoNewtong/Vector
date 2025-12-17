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

struct Cacher<T>
    where T: Fn(u32) -> u32 
{
    calculation: T,
    // Why use Option Type wrap
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //
    //    Because the closure can be only defined without any call
    // Before the closure execute , the value is Option<u32>::None
    //
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    value: Option<u32>,
}

impl<T> Cacher<T> 
    where T: Fn(u32) -> u32, 
{
    fn new(calculation: T) -> Cacher<T> {
        Cacher {
            calculation,
            value: None,
        }
    }

    //
    // Why use mut keyword : because the  member data named 'value' will be changed during run-time 
    //
    fn value(&mut self, arg: u32) -> u32 {
        match self.value {
            Some(v) => v,
            // if self.value is None
            // it means the closure haven't been executed yet!
            //   call the closure only once
            None    => {
                let v = (self.calculation)(arg);
                self.value = Some(v);
                v
            }
        }
    }


}


fn main() {
    let simulated_user_specified_value = 10;
    let simulated_random_number = 7;

    generate_workout( simulated_user_specified_value, simulated_random_number);
}

#[test]
fn study_closure_feature() {
    let x = 4;
    //                      x is not in the argument list of closure "equal_to_x"
    //  But the closure can capture the varible in the same scope before its defination 
    let closureequal_to_x = |z| z==x;

/**********************************************************************
error[E0434]: can't capture dynamic environment in a fn item
  --> src\main.rs:99:14
   |
99 |         z == x;
   |              ^
   |
   = help: use the `|| { ... }` closure form instead

error[E0308]: mismatched types
  --> src\main.rs:98:33
   |
98 |     fn fn_equal_to_x(z: i32) -> bool {
   |        -------------            ^^^^ expected `bool`, found `()`
   |        |
   |        implicitly returns `()` as its body has no tail or `return` expression
99 |         z == x;
   |               - help: remove this semicolon to return this value

Some errors have detailed explanations: E0308, E0434.
**********************************************************************/
    // fn fn_equal_to_x(z: i32) -> bool { // [!!!Notes!!!] : a function defined inside another function is allowed by Rust Grammer!
    //     z == x;
    // }/*;  [NO] semicolon at the end of its defination is NOT allowed */

    /****************************************************************************************************
    *****************************************************************************************************/
    let y = 4;
    assert!( closureequal_to_x(y) ); // the test will passed without any error
    // assert!( fn_equal_to_x(y) ); // the test will passed without any error
}


#[test]
fn study_move_keyword() {
    let x = vec![1,2,3];
    let use_move_closure = move |z| z==x;
    //
    // Compile time Error, because the varible x has been moved inside the closure's body ( by keyword 'move' )
    //  x will be drop (dead) after the defination of use_move_closure 
    println!("can't use x here: {:?}", x);

    let y = vec![1,2,3];
    assert!( use_move_closure(y) );
}


fn generate_workout(intensity: u32, random_number: u32) {
    // must add a new keyword  mut before the varible name "expensive_closure"
    // because the closure will change the internal data's value of Object Cacher<u32>
    let mut expensive_closure = Cacher::new( |num| {
        println!("calculating slowly...");
        thread::sleep( Duration::from_secs(2) ); // sleep for 2 seconds
        num
    } );

    if intensity < 25 {
        println!("Today, do {} pushups!", expensive_closure.value(intensity) );
        println!( "Next, do {} situps!", expensive_closure.value(intensity) );
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
            println!( "Today, run for {} minutes!", expensive_closure.value(intensity) );
        }
    }

}


#[cfg(test)]
mod tests {

    #[test]
    fn call_with_different_values() {
        let mut c = super::Cacher::new(|a| a);
        let v1 = c.value(1);

        // The value of c.value(xxx) will not changed forever ( always return 1 )
        // because it has been locked after the 1st call 
        // always keep   return 1 for the upcomming calling
        let v2 = c.value(2);

        // the statement will got panic when execute the command :    
        //     $ cargo test
        assert_eq!( v2, 2 ); 
    }
}

