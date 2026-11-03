
fn if_let_match_case()
{
    let favorite_color: Option<&str> = None;
    let is_tuesday:             bool = false;
    let age: Result<u8,_> = "34".parse();

    /**************************************************
        if let   can refers to any different irrelative varibles
    ***************************************************/


    if let Some(color) = favorite_color {
        println!("Using your favorite color, {}, as the background", color);
    } else if is_tuesday {
        println!("Tuesday is [Green] day!");


    // The 2 'age' are different varibles
    //   1.  the 'age' wrapped with Ok(...)   
    //   2.  the 'age' after =   
    //  
    } else if let Ok(age) = age {
        // refers to the age wrapped with Ok(...)
        if age > 30 {
            // :)   [Execute] this statement
            println!("Using [Purple] as the background color"); 
        } else {
            println!("Using [Orange] as the background color");
        }
    } else {
        println!("Using [Blue] as the background color");
    }
}

fn while_let()
{
    let mut stack = Vec::new();
    stack.push(1);
    stack.push(2);
    stack.push(3);

    /* output :
      3
      2
      1
    */
    while let Some(top) = stack.pop() {
        println!("{}", top);
    }
}

fn for_loop()
{
    let v = vec!['a', 'b', 'c'];

    // like other programming language
    // index starts at [0]
    // for match (One tuple) with 2 elements
    //    1st.   index
    //    2nd.   value
    for (index, value) in v.iter().enumerate() {
        println!("{} is at index {}", value, index);
    }
}

fn let_match()
{
    // varibles 'a'   match an i32 value
    let a = 5;

    // match each tuple's element with the given tuple
    //   x: 1
    //   y: 2
    //   z: 3
    let (x,y,z) = (1,2,3);

    // [ERROR] at compile time
    // let (q,w) = (4,5,6);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//here x is a common pattern to match an upcoming i32 varible or an fixed number 
fn foo(x: i32) 
{
    println!("in function foo, x = {}", x);
}

// function's arguments can also  apply on  < Pattern Match > 
//     extract the passed tuple[0] -> x
//     extract the passed tuple[1] -> y
fn print_coordinates(&(x,y) : &(i32,i32) ) {
    println!("Current location: ({}, {})", x,y );
}

fn fn_pattern()
{
    foo(2333);
    let point = (3,5);
    print_coordinates(&point);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

fn main() {
    if_let_match_case();
    println!("---------------------------------------------------------------------------");
    while_let();
    println!("---------------------------------------------------------------------------");
    for_loop();
    println!("---------------------------------------------------------------------------");
    let_match();
    println!("---------------------------------------------------------------------------");
    fn_pattern();
}



