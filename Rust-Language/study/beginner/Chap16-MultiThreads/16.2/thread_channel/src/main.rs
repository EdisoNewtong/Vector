use std::sync::mpsc;
use std::time::Duration;
use std::thread;

fn test_1()
{
    // tx : transmit X
    // rx : receive  X
    let (tx, rx) = mpsc::channel();
    
    thread::spawn( move | | {
        let val = String::from("say 'hi' to main-thread. ");
        tx.send(val).unwrap();
    });

    // Notes :
    // recv() will block the calling thread ( main-thread) until the message has reached the destination ( the received endpoint of the channel )
    let received = rx.recv().unwrap();
    println!("Got: {}", received);

}

fn test_2()
{
    // tx : transmit X
    // rx : receive  X
    let (tx, rx) = mpsc::channel();
    
    thread::spawn( move | | {
        let val = String::from("say 'hi' to main-thread. ");
        tx.send(val).unwrap(); // val has been moved outside
                               //
                               //
        // append a new statement after send(val)
        // Compile-Time ERROR  
        /********************************************
        println!("in Spawn-thread,  val = {}", val);
                                               ^^^   value borrowed here  after move 
        ********************************************/
    });

    // Notes :
    // recv() will block the calling thread ( main-thread) until the message has reached the destination ( the received endpoint of the channel )
    let received = rx.recv().unwrap();
    println!("Got: {}", received);

}

fn test_3()
{
    // tx : transmit X
    // rx : receive  X
    let (tx, rx) = mpsc::channel();
    
    thread::spawn( move | | {
        let vals = vec![
            String::from("hi"),
            String::from("from"),
            String::from("the"),
            String::from("thread"),
        ];

        for val in vals {
            tx.send(val).unwrap(); 
            thread::sleep( Duration::from_millis(200) );
        }
    });

    // Notes :
    // recv() will block the calling thread ( main-thread) until the message has reached the destination ( the received endpoint of the channel )
    for received in rx {
        println!("Got: {}", received);
    }

}

fn test_4()
{
    // tx : transmit X
    // rx : receive  X
    let (tx, rx) = mpsc::channel();

    let tx1 = mpsc::Sender::clone(&tx);
    // thread #1 : use the cloned tx1 to send
    thread::spawn( move | | {
        let vals = vec![
            String::from("tx #1 : hi"),
            String::from("tx #1 : from"),
            String::from("tx #1 : the"),
            String::from("tx #1 : thread"),
        ];

        for val in vals {
            tx1.send(val).unwrap(); 
            thread::sleep( Duration::from_millis(200) );
        }
    });

    // thread #2 : use tx to send
    thread::spawn( move | | {
        let vals = vec![
            String::from("tx    : hi"),
            String::from("tx    : from"),
            String::from("tx    : the"),
            String::from("tx    : thread"),
        ];

        for val in vals {
            tx.send(val).unwrap(); 
            thread::sleep( Duration::from_millis(200) );
        }
    });

    // Notes :
    // recv() will block the calling thread ( main-thread) until the message has reached the destination ( the received endpoint of the channel )
    //
    // the messages received will be output alternatively ( You can't control the order of coming messages  )
    /*********************************************************
            tx #1 : hi
            tx    : hi
            tx #1 : from
            tx    : from
            tx #1 : the
            tx    : the
            tx #1 : thread
            tx    : thread
    **********************************************************/
    for received in rx {
        println!("Got: {}", received);
    }

}

fn main() {
    test_1();
    println!();
    println!();
    test_2();
    println!();
    println!();
    test_3();
    println!();
    test_4();
}



