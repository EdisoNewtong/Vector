use std::sync::{ Mutex, Arc };
use std::thread;
// use std::rc::Rc;

fn test_1()
{
    let m = Mutex::new(5);

    {
        // m.lock() is a block operation
        // it will return until got the ownership of lock finally 
        //----------------------------------------------------------------------------------------------------
        //  num:   std::sync::MutexGuard<i32>
        //----------------------------------------------------------------------------------------------------
        let mut num = m.lock().unwrap();
        *num = 6;
    }

    println!("m = {:?}", m);
}

fn test_2()
{
    /*
    let counter = Mutex::new(0);
    let mut thread_handles = vec![];

    //
    // Create 10 threads, each thread will increase the counter 1 step once  
    // so after all the threads done , Possiblely the varible counter's value will stop at 10 ?
    // Really ?
    for _ in 0 .. 10 {
        // [ERROR] @ Compile Time 
        // for the 1st time, counter has been move to the 1st thread
        //  so the 2nd time, counter has been expired !!!
        //       ...
        //     the 10th time, and so forth.
        let handle = thread::spawn( move | | {
            let mut num = counter.lock().unwrap();

            *num += 1;
        });

        thread_handles.push(handle);
    }


    for handle in thread_handles {
        handle.join().unwrap();
    }
    println!("Result: {}", *counter.lock().unwrap() );
    */
}

fn test_3()
{
    let counter = Arc::new( Mutex::new(0) );
    let mut thread_handles = vec![];

    //
    // Create 10 threads, each thread will increase the counter 1 step once  
    // so after all the threads done , Possiblely the varible counter's value will stop at 10 ?
    // Really ?
    for _ in 0 .. 10 {
        let counter = Arc::clone( &counter );
        let handle = thread::spawn( move | | {
            let mut num = counter.lock().unwrap();

            *num += 1;
        });

        thread_handles.push(handle);
    }


    for handle in thread_handles {
        handle.join().unwrap();
    }
    // result will be 10 as expected
    println!("Result: {}", *counter.lock().unwrap() );

}


fn main() {
    test_1();
    println!();
    // test2();
    test_3();
}



