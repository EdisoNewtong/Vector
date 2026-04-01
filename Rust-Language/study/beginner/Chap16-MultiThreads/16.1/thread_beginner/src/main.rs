use std::thread;
use std::time::Duration;

/**************************************************
$ cargo run

    hi number 1 from the main thread!
    hi number 1 from the spawned thread!
    hi number 2 from the main thread!
    hi number 2 from the spawned thread!
    hi number 3 from the main thread!
    hi number 3 from the spawned thread!
    hi number 4 from the spawned thread!
    hi number 4 from the main thread!     // main thread finished for-loop
    hi number 5 from the spawned thread!  // there has a little bit time of CPU slice for the spawn thread to run.

***************************************************/

fn test_1()
{
    println!("---------- In test_1() ----------");
    thread::spawn( | | {
        for i in 1 .. 10 { // [ 1, 10 ) => [1,9]
            println!("hi number {} from the spawned thread!", i );
            thread::sleep( Duration::from_millis(1) );
        }
    });

    // main thread
    for i in 1 .. 5 { // [ 1, 5 ) => [1,4]
        println!("hi number {} from the main thread!", i );
        thread::sleep( Duration::from_millis(1) );
    }
}

fn test_2()
{
    println!("---------- In test_2() ----------");
    let spawn_thread_handle = thread::spawn( | | {
        for i in 1 .. 10 { // [ 1, 10 ) => [1,9]
            println!("hi number {} from the spawned thread!", i );
            thread::sleep( Duration::from_millis(1) );
        }
    });

    // main thread
    for i in 1 .. 5 { // [ 1, 5 ) => [1,4]
        println!("hi number {} from the main thread!", i );
        thread::sleep( Duration::from_millis(1) );
    }

    // Why ??? call unwrap() ?
    // if thread.join() failed , call unwrap() to raise a panic
    // the calling thread( main thread will wait ( be frozen ) until spawn_thread has been finished , main thread resume to run )
    spawn_thread_handle.join().unwrap();
}

fn test_3()
{
    println!("---------- In test_3() ----------");
    let spawn_thread_handle = thread::spawn( | | {
        for i in 1 .. 10 { // [ 1, 10 ) => [1,9]
            println!("hi number {} from the spawned thread!", i );
            thread::sleep( Duration::from_millis(1) );
        }
    });

    // TODO :
    // Why    call unwrap() ???
    spawn_thread_handle.join().unwrap();

    // main thread
    for i in 1 .. 5 { // [ 1, 5 ) => [1,4]
        println!("hi number {} from the main thread!", i );
        thread::sleep( Duration::from_millis(1) );
    }

}

fn test_4() 
{
    let v = vec![1,2,3];
    //                          move the varible v into the thread
    //          the thread take the ownership of v
    let handle = thread::spawn( move | | {
        // for i in 1 .. 99999999 {
            println!("Here's a vector: {:?}", v);
        // }
    });

    // may be the above thread will be alive for a period ( inside a long for-loop ) longer than the varible v's lifetime 
    // main thread    drop v  in advance of the above thread 
    /********** 
     drop(v); 
    **********/

    handle.join().unwrap();
}


fn main() {
    // test_1(); // uncomment this line will raise a bug during test_2() 
    println!();
    println!();
    test_2();
    println!();
    println!();
    test_3();
    println!();
    println!();
    test_4();

}




