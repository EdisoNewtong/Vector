pub fn add_2(a: i32) -> i32 {
    a + 2
}

// private function declaration
fn internal_adder(a: i32, b: i32) -> i32 {
    a + b
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        // invoke interal (private) function to test its logic is true or not 
        assert_eq!(4, internal_adder(2,2) );
    }
}

