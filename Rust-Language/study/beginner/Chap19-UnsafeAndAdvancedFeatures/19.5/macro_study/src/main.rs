// let v: Vec<u32> = vec![1, 2, 3 ];

// #[macro_export]指明 : 它所处的包被引用作用域后，这个 宏 才能被使用
//  缺少了 这个标注的 宏，就不能被引用 作用域 
#[macro_export]

// macro_rules!  is the beginning declaration of the    macro 
//           vec is the macro's name 
macro_rules! vec {
// like the match pattern  ,  the following line whose end tag is '=>'   is the pattern  
//   $x: expr    match  arbitrary rust expressions
//             , the comma itself   e.g.   1, 2, 3, 4  ( there are 3 ',' )
//              * it can match 0-n  previous item   $( $x: expr ),
( $( $x: expr ),* ) => 
{
/******************************************************************
'match'        match    value     
'macro code'   match    a bunch of code snippets with [**Same**] struct

*******************************************************************/
// the corresponding implementation code
/*
let v: Vec<u32> = vec![1, 2, 3 ];

        $(
            temp_vec.push($x);
        )*

it will generate 3 codes like the following ( so use   $( ... )*  to generate 0~n times by the given arguments  ):

                temp_vec.push(1);
                temp_vec.push(2);
                temp_vec.push(3);

*/
    {

        let mut temp_vec = Vec::new();
        $(
            temp_vec.push($x);
        )*

        temp_vec
    }
};

}

/****************************************************************************************************
use proc_macro;

// TokenStream is define inside 'proc_macro' package  previously mentioned 
#[some_attribute]
//                      input macro  ->  out macro
pub fn some_name(input: TokenStream) -> TokenStream 
{
}
*****************************************************************************************************/

fn main() {
    // println!("Hello, world!");
}





