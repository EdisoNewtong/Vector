/**************************************************

error[E0282]: type annotations needed for `Vec<T>`
 --> src\main.rs:20:9
   |
20 |     let v = Vec::new();
   |         ^   ---------- type must be known at this point
   |
help: consider giving `v` an explicit type, where the type for type parameter `T` is specified
   |
20 |     let v: Vec<T> = Vec::new();
   |          ++++++++

For more information about this error, try `rustc --explain E0282`.
**************************************************/
fn v_test1()
{
    println!("---------- #1 ----------");
    // [ERROR] @ Compile Time
    // let v = Vec::new();
    let v: Vec<i32> = Vec::new();
    println!("length of v = {}", v.len() );
}

fn v_test2()
{
    println!("---------- #2 ----------");
    let v = vec![1,2,3];  // => v's actual type is deduced by the compiler  => Vec<i32>
    println!("length of v = {}", v.len() );
}


fn v_test3()
{
    println!("---------- #3 ----------");
    let mut v = Vec::new();
    v.push(1);  //  Here : => v's actual type is deduced by the compiler  => Vec<i32>
    println!("length of v = {}", v.len() );
}


fn v_test4()
{
    println!("---------- #4 ----------");
    let v = vec![111,222,333,444,555];
    //
    // Notes :
    //     if index is out of range of [0,4] 
    //        the program will result in a *** RunTime *** error ( panic )
    let element_3rd = &v[2];
    println!("&v[2] is {}", element_3rd);


    //
    // vec.get(index) will be executed safely
    //
    match v.get(2) {
        Some(third_element) => {
            println!("The result of v.get(2) is {}", third_element);
        },
        None                => {
            println!("The result of v.get(2) is *** None ***");
        }
    }
}


fn v_test5()
{
    println!("---------- #5 ----------");

    // [ERROR] Compile 
    let mut v = vec![1,2,3,4,5];
    // let first = &v[0]; // immutable borrowing
    // v.push(6);         // mutable borrowing
    // //                             immutable borrowing used
    // println!("first element = {}", first );

    /***************************************************
    // Compile [PASSED]

    // Ver#1 PASSED
    let mut v = vec![1,2,3,4,5];
    let first = &v[0];
    v.push(6);

    // Ver#2 PASSED
    let mut v = vec![1,2,3,4,5];
    let first = &v[0];
    v.push(6);
    println!("v.length = {}", v.len() );


    // Ver#3 PASSED
    let mut v = vec![1,2,3,4,5];
    // Notes : Change the order of  v.push(6) and let first = &v[0];
    v.push(6);         // mutable borrowing
    let first = &v[0]; // immutable borrowing
    //                             immutable borrowing used
    println!("first element = {}", first );

    ****************************************************/

}


fn v_test6()
{
    println!("---------- #6 ----------");
    let mut ary = vec![12, 34, 56];

    let mut idx; // index
    println!("Originally => ");
    idx = 0;
    for e in &mut ary {
        println!("\t{}. {}", idx, e);
        idx += 1;
    }

    // * 10
    for e in &mut ary {
        // Core Core Core :
        *e *= 10;
    }

    println!("After elements * 10 => ");
    idx = 0;
    for e in &mut ary {
        println!("\t{}. {}", idx, e);
        idx += 1;
    }
}

// Excel's Grid-Cell
enum SpreadsheetCell {
    Int(i32),
    Float(f64),
    Text(String),
}

fn v_test7()
{
    // Core Core Core : you can store difference type of data in vector by  'enum' syntactic sugar
    let _row = vec![
        SpreadsheetCell::Int(3),
        SpreadsheetCell::Text(String::from("hello")),
        SpreadsheetCell::Float(3.14),
    ];
}


fn main() {
    v_test1();
    println!();
    v_test2();
    println!();
    v_test3();
    println!();
    v_test4();
    println!();
    v_test5();
    println!();
    v_test6();
    println!();
    v_test7();
}



