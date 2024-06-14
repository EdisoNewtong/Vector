


fn main()
{
    let x: i32 = -5;

    //
    //
    // It's a !!!MUST!!!  that the pair of parentheses surround the expression of if/else part , otherwise the compiler will report an error
    //                     { 100  }       { -100  }     

    /***************************************************
  --> main.rs:14:29
   |
30 |     let a: i32 = if x<0     100   else  -100;
   |                             ^^^ expected `{`
   |
note: the `if` expression is missing a block after this condition
  --> main.rs:14:21
   |
30 |     let a: i32 = if x<0     100   else  -100;
   |                     ^^^
help: try placing this code inside a block
   |
30 |     let a: i32 = if x<0     { 100 }   else  -100;
   |  
    ****************************************************/

    // In C Language :
    // int a =     x<0  ?    100      :    -100
    // let a: i32 = if x<0    { 100 }  else { -100   };
    let a: i32 = if x<0  {  100  }  else {  -100 };
    println!("a = {a}");

    /*
error[E0308]: `if` and `else` have incompatible types
  --> main.rs:37:45
   |
51 |     let b      = if x<0   { val   } else  { ch };
   |                             ---             ^^ expected integer, found `char`
   |                             |
   |                             expected because of this

error: aborting due to 1 previous error
    */

    /*
    let ch = 'a';
    let val = 123;
    let b      = if x<0   { val   } else  { ch };
    println!("b = {b}");
    */
}


