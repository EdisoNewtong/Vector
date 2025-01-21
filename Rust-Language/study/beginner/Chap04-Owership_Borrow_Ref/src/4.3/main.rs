// 
// "Slice" (切片)  is a kind of Data Type which doesn't take ownership of an object
//

/******************************************************************************************************
Question : implement a function to return the first word of the given string
           if there is no space in the given string , return the entire string


This function has some shortage that it return an index from the given string , the index's validation depend on the string's content
once the string's content is changed , the index will be expired immediately . the index strongly rely on the context of string 


*******************************************************************************************************/


fn first_word_v1(s: &String) -> usize 
{
    let bytes: &[u8] = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return i;
        }

    }

    s.len()
}

/******************************************************************************************************

error[E0502]: cannot borrow `s` as mutable because it is also borrowed as immutable
  --> main.rs:84:5
   |
82 |     let word_index = first_word_v2(&s); // it will yield 5
   |                                    -- immutable borrow occurs here
83 |
84 |     s.clear(); // world_index will lose its meaning of the first word , the word_index will still keep its value for the upcomming use
   |     ^^^^^^^^^ mutable borrow occurs here
85 |     println!("word index = {}", word_index);
   |                                 ---------- immutable borrow later used here

error: aborting due to previous error

For more information about this error, try `rustc --explain E0502`.

******************************************************************************************************/
fn first_word_v2(s: &String) -> &str 
{
    let bytes: &[u8] = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[ .. i];
        }

    }

    &s[ .. ]
}


fn ex1()
{
    println!("---------- Get the first word's length of a given string ----------");
    let mut s = String::from("Hello world");
    let word_index = first_word_v1(&s); // it will yield 5

    s.clear(); // world_index will lose its meaning of the first word , the word_index will still keep its value for the upcomming use
    println!("word index = {}", word_index);
}





fn ex2_compile_error()
{
    println!("---------- Get the first word's length of a gvien string ----------");
    let mut s = String::from("Hello world");
    let word_index = first_word_v2(&s); // it will yield 5

    s.clear(); // world_index will lose its meaning of the first word , the word_index will still keep its value for the upcomming use
    println!("word index = {}", word_index);
}


/******************************************************************************************************
slice is a part of a given String
                                      ****** after-end-index ******
the format is &str_name[start-index .. after-end-index]
there has some syntactic sugar of the slice index
       zero can be omit
    &s[0 .. end]


             index-after-last-char can be omit
    &s[ 3 .. index-after-last-char]

    &s[ .. ]  => the entire string

******************************************************************************************************/
fn slice_example()
{
    println!("---------- Slice example  ----------");
    let s = String::from("Hello world");

    // [ start-index ~ last-index )
    //             0 -> h     5 -> the space between world
    let str_hello1: &str = &s[0       .. 5];
    let str_hello2: &str = &s[       .. 5];
    //             6 -> w     11 -> the dummy char after 'd'
    let str_world1: &str = &s[6       .. 11];
    let str_world2: &str = &s[6       .. ];

    let str_entire1: &str = &s[0       .. s.len()];
    let str_entire2: &str = &s[ .. ];

    println!("str_hello1 = \"{}\"", str_hello1);
    println!("str_hello2 = \"{}\"", str_hello2);
    println!("str_world1 = \"{}\"", str_world1);
    println!("str_world2 = \"{}\"", str_world2);
    println!("str_entire1 = \"{}\"", str_entire1);
    println!("str_entire2 = \"{}\"", str_entire2);
}

/******************************************************************************************************

Notes: the slice index can not locate in the middle of the given utf-8 string
e.g


我 : E6 88 91 
们 : E4 BB AC 
let s: String = String::from("我们");

let s_me:        &str = &s[0 .. 3];
let s_me_error1: &str = &s[0 .. 2]; // ERROR
let s_me_error2: &str = &s[0 .. 4]; // ERROR

*******************************************************************************************************/
fn ex_slice_for_utf8_error()
{
    println!("---------- incorrect slice in the middle of a given utf8 string ----------");

    let s: String = String::from("我们");

    let s_me:        &str = &s[0 .. 3];
    let s_me_error1: &str = &s[0 .. 2]; // Runtime ERROR
    let s_me_error2: &str = &s[0 .. 4]; // Runtime ERROR
    println!("s_me = \"{}\"", s_me);
    println!("s_me_error1 = \"{}\"", s_me_error1);
    println!("s_me_error2 = \"{}\"", s_me_error2);
}


// fn first_word_v3(s: &String) -> &str 
fn first_word_v3(s: &str)    -> &str 
{
    let bytes: &[u8] = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[ .. i];
        }

    }

    &s[ .. ]
}

fn ex3()
{
    println!("---------- Get the first word #V3 of a given string ----------");
    let my_string = String::from("Hello world"); // a kind of String
  //let word1 = first_word_v3(my_string); //  compiler will do implicit conversion from String -> to &str 
    let word1 = first_word_v3(&my_string[ .. ]);

    let my_string_literal = String::from("Hello world"); // a kind of &str
    let word2 = first_word_v3(&my_string_literal);
    println!("word1 = \"{}\", word2 = \"{}\"", word1, word2);
}

fn array_slice()
{
    println!("---------- example for a traditional array ----------");
    let array = [1,2,3,4,5]; // an i32 array  =>  array's type is   [i32;5]
    let slice = &array[1..3]; // a slice of the array  => &[i32]    [2,3]
    println!("slice = {:?}", slice);
}

fn main()
{
    ex1();
    println!();
    slice_example();
    println!();
    ex2_compile_error();
    println!();
    ex_slice_for_utf8_error();
    println!();
    ex3();
    println!();
    array_slice();

}




