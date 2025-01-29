
fn largest<T>(lst: &[T]) -> T 
{
    let mut largest_num = lst[0];
    for &item in lst {
        if item > largest_num {
            largest_num = item;
        }
    }
    largest_num
}

/**********************************************************************

error[E0369]: binary operation `>` cannot be applied to type `T`
 --> src\main.rs:6:17
  |
6 |         if item > largest {
  |            ---- ^ ------- T
  |            |
  |            T
  |
help: consider restricting type parameter `T`
  |
2 | fn largest<T: std::cmp::PartialOrd>(list: &[T]) -> T
  |             ++++++++++++++++++++++

For more information about this error, try `rustc --explain E0369`.

**********************************************************************/

fn test_function()
{
    println!("----- test_function() -----");
    let lst1 = vec![34, 50, 25, 100, 65];
    let largest_1 = largest(&lst1);
    println!("largest number inside {}  is :  {}", lst1, largest_1);

    // let char_list = vec!['y', 'm', 'a', 'q'];
    //  [ERROR] @ Compile time       can't compare  2  chars
    // let largest_2 = largest(&char_list);
    // println!("largest char inside {:?}  is :  {}", char_list, largest_2);
}

//
// template struct
//
struct Point<T> {
    x: T,
    y: T,
}

impl<T> Point<T> {
    //
    // field 'x'  getter
    //
    fn x(&self) -> &T {
        &self.x
    }
}

// Core Core Core :
// Specialize template T as i32
// 
impl Point<i32> {
    //
    // field 'x'  getter
    //
    fn x1(&self) -> &i32 {
        println!("Specialized   T as i32,  fn x1() ");
        &self.x
    }
}

struct Point_2<T,V> {
    x: T,
    y: V,
}

impl <T,V> Point<T,V> {
    //         W must be neither  T nor W
    fn mixup<T,W>(self, other: Point<V,W>) -> Point<T,W> {
        // get 'x' from   self     point
        // get 'y' from   other    point
        Point {
            x: self.x,
            y: other.y,
        }
    }
}



/**********************************************************************
 
error[E0308]: mismatched types
  --> src\main.rs:68:36
   |
68 |     let misc_pt = Point  { x:-3, y:123.45 };
   |                                    ^^^^^^ expected integer, found floating-point number

For more information about this error, try `rustc --explain E0308`.

**********************************************************************/
fn test_struct_template_1() 
{
    println!("----- test_struct_template_1() -----");
    let int_pt   = Point { x:5,   y:10  };
    let float_pt = Point { x:1.0, y:4.0 };

    /*
     
        struct Point_2<T,V> {
            x: T,
            y: V,
        }

        let misc_pt = Point_2  { x:-3, y:123.45 };
    */
    let misc_pt = Point  { x:-3, y:123.45 };

    let p1 = Point { x:5, y: 10 };
    println!("p1.x = {}", p.x() );

}


fn test_struct_template_2() 
{
    println!("----- test_struct_template_2() -----");
    let p1 = Point_2 { x: 5, y: 4 };
    let p2 = Point_2 { x: "Hello", y: 'c' };
    let p3 = p1.mixup(p2);

    //
    //    i32        char
    //     |          |
    //  p3.x = 5,  y3.y = 'c' 
    println!("p3.x = {}, p3.y = {}", p3.x, p3.y );
}



//
// template enum
//
enum my_option<T> {
    Some(T),
    None
}

enum my_result<T,E> {
    Ok(T),
    Err(E),
}

fn main() 
{
    test_function();

    println!();
    test_struct_template();

    println!();
    test_struct_template_2();
}





