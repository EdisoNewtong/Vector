
fn get_largest_number_1(ary: &[i32]) -> i32
{
    let mut largest = ary[0];
    // item is a kind of   &i32    
    //  in rust   &i32 stand for the        int*   in   C/C++
    for item in ary {
        // you need to use the  (*item) 
        //     to get the real int value where it stored at address given by the pointer item ( a kind of &i32 )
        if *item > largest {
            largest = *item;
        }
    }

    largest
}


fn get_largest_number_2(ary: &[i32]) -> i32
{

    let mut largest = ary[0];
    // &item   yield ->   i32 
    //  it do de-ref implictly by the compiler
    for &item in ary {
        if item > largest {
            largest = item;
        }
    }

    largest
}




fn main() 
{
    println!();
    let lst1 = vec![34,   50,  25,  100,  65];
    let largest_1 = get_largest_number_1(&lst1);
    println!("The largest number inside list#1 {:?}  is :   {}", lst1,  largest_1);

    println!();

    let lst2 = vec![102,  34,  6000,  89,  54,  2,  43,  8];
    let largest_2 = get_largest_number_2(&lst2);
    println!("The largest number inside list#2 {:?}  is :   {}", lst2,  largest_2);
    
}



