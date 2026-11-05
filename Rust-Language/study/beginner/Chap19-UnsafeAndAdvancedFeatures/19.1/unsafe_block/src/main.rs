use std::slice;

// Rust Call ->  C function
extern "C" {
    // here is a pre-declear of function abs(...) which is implemented by outside C language
    fn abs(input: i32) -> i32;
}


// C Call ->  Rust function
#[no_mangle]
pub extern "C" fn call_from_c() {
    println!("Just called a Rust function from C!");
}

//
// use keyword 'unsafe' as a qualifier of the given function
//
unsafe fn unsafe_function() 
{
    println!("------- call unsafe_function(...) -------");
}


fn unsafe_code_0() {
    println!("------- call unsafe_code_0(...) -------");

    let mut num = 5;

    // varible declaration is VALID , no need to surrounded with unsafe { ... }
    let r1 = &num     as  *const i32;
    let r2 = &mut num as  *mut   i32;

    // It's must to wrap the dereference of a named pointer    inside  unsafe { ... }block
    //   Otherwise it will raise a compile-time [ERROR]
    unsafe {
        println!("r1: {}", *r1);
        println!("r2: {}", *r2);
    }


    ////////////////////////////////////////////////////
    //
    // given the address with Hard-Code 
    //
    ////////////////////////////////////////////////////
    let address = 0x123456usize;    // type:   usize
    let r = address as *const i32;  // type:   const int*
    unsafe  {
        println!("*r = {}", *r); // Maybe Runtime Crash 
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Compile-time ERROR
//         error[E0499]: cannot borrow `*slice` as mutable more than once at a time                
//
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
fn split_at_mut_1(slice: &mut [i32], mid: usize) -> (&mut [i32], &mut [i32]) 
{
    let len = slice.len();
    assert!(mid <= len);

    // error[E0499]: cannot borrow `*slice` as mutable more than once at a time
    ( &mut slice[ ..mid],  &mut slice[mid ..] )
//|     ----------------------------^^^^^----------
//|     |      |                    |
//|     |      |                    second mutable borrow occurs here
//|     |      first mutable borrow occurs here
//|     returning this value requires that `*slice` is borrowed for `'1`
}
*/


fn split_at_mut_2(slice: &mut [i32], mid: usize) -> (&mut [i32], &mut [i32]) 
{
    let len = slice.len();
    let ptr = slice.as_mut_ptr();  // &mut [i32]
    assert!(mid <= len);

    unsafe {
        ( //                 mid is a length of slice 
          slice::from_raw_parts_mut(ptr, mid),              // left  part  len = mid
          slice::from_raw_parts_mut(ptr.add(mid), len-mid ) // right part  len = (len-mid)
        )
    }
}



fn cut_vec()
{
    let mut v = vec![1,2,3,  4,5,6];

    let r = &mut v[ .. ];

    // let (a,b) = r.split_at_mut(3);
    let (a,b) = split_at_mut_2(r,3); 

    assert_eq!(a, &mut [1,2,3]);
    assert_eq!(b, &mut [4,5,6]);
}


fn main() {
    unsafe_code_0();

    // It's must to wrap the invoking function which is mared as'unsafe'
    unsafe { 
        unsafe_function();
    } 

    cut_vec();

    unsafe {
        // Call C interface named abs   in Rust language
        let p_x: i32            = abs(-3);
        println!("p_x = {}", p_x);
    }

}






