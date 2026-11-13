# void return function in Rust 
```Rust

/* the final return value of f1()  is an ==**EMPTY tuple**== (a tuple with 0 element )
the return logic is different from 'C'   ==**void**== return type
fn f1()  ->  () */
fn f1()
{
    println!("in function #1");
}

/*********************************************
// The following form will raise an compile error
// Because the rust compiler treat the following function as a function with EMPTY tuple return value =>  () 
fn f2() 
{ 
}
*********************************************/

/*  Here  '!' , the exclmation mark stands for   a Never type */
fn f2() -> !
{
    // Correct form of Never type
    loop {
    }
}


fn main()
{
    let ret_1 = f1(); //      Compile ==**Passed**==
    println!("ret_1 = {:?}", ret_1);

    f2();              //     Compile ==**Passed**==

    let _ret_2 = f2(); // <== Compile [WARNING]   warning: unreachable statement
    //           ---- any code following this expression is unreachable

    println!("ret_2 = {:?}", _ret_2);
}

```
