
fn iterator_study1() {
    let v1 = vec![1,2,3];
    // the following statement has Zero-Cost at runtime
    let v1_iter = v1.iter();
}

fn iterator_study2() {
    let v1 = vec![101,202,303];
    let v1_iter = v1.iter(); // the following statement will take cost inside the for-loop
    // Notes : Why the for-loop doesn't need to qulify the v1_iter by 'mut' keyword
    //  Because the for-loop take the ownership of the varible v1_iter , instead the tests mod doesn't 
    for val in v1_iter {
        println!("Got: {}", val);
    }
}


fn main() {
    iterator_study1();
    iterator_study2();
}


#[cfg(test)]
mod tests {
    #[test]
    fn iterator_demonstration() {
        let v1 = vec![1, 2, 3];
        let mut v1_iter = v1.iter();

/****************************************************************************************************
error[E0308]: mismatched types
  --> src\main.rs:73:37
   |
73 |         assert_eq!( v1_iter.next(), Some(1) );
   |                                     ^^^^^^^ expected `Option<&{integer}>`, found `Option<{integer}>`
   |
   = note: expected enum `Option<&{integer}>`
              found enum `Option<{integer}>`
help: try using `.as_ref()` to convert `Option<{integer}>` to `Option<&{integer}>`
   |
73 |         assert_eq!( v1_iter.next(), Some(1).as_ref() );
   |                                            +++++++++

error[E0308]: mismatched types
  --> src\main.rs:74:37
   |
74 |         assert_eq!( v1_iter.next(), Some(2) );
   |                                     ^^^^^^^ expected `Option<&{integer}>`, found `Option<{integer}>`
   |
   = note: expected enum `Option<&{integer}>`
              found enum `Option<{integer}>`
help: try using `.as_ref()` to convert `Option<{integer}>` to `Option<&{integer}>`
   |
74 |         assert_eq!( v1_iter.next(), Some(2).as_ref() );
   |                                            +++++++++

error[E0308]: mismatched types
  --> src\main.rs:75:37
   |
75 |         assert_eq!( v1_iter.next(), Some(3) );
   |                                     ^^^^^^^ expected `Option<&{integer}>`, found `Option<{integer}>`
   |
   = note: expected enum `Option<&{integer}>`
              found enum `Option<{integer}>`
help: try using `.as_ref()` to convert `Option<{integer}>` to `Option<&{integer}>`
   |
75 |         assert_eq!( v1_iter.next(), Some(3).as_ref() );
   |                                            +++++++++
*****************************************************************************************************/
        //                          Some(&1) rather than Some(1) => otherwise Compile-time Error 
        assert_eq!( v1_iter.next(), Some(&1) );
        assert_eq!( v1_iter.next(), Some(&2) );
        assert_eq!( v1_iter.next(), Some(&3) );
    }

    #[test]
    fn iterator_sum() {
        let v1         = vec![1, 2, 3];
        let v1_iter    = v1.iter();
        // 'v1_iter' with 'sum' function   
        //           will consume each element until all elements has been swallowed
        let total: i32 = v1_iter.sum(); 

        assert_eq!(total, 6);
    }

    #[test]
    fn iterator_map() {
        let v1: Vec<i32> = vec![1,2,3];
        // !!![Note]!!! : iterators are lazy and do nothing unless consumed
        // after the statement has been executed , 
        //     v1 will not changed to [2,3,4]
        v1.iter().map( |x| x+1 );


        // collect is a kind of consumed function , it take effect 
        let v2: Vec<_> = v1.iter().map( |x| x+1 ).collect();

        assert_eq!(v1, vec![1,2,3]); // collect will not change v1's element value
        assert_eq!(v2, vec![2,3,4]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#[derive(PartialEq, Debug)]
struct Shoe {
    size: u32,
    style: String,
}

fn shoes_fit_my_size(shoes: Vec<Shoe>, shoe_size: u32) -> Vec<Shoe> {
    // if closure return true => keep the element 
    //    otherwise drop(kick) it out
    shoes.into_iter().filter( |x| x.size == shoe_size).collect()
}


#[test]
fn filter_by_size() {
    let shoes = vec![
        Shoe { size: 10, style: String::from("Nike"), },
        Shoe { size: 13, style: String::from("Addidas"), },
        Shoe { size: 10, style: String::from("Li-Ning"), },
    ];
    let fit_my_size = shoes_fit_my_size(shoes, 10);

    assert_eq!(fit_my_size, vec![
        Shoe { size: 10, style: String::from("Nike"), },
        Shoe { size: 10, style: String::from("Li-Ning"), },
    ] );
}
////////////////////////////////////////////////////////////////////////////////////////////////////



struct Counter {
    count: u32,
}

impl Counter {
    fn new() -> Counter {
        Counter { count: 0 }
    }
}


impl Iterator for Counter {
    type Item = u32;

    fn next(&mut self) -> Option<Self::Item> {
        // begin from  0 => yield 1
        if self.count < 5 {
            self.count += 1;

            // finally when self.count == 4    
            //         =>  yield   5
            Some(self.count)
        } else {

            None
        }
    }
}

#[test]
fn calling_next_directly() {
    let mut counter = Counter::new();

    assert_eq!( counter.next(), Some(1) );
    assert_eq!( counter.next(), Some(2) );
    assert_eq!( counter.next(), Some(3) );
    assert_eq!( counter.next(), Some(4) );
    assert_eq!( counter.next(), Some(5) );
    assert_eq!( counter.next(), None );
}

#[test]
fn using_other_iterator_trait_methods() {
    /************************************************
             [start from 2] so skip(1)
        1 *    2                   = 2
        2 *    3                   = 6   ( match 6%3 == 0 )
        3 *    4                   = 12  ( match 12%3 == 0 )
        4 *    5                   = 20
        5 *    None                // drop it
    
       so (6+12) = 18
    ************************************************/


    let sum: u32 = Counter::new()
                   .zip( Counter::new().skip(1) ) // zip will wrap 2 element into 1 group
                   .map( | (a,b) |   a*b)     
                   .filter(|x| x % 3 == 0)
                   .sum();

    assert_eq!(18, sum);
}
