//////////////////////////////////////////////////
// 
// Use Result<T,E> 
//   as a result of a test function 
// 
//////////////////////////////////////////////////
#[cfg(test)]
mod tests {
    // use super::*;

    #[test]
    fn it_works() -> Result<(), String> {
        if 2+2 == 4 {
            Ok(())
        } else {
            Err( String::from("2 + 2 != 4") )
        }
    }

    #[test]
    // if the return type is Result<T,E>
    //     attribute :
    // #[should_panic] should never existed before the test function
    fn it_does_not_works() -> Result<(), String> {
        if 2+2 == 5 {
            Ok(())
        } else {
            Err( String::from("2 + 2 != 5") )
        }
    }
}
