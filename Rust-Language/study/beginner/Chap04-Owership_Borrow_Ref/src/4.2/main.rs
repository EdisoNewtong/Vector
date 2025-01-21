
fn calculate_length1(s: String) -> (String, usize)
{
    let length = s.len();

    (s, length)
}


/***************************************************

error[E0382]: borrow of moved value: `s1`
  --> main.rs:50:24
   |
46 |     let s1 = String::from("Hello");
   |         -- move occurs because `s1` has type `String`, which does not implement the `Copy` trait
47 |
48 |     let (s2,len) = calculate_length1(s1);
   |                                      -- value moved here
49 |
50 |     println!("s1 = {}",s1); // Compile [ERROR] Here
   |                        ^^ value borrowed here after move
   |
note: consider changing this parameter type in function `calculate_length1` to borrow instead if owning the value isn't necessary
  --> main.rs:2:25
   |
2  | fn calculate_length1(s: String) -> (String, usize)
   |    -----------------    ^^^^^^ this parameter takes ownership of the value
   |    |
   |    in this function
   = note: this error originates in the macro `$crate::format_args_nl` which comes from the expansion of the macro `println` (in Nightly builds, run w
ith -Z macro-backtrace for more info)
help: consider cloning the value if the performance cost is acceptable
   |
48 |     let (s2,len) = calculate_length1(s1.clone());
   |                                        ++++++++

error: aborting due to previous error

For more information about this error, try `rustc --explain E0382`.

***************************************************/
fn ex1()
{
    println!("----- stupid movement and giveback -----");
    let s1 = String::from("Hello");

    let (s2,len) = calculate_length1(s1);

    // println!("s1 = {}",s1); // Compile [ERROR] Here
    println!("s2 = {}, len = {}",s2, len);
}


// the reference   '&s'   is immutable   by default
//                      &   is the sign to tell the compiler s is a borrowed (reference) parameter rather taken the passed argument's owership
fn calculate_length2(s: &String) -> usize
{
    s.len()
} // s is still valid rather than calling drop() function


fn ex2()
{
    println!("----- Borrowing #1 the varible without take its owership -----");
    let s1 = String::from("Hello");
    //                    &s1 is an immutable reference , s1 cannot be modified inside the function calculate_length2()
    //                    use  '&'  to create a borrowed reference which do not take the ownership of s1
    let len = calculate_length2(&s1);

    // so after the calling of calculate_length2(), 's1' still keep its ownership without move to another varible
    println!("s1 = \"{}\"  len = {}",s1, len); // Compile [PASSED]
}


fn calculate_length3(s: &mut String) -> usize
{
    s.push_str(", World");
    s.len()
} // s is still valid rather than calling drop() function



/***************************************************

error[E0308]: mismatched types
   --> main.rs:112:33
    |
112 |     let len = calculate_length3(&s1);
    |               ----------------- ^^^ types differ in mutability
    |               |
    |               arguments to this function are incorrect
    |
    = note: expected mutable reference `&mut String`
                       found reference `&String`
note: function defined here
   --> main.rs:76:4
    |
76  | fn calculate_length3(s: &mut String) -> usize
    |    ^^^^^^^^^^^^^^^^^ --------------

error: aborting due to previous error

For more information about this error, try `rustc --explain E0308`.

***************************************************/
fn ex3()
{
    println!("----- Borrowing #1 the mutable varible without take its owership -----");
    let mut s1 = String::from("Hello");
    //                          &s1  (&s1 is an immutable reference ) <----------------- ^^^ types differ in mutability
    let len = calculate_length3(&mut s1);
    println!("s1 = \"{}\"  len = {}", s1, len); // Compile [PASSED]
}


/***************************************************

error[E0499]: cannot borrow `s` as mutable more than once at a time
   --> main.rs:139:17
    |
138 |     let s1    = &mut s;
    |                 ------ first mutable borrow occurs here
139 |     let s2    = &mut s;
    |                 ^^^^^^ second mutable borrow occurs here
140 |     println!("s1 = {}, s2 = {}", s1, s2);
    |                                  -- first borrow later used here

error: aborting due to previous error

For more information about this error, try `rustc --explain E0499`.

***************************************************/
fn ex4()
{
    println!("----- 2 mutable refereces pointer to the same varible is [NOT] Allowed -----");
    let mut s = String::from("Hello");
    let s1    = &mut s;
    let s2    = &mut s;
    println!("s1 = {}, s2 = {}", s1, s2);
}


fn ex5()
{
    println!("----- 2 mutable refereces pointer to the same varible is [NOT] Allowed -----");
    let mut s = String::from("Hello");
    {
        // 2 reference(s) in different scopes are allowed
        let s1    = &mut s;
        println!("s1 = {}", s1);
    }
    let s2    = &mut s;
    println!("s2 = {}", s2);
}


/***************************************************

error[E0502]: cannot borrow `s` as mutable because it is also borrowed as immutable
   --> main.rs:183:17
    |
181 |     let r1    = &s; // immutable reference #1  named  'r1' -> s
    |                 -- immutable borrow occurs here
182 |     let r2    = &s; // immutable reference #2  named  'r2' -> s , it is allowed that there are more than one immutable referenc...
183 |     let ms    = &mut s; // mutable reference has been created while immutable reference is existed already    =>  Compile [ERROR]
    |                 ^^^^^^ mutable borrow occurs here
184 |     println!("r1 = {}, r2 = {}, ms = {}", r1, r2, ms );
    |                                           -- immutable borrow later used here

error: aborting due to 2 previous errors

Some errors have detailed explanations: E0499, E0502.
For more information about an error, try `rustc --explain E0499`.

***************************************************/
fn ex6()
{
    println!("----- 1 immutable accompany with 1 mutable pointer to the same varible is [NOT] Allowed -----");
    let mut s = String::from("Hello");
    let r1    = &s; // immutable reference #1  named  'r1' -> s
    let r2    = &s; // immutable reference #2  named  'r2' -> s , it is allowed that there are more than one immutable references at the same tims
    let ms    = &mut s; // mutable reference has been created while immutable reference is existed already    =>  Compile [ERROR] 
    println!("r1 = {}, r2 = {}, ms = {}", r1, r2, ms );
}


/***************************************************

error[E0106]: missing lifetime specifier
   --> main.rs:207:28
    |
207 | fn dangling_reference() -> &String
    |                            ^ expected named lifetime parameter
    |
    = help: this function's return type contains a borrowed value, but there is no value for it to be borrowed from
help: consider using the `'static` lifetime
    |
207 | fn dangling_reference() -> &'static String
    |                             +++++++

error: aborting due to previous error

For more information about this error, try `rustc --explain E0106`.

***************************************************/
fn dangling_reference() -> &String
{
	let tmp_str = String::from("Hello");
	&tmp_str // the returning of an reference of varible whose lifetime will be expired comming soon is not allowed
}

fn ex7()
{
    println!("----- ERROR Dangling Reference -----");
	let s = dangling_reference();
	println!("s = {}", s);
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
    println!();
    ex5();
    println!();
    ex6();
    println!();
    ex7();
}


