
fn add(a: i32, b: i32) -> i32 {
    a+b
}

fn minus(a: i32, b: i32) -> i32 {
    a-b
}

fn multiply(a: i32, b: i32) -> i32 {
    a*b
}

fn divide(a: i32, b: i32) -> i32 {
    a/b
}

fn do_op(func: fn(i32,i32) -> i32 , op1: i32, op2: i32) -> i32 {
    func(op1, op2)
}

fn test_fn_argument()
{
    println!("---------- test_fn_argument() ----------");
    let a: i32 = 11;
    let b: i32 = 4;
    println!("a + b = {}",  do_op( add,      a,b) );
    println!("a - b = {}",  do_op( minus,    a,b) );
    println!("a * b = {}",  do_op( multiply, a,b) );
    println!("a / b = {}",  do_op( divide,   a,b) );
}


fn func_vs_closure()
{
    /************************************************************
       map<self, F>(self, f: F)
       F: FnMut(Self::Item)

       Both closure and function-pointer can meet the demands 
       of the function signature's requirement
            
     *************************************************************/
    println!("---------- func_vs_closure() ----------");
    let list_of_numbers = vec![1,2,3];
    let list_of_string:  Vec<String> 
                      = list_of_numbers
                        .iter()
                        .map(|i| i.to_string()) // use closure feature
                        .collect();

    let list_of_numbers = vec![1,2,3];
    let list_of_string:  Vec<String> 
                      = list_of_numbers
                        .iter()
                        .map( ToString::to_string ) // use function pointer
                        .collect();
    
}

fn enum_tuple()
{
    println!("---------- enum_tuple() ----------");
    enum Status {
        Value(u32),
        Stop,
    }

    //                    it looks like a function call 
    //          Actually  it is implemented as a constructor function ( like c++ class's constructor function ) 
    let s_value: Status = Status::Value(3);

    let list_of_statuses: Vec<Status> 
       = (0u32 .. 20)
         .map( Status::Value ) // so HERE , expression  'Status::Value' has been treated as a constructor function 
         .collect();
}

/************************************************************
The following code will compile ERROR

fn return_closure() ->     Fn(i32) -> i32           {
    |x| x + 1
}
************************************************************/
fn return_closure() ->     Box<dyn Fn(i32) -> i32> {
    Box::new( | x | x + 2 )
}

fn invoke_closure_from_return_fn() {
    println!("---------- invoke_closure_from_return_fn() ----------");
    let f: Box<dyn Fn(i32) -> i32> = return_closure();
    let x: i32 = f(10);
    //                                     12
    println!("after call f(10) -> it return {}", x ); // output : 12
}


fn main() {
    test_fn_argument();
    println!("--------------------------------------------------");
    func_vs_closure();
    println!("--------------------------------------------------");
    enum_tuple();
    println!("--------------------------------------------------");
    invoke_closure_from_return_fn();
}




