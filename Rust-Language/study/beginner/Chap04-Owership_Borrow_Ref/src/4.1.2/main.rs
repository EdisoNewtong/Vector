
fn simple_example()
{
    println!("----- simple_example() -----");
    let s = String::from("Hello"); // s will be valid here
    // you can use s when it is in the scope
    println!("s = {}", s);
} // s will be droped while it is out of scope 



fn basic_datatype_copy()
{
    println!("----- basic_datatype_copy() -----");
    let v1 = 5; // i32
    let v2 = v1;
    println!("i32 v1 = {}", v1);
    println!("i32 v2 = {}", v2);
}




/***************************************************

error[E0382]: borrow of moved value: `v1`
  --> main.rs:57:36
   |
53 |     let v1 = String::from("Hello"); // a type: String
   |         -- move occurs because `v1` has type `String`, which does not implement the `Copy` trait
54 |     let v2 = v1;                    // a type: String , v1 is moved into v2 and v1 will be set as invalid , Only 'v2' will be droped while it ...
   |              -- value moved here
...
57 |     println!("String v1 = \"{}\"", v1); // here Compile [ERROR]
   |                                    ^^ value borrowed here after move
   |
   = note: this error originates in the macro `$crate::format_args_nl` which comes from the expansion of the macro `println` (in Nightly builds, run w
ith -Z macro-backtrace for more info)
help: consider cloning the value if the performance cost is acceptable
   |
54 |     let v2 = v1.clone();                    // a type: String , v1 is moved into v2 and v1 will be set as invalid , Only 'v2' will be droped whil
e it is out of scope
   |                ++++++++

error: aborting due to previous error

For more information about this error, try `rustc --explain E0382`.

****************************************************/
fn special_datatype_copy1()
{
    println!("----- special_datatype_copy1() -----");
    let v1 = String::from("Hello"); // a type: String
    let v2 = v1;                    // a type: String , v1 is moved into v2 and v1 will be set as invalid , Only 'v2' will be droped while it is out of scope 
    // !! Notes:  !!
    //     so there is no chance to hit  <double free>  bug , because v1 has been moved and invalidated after   the statement   v2 = v1
    println!("String v1 = \"{}\"", v1); // here Compile [ERROR]  
    println!("String v2 = \"{}\"", v2);
} // v1 will not be droped because it is invalid after the statement   "let v2 = v1;"


fn special_datatype_copy2()
{
    println!("----- special_datatype_copy2() -----");
    let v1 = String::from("Hello"); // a type: String
    let v2 = v1.clone();            // a type: String , v1 will keep valid , because it use the clone() method 

    println!("String v1 = \"{}\"", v1);
    println!("String v2 = \"{}\"", v2);
}



fn main()
{
    simple_example();
    println!();
    basic_datatype_copy();
    println!();
    special_datatype_copy1();
    println!();
    special_datatype_copy2();
}





