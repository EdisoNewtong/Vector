

fn match_fixednumber() 
{
    println!("----- inside match_fixednumber() -----");
    let x = 1;
    match x {
        1 => { println!("match 1"); 
               println!("one");   
        },

        2 => { println!("match 2"); 
               println!("two");   
        },

        3 => { println!("match 3"); 
               println!("three"); 
        },

        _ => { println!("match any number other than 1,2,3 ."); },
    }
}

fn match_named_varible() 
{
    println!("----- inside match_named_varible() -----");
    let x = Some(5);
    let y = 10;

    match x {
        Some(50) => println!("Got 50"),

        //
        // HERE : y is a [NEW] varible whose lifetime is  inside the match code block
        //  NOT the y ( y=10 )
        Some(y)  => println!("Matched, y = {:?}", y), // output : Matched, y = 5

        _        => println!("Default case,, x = {:?}", x),
    }

    println!("at the end: x= {:?},  y = {:?}", x, y);
}



fn match_multivalues() 
{
    println!("----- inside match_multivalues() -----");
    let x = 1;
    match x {
        1 | 2 => println!("1 or 2"), // this sentence will be output
        3     => println!("3"),
        _     => println!("any other number."),
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
struct Point {
    x: i32,
    y: i32,
}



// De-Construction to match   struct / tuple / enum 
fn match_complex_object()
{
    println!("----- inside match_complex_object() -----");
    let p = Point { x:0, y:7 };

    //  a   <-  x's value
    //  b   <-  y's value
    let Point { x: a, y: b } = p; // a=0 , b=7
    println!("After a,b deconstruction point, a = {}, b = {} ",a,b);

    // syntactic sugar ( use field's name as the dereference varibles x and y )
//  let Point { x: x, y: y } = p; // a=0 , b=7
    let Point { x,    y    } = p;
    println!("After x,y deconstruction point, a = {}, b = {} ",x,y);

    match p {
        Point { x,    y: 0 } => println!("On the x axis at {}", x), // x can be any value, y is 0
        Point { x: 0, y    } => println!("On the y axis at {}", y), // x is 0, y can be any value , ==>  Output here <==
        Point { x,    y    } => println!("On neither axis: ({}, {})",x, y), 
    }
}

enum Message0 {
    Quit,
    Move { x: i32, y: i32 },      // struct
    Write(String),                // tuple with 1 string argument
    ChangeColor(i32, i32, i32),   // tuple with 3 i32    arguments
}

enum Color {
    Rgb(i32, i32, i32),
    Hsv(i32, i32, i32),
}


enum Message1 {
    Quit,
    Move { x: i32, y: i32 },      // struct
    Write(String),                // tuple with 1 string argument
    ChangeColor(Color),   // tuple with 3 i32    arguments
}



fn match_enum0() 
{
    println!("----- inside match_enum0() -----");
    let msg = Message0::ChangeColor(0, 160, 255);

    match msg {
        Message0::Quit => {
            println!("The Quit variant has no data to destructure."); // ';' can be ommited
        },

        Message0::Move { x, y } => {
            println!("Move in the x direction {} and in the y direction {}", x, y);
        },

        Message0::Write(text)  => {
            println!("Text message: {}", text);
        },

        Message0::ChangeColor(r, g, b) => {
            println!("Change the color to red {}, green {}, and blue {}", r, g, b);
        }
    }
}

fn match_enum1() 
{
    println!("----- inside match_enum1() -----");
    let msg = Message1::ChangeColor( Color::Hsv(0,160,255) );

    match msg {
        Message1::ChangeColor( Color::Rgb(r, g, b)) => {
            println!("Change the color to RGB Mode red {}, green {}, and blue {}", r, g, b);
        }

        Message1::ChangeColor( Color::Hsv(h, s, v)) => {
            println!("Change the color to Hsv Mode h {}, s {}, and v {}", h, s, v);
        }

        _ => { println!("Match other type"); }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////

fn match_range() 
{
    println!("----- inside match_range() -----");
    //   ..=
    // use   beg_value ..= end_value (included)   to match [ beg , end ]
    let x = 5;
    match x {
        1..=5 => println!("1 - 5"), // this sentence will be output
        _     => println!("any other number."),
    }


    let ch = 'c';
    match ch {
        'a' ..='j' => println!("ch is inside [a ~ j]. "), // output this line
        'k' ..='z' => println!("ch is inside [k ~ z]. "),
        _          => println!("something else"),
    }
}

fn match_tuple_and_struct()
{
    println!("----- inside match_tuple_and_struct() -----");
    //      3     10               5  -10
    let ( (feet, inches),  Point{  x,  y  } ) = ( (3,10),   Point {x:5, y: -10 } );
    println!("feet = {}, inches = {}, x = {}, y = {}", feet, inches, x, y);
}

// ignore the 1st parameter by _  ( '_'  stands for a placeholder varible )
fn match_ignored_0(_: i32, y: i32) 
{
    println!("----- inside match_ignored_0() -----");
    // Core Core Core :
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Different with C/C++ , 
    //
    // the underline     '_' is not a valid varible name in Rust
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    println!("This code only uses the y parameter: y = {}", y);
    
    //
    // the following code will raise a compile-time error by Rust compiler
//  /*println!("This code only uses the y parameter: x = {}, y = {}", _, y);  */ 

}

fn match_ignore_1()
{
    println!("----- inside match_ignored_1() -----");
    let mut setting_value = Some(5);
    let new_setting_value = Some(10);

    match (setting_value, new_setting_value) {
        // if **Both** are Some(value)  , the pattern matched
        ( Some(_), Some(_) ) => {
            println!("Can't overwrite an existing customized value.");
        }

        _   => { println!("Default match here. "); }
    }

    //                          Some(5)
    println!("setting is {:?}", setting_value);


    let number = (2,4,6,8,10);
    match number {
        (first, _, third, _,  fifth) => {
            //                           2         6         10
            println!("Some number: 1st = {}, 3rd = {}, 5th = {}", first, third, fifth)
        },
        _ => { println!("Default match here.");}
    }
}

fn match_ignore_2()
{
    println!("----- inside match_ignored_2() -----");
    let _x = 5; // suppression compiler's warning  because of the varible is start with _
    let  y = 5; // [WANRING] Compile warning because of unused varible 'y'
}

fn match_ignore_3()
{
    println!("----- inside match_ignored_3() -----");

    let s = Some( String::from("Hello!") );
    if let Some(_s) = s {
//              -- value partially moved here
        println!("found a string");
    }

    //
    // UnComment the following line will raise a compile error
    // [ERROR] Compile-time error , because s has been moved to  Some(_s)
//  /* println!("After if let ,   s = {:?}", s); */
    //                                       ^ value borrowed here after partial move
}

fn match_ignore_4()
{
    println!("----- inside match_ignored_4() -----");

    let s = Some( String::from("Hello!") );

    // use      _  , the [Binding] operation will not happen .
    if let Some(_) = s {
        println!("found a string");
    }

    // Compile OK :)
    println!("After if let ,   s = {:?}", s);
}

struct Point3 {
    x: i32,
    y: i32,
    z: i32,
}



fn match_ignore_5()
{
    println!("----- inside match_ignored_5() -----");
    let origin = Point3 { x:0, y:0, z:0 };
    match origin {
        Point3 { x,   .. }  => println!("x is {} , y and z are both ignored", x),
    }

    let numbers = (2,4,6,8,10);
    match numbers {
        // 2               10
        (first,    ..,   last) => {
            println!("Some number has been matched : 1st = {}, last = {}", first, last);
        }
    }

    /****************************************************************************************************

    Compile-time ERROR : which one is the middle one inside   tuple numbers ?

    match numbers {
        ( ..,    middle_one,   ..) => {
            println!("middle number has been matched : middle = {}", middle_one);
        }
    }
    */

}

fn match_guard_0()
{
    println!("----- inside match_guard_0() -----");
    let num = Some(4);
    match num {
        //     (if x < 5)  is a kind of match guard
        Some(x) if x < 5 => println!("less than five: {}",x),
        Some(x)          => println!("{}", x),
        None             => (),
    }


    let x = Some(5);
    let y = 10;

    match x {
        Some(50)            => println!("Got 50"),
        //     (if n == y)  is a kind of match guard
        Some(n) if n == y   => println!("Matched, n = {:?}", n),
        _                   => println!("Default case, x = {:?}",x),
    }
    println!("at the end: x= {:?}, y = {:?}", x,y);
}

fn match_guard_1()
{
    println!("----- inside match_guard_1() -----");
    let x = 4;
    let y = true;

    match x {
        4 | 5 | 6    if y     =>    println!("yes"),  // output this line
        _                     =>    println!("no"),  
    }
}

enum Message {
    Hello { id: i32 },
}


// @  syntax
fn match_binding()
{
    println!("----- inside match_binding() -----");
    let msg = Message::Hello{ id: 5 };

    match msg {
        Message::Hello {
            //      5 is inside [3,7]
            id:   id_variable @ 3..=7,
        } => {
            println!("Found an id in range: {}", id_variable) // output this line
        }

        Message::Hello { id:   10 ..=12 } => {
            println!("Found an id in another range")
        }

        Message::Hello { id } => {
            println!("Found some other id: {}", id)
        }
    }

}

fn main() {
    match_fixednumber(); // println!("Hello, world!");
    println!("==================================================");
    match_named_varible();
    println!("==================================================");
    match_multivalues();
    println!("==================================================");
    match_range();
    println!("==================================================");
    match_complex_object();
    println!("==================================================");
    match_enum0(); 
    println!("==================================================");
    match_enum1();
    println!("==================================================");
    match_tuple_and_struct();
    println!("==================================================");
    // ignore       3      match y by 4
    match_ignored_0(3,                4);
    println!("==================================================");
    match_ignore_1();
    println!("==================================================");
    match_ignore_2();
    println!("==================================================");
    match_ignore_3();
    println!("==================================================");
    match_ignore_4();
    println!("==================================================");
    match_ignore_5();
    println!("==================================================");
    match_guard_0();
    println!("==================================================");
    match_guard_1();
    println!("==================================================");
    match_binding();
}





