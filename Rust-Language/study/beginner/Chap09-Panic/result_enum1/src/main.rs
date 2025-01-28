use std::fs::File;
use std::io::Error;

use std::io::ErrorKind;

fn result_1()
{
    println!("----- in fn result_1() -----");
    // File    => std::fs::File
    // Error   => std::io::Error
    let f: Result<File,Error> = File::open("hello.txt");

    match f {
        Ok(_opened_file) => println!("Found hello.txt :)"),   // if the file "hello.txt" existed in the same layer of Cargo.toml , 
                                                                                    //  error msg : 系统找不到指定的文件。 (os error 2)
        Err(error_msg)   => println!("\"hello.txt\" is not existed or be opened failed. error msg : {}",error_msg), // else execute this line
    };
}


fn result_2()
{
    println!("----- in fn result_2() -----");
    let f: Result<File,Error> = File::open("hello.txt");

    let fobj = match f {
        Ok(file) => file,
                                                                                    //  error msg : 系统找不到指定的文件。 (os error 2)
        Err(error) =>  match error.kind() {
            // referenced here  <== std::io::ErrorKind
            ErrorKind::NotFound => match File::create("hello.txt") {
                Ok(fc) => fc,
                Err(e) => panic!("Error createing file: {:?}", e),
            },

            // oe is OK , it's a user-defined varible name
            other_error => panic!("Error opening the file: {:?}", other_error),
        },
    };
}

fn result_3()
{
    println!("----- in fn result_3() -----");

    //--------------------------------------------------
    //       Rust closure grammer 
    //--------------------------------------------------
    //
    /*****************************************************
        |error| {
            panic!("Error creating file: {:?}", error);
        } 
    ******************************************************/

    let f = File::open("hello.txt").unwrap_or_else( |error| {
        if error.kind() == ErrorKind::NotFound {
            File::create("hello.txt").unwrap_or_else( |error| {
                panic!("Error creating file: {:?}", error);
            } )
        } else {
            panic!("Error opening file: {:?}", error);
        }
    });
}


fn result_4()
{
    println!("----- in fn result_4() -----");

    /***********************************************
    //  
    //  code #1     by 'match' keyword
    // 
    let f = File::open("hello.txt");


    let f_obj = match f {
        Ok(file)   => file,
        Err(error) => {
            panic!("Error opening file {:?}", error);
        }
    };

    ************************************************/

    //
    // unwrap()  is a simple style of   <match expression>
    //
    //    But there the shortage of method 'unwrap' 
    // that you can't do user-defined code while you have got the error object 
    //
    let f_obj1 = File::open("hello.txt").unwrap();

    //
    // the member function 'expect' is capable to do that. 
    //
    let f_obj2 = File::open("hello.txt").expect("User-defined code Here : the program can't open file \"hello.txt\"");

}



fn main() 
{
    result_1();

    println!();
    result_2();

    println!();
    result_3();

    println!();
    result_4();
}



