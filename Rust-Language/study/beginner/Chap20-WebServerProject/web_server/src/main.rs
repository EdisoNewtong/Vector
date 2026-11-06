use std::fs;
use std::io::prelude::*;

use std::thread;
use std::time::Duration;

use std::net::TcpListener;
use std::net::TcpStream;

use web_server::ThreadPool;


fn main() {
    // static mut CON_CNT: u32 = 0 as u32;

    let listener = TcpListener::bind("localhost:12800").unwrap();
    // 编译器驱动开发的模式 : 先把希望调用的函数或者类型给写上(无论对错)
    // 然后根据编译器的错误再逐步完善代码
    let pool = ThreadPool::new(4);

    let mut times: u32 = 0 as u32;
    println!("----------------------------------------------------------------------------------------------------");
    for stream in listener.incoming() {
        let stream = stream.unwrap();

        times = times + 1;
        // println!("Connection {} established!", times);

        pool.execute( | | {
            handle_connection(stream);
        });

        // thread::spawn( | | {
        //     handle_connection(stream);
        // });
    }
}

fn handle_connection(mut stream: TcpStream) {
    //         there are 512 numbers in a buffer and all numbers are 0.
    let mut buffer = [0; 512];

    stream.read(&mut buffer).unwrap();

    // Version #1 :
    /* 
    println!("Request: {}", String::from_utf8_lossy(&buffer[ .. ]) );
    println!("----------------------------------------------------------------------------------------------------");
    */
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //
    // Request: GET / HTTP/1.1
    // Host: localhost:7878
    // Connection: keep-alive
    // Cache-Control: max-age=0
    // sec-ch-ua: "Not:A-Brand";v="8", "Chromium";v="156", "Google Chrome";v="156"
    // sec-ch-ua-mobile: ?0
    // sec-ch-ua-platform: "Windows"
    // Upgrade-Insecure-Requests: 1
    // User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/156.0.0.0 Safari/537.36
    // Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/jxl,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-excha
    //
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    /****************************************************************************************************
     
       request:
             Method Request-URI  HTTP-Version  CRCF
             headers CRLF
             message-body

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
       response:
             HTTP-Version Status-Code Reason-Phrase CRLF
             headers CRLF
             message-body
             
    ****************************************************************************************************/
    //                              \r\n      => response format  1st line EOL tag
    //                                  \r\n  => move to the 2nd line
    // Version #2 :
    /* 
    let response = "HTTP/1.1 200 OK \r\n\r\n";

    stream.write( response.as_bytes() ).unwrap();
    stream.flush().unwrap(); // will wait until all bytes has been written into the stream 
    */

    // Version #3 :
    // println!("c = {}",contents);
    //      'b' stands for the type byte string's prefix
    let get   = b"GET / HTTP/1.1\r\n";
    let sleep = b"GET /sleep HTTP/1.1\r\n";

    let (status_line, filename) = if buffer.starts_with(get) {
        // println!("in if ");
        ("HTTP/1.1 200 OK\r\n\r\n" , "hello_rust.html")
    } else if buffer.starts_with(sleep) {
        thread::sleep( Duration::from_secs(5) );
        ("HTTP/1.1 200 OK\r\n\r\n" , "hello_rust.html")
    } else {
        // println!("in else");
        ("HTTP/1.1 404 NOT FOUND\r\n\r\n" , "404.html")
    };

    let contents = fs::read_to_string(filename).unwrap();
    let response = format!("{}{}", status_line, contents);

    stream.write( response.as_bytes() ).unwrap();
    stream.flush().unwrap(); // will wait until all bytes has been written into the stream 


}




