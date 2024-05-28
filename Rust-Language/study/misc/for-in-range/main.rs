
/*
fn test1(r: &std::ops::Range<i32>)
{
    for i in r {
        println!("{i}");
    }
    
}
*/


fn main()
{
    let ary: [i32;11] = [ 11,22,33,44,55,66,77,88,99, 1010, 1111 ];

    // test1(&range1);

    println!("Forward traversal ... ");
    // forward travelsal from [0, 10 )  , include [0] exclude [10] , step is +1
    for i in 0 .. 11 {
        println!("{:>2}. [{}].  {} ",i+1, i, ary[i]);
    }

    println!();
    println!("Use include end expression :    0..=10");
    for i in 0..=10 {
        println!("{:>2}. [{}].  {} ",i+1, i, ary[i]);
    }

    println!("--------------------------------------------------");
    println!("Backward traversal ... ");
    // backward travelsal from (10, 0]  ,  [9,8,7, ... , 0   step is (-1)
    for i in (0..11).rev() {
        println!("{:>2}. [{}].  {} ",i+1, i, ary[i]);
    }

    println!();
    println!("Use include end expression :    (0..=10).rev() ");
    for i in (0..=10).rev() {
        println!("{:>2}. [{}].  {} ",i+1, i, ary[i]);
    }


    println!();
    println!("--------------------------------------------------");
    println!("Forward traversal -> step 2 ... ");
    for i in (0..11).step_by(2) {
        println!("{:>2}. [{}].  {} ",i+1, i, ary[i]);
    }

    println!();
    println!("Backward traversal -> step (-2) ... ");
    // You must use step_by(2) before  rev() due to the function step_by(...) doesn't accept  negative step such as (-2) 
    for i in (0..11).step_by(2).rev() {
        println!("{:>2}. [{}].  {} ", i+1, i, ary[i]);
    }


}


