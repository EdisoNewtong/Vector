
fn case_0() {

    let a: Option<i32> = Some(5);

    // [ERROR] at compile time , because Some(x) will be mismatched if a's value is NONE 
    //     refutable pattern in local binding : 'None' not covered

    // let Some(x) = a;
    if let Some(x) = a { println!("if let matched"); }
    else               { println!("if let -> else"); }
}

fn case_1() {

}





fn main() {
    case_0(); 
    println!("---------------------------------------------------------------------------");
}



