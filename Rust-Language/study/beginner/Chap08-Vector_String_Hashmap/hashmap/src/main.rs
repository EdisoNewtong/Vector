// HashMap is not in prelude
// So you must import the 'use' instruction before use
use std::collections::HashMap;

/****************************************************************************************************

error[E0282]: type annotations needed for `HashMap<K, V>`
 --> src\main.rs:24:9
   |
24 |     let mut kv = HashMap::new();
   |         ^^^^^^   -------------- type must be known at this point
   |
help: consider giving `kv` an explicit type, where the type for type parameter `K` is specified
   |
24 |     let mut kv: HashMap<K, V> = HashMap::new();
   |               +++++++++++++++

****************************************************************************************************/

fn my_hashmap_1()
{
    println!("------- my_hashmap_1 -------");
    // [ERROR] @ Compile time 
    let mut kv                   = HashMap::new();
    // to fix it , use the following code
  //let mut kv: HashMap<i32,i32> = HashMap::new();
}

fn my_hashmap_2()
{
    println!("------- my_hashmap_2 -------");
    let mut kv                   = HashMap::new();
    kv.insert( 3,5 );
    kv.insert( 4,6 );
}

fn my_hashmap_3()
{
    println!("------- my_hashmap_3 -------");
                      //             10                    50
    let teams = vec![ String::from("Blue"), String::from("Yello"), ];
    let initial_score = vec![10, 50];

    //
    //
    //                          wrap team with it's score  into a tuple with 2 elements 
    //                                           then collect all tuples into a HashMap
    //Notes:    
    //          HashMap<_,_>  is a most , otherwise compile error
    //
    //  the unerderline of HashMap<_,_>  
    //      means  that the data-type need to be decuded by the compiler.
    let scores: HashMap<_,_> = teams.iter().zip( initial_score.iter() ).collect();
}


fn my_hashmap_4()
{
    println!("------- my_hashmap_4 -------");
    let k = String::from("China");
    let v = String::from("Beijing");

    let mut map = HashMap::new();
    // do move operator on varible 'k' && 'v' if the varible doesn't implement 'copy' trait
    map.insert(k,v);
    // after that k && v will be droped because k && v's data type is String which doesn't implement 'copy' trait

    // [ERROR] @ Compile time
    println!("key={}, value={}", k,v);
    
}

fn my_hashmap_5()
{
    println!("------- my_hashmap_5 -------");

    let k = String::from("China");
    let v = String::from("Beijing");

    let mut map = HashMap::new();
    // do borrowing without take its owership
    map.insert(&k,&v);

    // Compile PASSED   :)
    println!("key={}, value={}", k,v);
}



fn my_hashmap_6()
{
    println!("------- my_hashmap_6 -------");
    let mut scores = HashMap::new();

    // team           Blue vs Yellow 
    // the score is     10  :  50
    scores.insert( String::from("Blue"), 10 );
    scores.insert( String::from("Yellow"), 50 );

    //
    // How to get the value by the given key
    //
    let team_name  = String::from("Blue");
    let team_score = scores.get(&team_name);

    match team_score {
        Some(correspond_score) => println!("team {} get score : {}", team_name, correspond_score),
        None                   => println!("team {} is not found ", team_name),
    };

    //
    // How to get travelsal hashmap
    // for-loop will generate an anonymous tuple like (k,v) from the given hashmap's pair data type
    for (k,v) in &scores {
        println!("team '{}' get score : {}", k,v);
    }

}


fn my_hashmap_7()
{
    println!("------- my_hashmap_7 -------");
    let mut scores = HashMap::new();

    scores.insert( String::from("Blue"), 10 );
    scores.insert( String::from("Blue"), 50 );  // update Blue's score from 10 to 50

    println!("{:?}", scores); // yield { "Blue" : 50 }
}

fn my_hashmap_8()
{
    println!("------- my_hashmap_8 -------");
    let mut scores = HashMap::new();

    scores.insert( String::from("Blue"), 10 );

    let e = scores.entry( String::from("Yellow") ); // yield Entry( VacantEntry ("Yellow") )
    e.or_insert( 50 ); // set 50 only if e is vacant , otherwise do nothing

    // Do nothing because there already exists a corresponding value of Blue , it's 10
    scores.entry( String::from("Blue") ).or_insert( 50 ); // do not update from 10 ==> 50 

    println!("{:?}", scores); // yield { "Blue" : 50 }
}



fn my_hashmap_9()
{
    println!("------- my_hashmap_9 -------");

    let text = "hello world wonderful world";

    let mut map = HashMap::new();

    for word in text.split_whitespace() {
        let count = map.entry(word).or_insert(0); // the return value is a kind of mutable  &i32
        *count += 1;
    }


    /**************************************************
     
        {
            "wonderful": 1,   // only 1 time
            "hello": 1,       // only 1 time
            "world": 2,       // appear 2 times
        }

    **************************************************/
    println!("{:#?}", map);
}

fn main() 
{
    my_hashmap_1();

    println!();
    my_hashmap_2();

    println!();
    my_hashmap_3();

    println!();
    my_hashmap_4();

    println!();
    my_hashmap_5();

    println!();
    my_hashmap_6();

    println!();
    my_hashmap_7();

    println!();
    my_hashmap_8();

    println!();
    my_hashmap_9();
}



