
fn take_ownership(some_string: String) {
    println!("some_string = {}", some_string);
} // argument "some_string" will be droped here

fn makes_copy(some_number: i32) {
    println!("some_number = {}", some_number);
} // because


/***************************************************

error[E0382]: borrow of moved value: `s`
  --> main.rs:52:23
   |
48 |     let s = String::from("Hello");
   |         - move occurs because `s` has type `String`, which does not implement the `Copy` trait
49 |
50 |     take_ownership(s); // s has been moved into the function's argument and s will be invalid after the function-call
   |                    - value moved here
51 |
52 |     println!("s = {}",s); // Compile [ERROR] Here
   |                       ^ value borrowed here after move
   |
note: consider changing this parameter type in function `take_ownership` to borrow instead if owning the value isn't necessary
  --> main.rs:2:32
   |
2  | fn take_ownership(some_string: String) {
   |    --------------              ^^^^^^ this parameter takes ownership of the value
   |    |
   |    in this function
   = note: this error originates in the macro `$crate::format_args_nl` which comes from the expansion of the macro `println` (in Nightly builds, run w
ith -Z macro-backtrace for more info)
help: consider cloning the value if the performance cost is acceptable
   |
50 |     take_ownership(s.clone()); // s has been moved into the function's argument and s will be invalid after the function-call
   |                     ++++++++

error: aborting due to previous error

For more information about this error, try `rustc --explain E0382`.

***************************************************/
fn ex1()
{
    println!("----- Sample for String Data Type -----");
    // Sample #1
    let s = String::from("Hello"); // s is a kind of String Data-Type

    take_ownership(s); // s has been moved into the function's argument and s will be invalid after the function-call

    println!("s = {}",s); // Compile [ERROR] Here
}


fn ex2()
{
    println!("----- Sample for Basic Data Type : i32 -----");
    let x = 5;

    makes_copy(x); // because

    println!("x = {}", x); // Compile [OK] here
}


fn gives_owership() -> String
{
    let some_string = String::from("hello");
    some_string   // return value
}

fn takes_and_gives_back(a_string: String) -> String 
{
    a_string // return value
}

/***************************************************

error[E0382]: borrow of moved value: `s2`
   --> main.rs:121:24
    |
117 |     let s2 = String::from("hello");
    |         -- move occurs because `s2` has type `String`, which does not implement the `Copy` trait
118 |
119 |     let s3 = takes_and_gives_back(s2);
    |                                   -- value moved here
120 |     println!("s1 = {}",s1);
121 |     println!("s2 = {}",s2);
    |                        ^^ value borrowed here after move
    |
note: consider changing this parameter type in function `takes_and_gives_back` to borrow instead if owning the value isn't necessary
   --> main.rs:73:35
    |
73  | fn takes_and_gives_back(a_string: String) -> String
    |    --------------------           ^^^^^^ this parameter takes ownership of the value
    |    |
    |    in this function
    = note: this error originates in the macro `$crate::format_args_nl` which comes from the expansion of the macro `println` (in Nightly builds, run
with -Z macro-backtrace for more info)
help: consider cloning the value if the performance cost is acceptable
    |
119 |     let s3 = takes_and_gives_back(s2.clone());
    |                                     ++++++++

error: aborting due to 2 previous errors

For more information about this error, try `rustc --explain E0382`.

***************************************************/
fn ex3()
{
    println!("----- Sample for function-arguments movement -----");
    let s1 = gives_owership();
    // gives_owership();
    
    let s2 = String::from("hello");
    
    let s3 = takes_and_gives_back(s2); // s2 will be moved into the function and it be droped after the function-call
    println!("s1 = {}",s1);
    println!("s2 = {}",s2); // Compile [ERROR] Here
    println!("s3 = {}",s3);
    
} // s1 and s3 will be droped after  the function  ex3() invoked


fn calculate_length(s: String) -> (String, usize)
{
    let length = s.len();

    (s, length)
}


/***************************************************

error[E0382]: borrow of moved value: `s1`
   --> main.rs:175:24
    |
171 |     let s1 = String::from("Hello");
    |         -- move occurs because `s1` has type `String`, which does not implement the `Copy` trait
172 |
173 |     let (s2,len) = calculate_length(s1);
    |                                     -- value moved here
174 |
175 |     println!("s1 = {}",s1);
    |                        ^^ value borrowed here after move
    |
note: consider changing this parameter type in function `calculate_length` to borrow instead if owning the value isn't necessary
   --> main.rs:127:24
    |
127 | fn calculate_length(s: String) -> (String, usize)
    |    ----------------    ^^^^^^ this parameter takes ownership of the value
    |    |
    |    in this function
    = note: this error originates in the macro `$crate::format_args_nl` which comes from the expansion of the macro `println` (in Nightly builds, run
with -Z macro-backtrace for more info)
help: consider cloning the value if the performance cost is acceptable
    |
173 |     let (s2,len) = calculate_length(s1.clone());
    |                                       ++++++++

error: aborting due to 3 previous errors

For more information about this error, try `rustc --explain E0382`.

****************************************************/
fn ex4()
{
    println!("----- stupid movement and giveback -----");
    let s1 = String::from("Hello");

    let (s2,len) = calculate_length(s1);

    println!("s1 = {}",s1); // Compile [ERROR] Here
    println!("s2 = {}, len = {}",s2, len);

}



fn main()
{

    ex1();
    println!();
    ex2();
    println!();
    ex3();
    println!();
    ex4();


}






