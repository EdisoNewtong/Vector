
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

        /*
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
        */
        //                          Some(&1) rather than Some(1) => otherwise Compile-time Error 
        assert_eq!( v1_iter.next(), Some(&1) );
        assert_eq!( v1_iter.next(), Some(&2) );
        assert_eq!( v1_iter.next(), Some(&3) );
    }
}
