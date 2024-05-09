/***************************************************
     let a: i32 = 12;

   //
   // Correct
   //
   a = a + 1;   a = a - 1;
   a += 1;      a -= 1;


   ++a;  // Compile Error
   a++;  // Compile Error

   --a;  // Compile Error
   a--;  // Compile Error


****************************************************/




// #[allow(dead_code)]
fn op_int()
{
    op_result(8,3);
    println!();
    op_result(3,8);
    println!();


    op_result(-8,3);
    println!();
    op_result(-3,8);
    println!();

    op_result(-8,-3);
    println!();
    op_result(-3,-8);
    println!();

                                  
}


fn op_result(n1: i32, n2 : i32)
{
    println!("--------------------------------------------------");
    println!("({}) + ({}) = ({})", n1,n2, n1 + n2);
    println!("({}) - ({}) = ({})", n1,n2, n1 - n2);
    println!("({}) * ({}) = ({})", n1,n2, n1 * n2);
    println!("({}) / ({}) = ({})", n1,n2, n1 / n2);
    println!("({}) % ({}) = ({})", n1,n2, n1 % n2);
    println!("({}) & ({}) = ({})", n1,n2, n1 & n2);
    println!("({}) | ({}) = ({})", n1,n2, n1 | n2);
    println!("({}) ^ ({}) = ({})", n1,n2, n1 ^ n2);
    if n2 > 0 {
        println!("{} << {} = {}",n1,n2, n1 << n2);
    } else {
        println!("the Left operand of operator  '<<' n2={n2} is a negative number! ");
    }

    if n2 > 0 {
        println!("{} >> {} = {}",n1,n2, n1 >> n2);
    } else {
        println!("the Right operand of operator  '>>' n2={n2} is a negative number! ");
    }
    println!("-({}) = {}",n1,-n1);
    println!("~({}) = {}",n1,!n1); // different from The C Language , ~a means flip all binary bits of a number , !a for  Rust
    println!("--------------------------------------------------");

}


fn main() 
{
	op_int();
    println!();
}




