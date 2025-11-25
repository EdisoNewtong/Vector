#[derive(Debug)]
pub struct Rectangle {
    length: u32,
    width: u32,
}

impl Rectangle {
    pub fn can_hold(&self, other: &Rectangle) -> bool {
        self.length > other.length && self.width > other.width
    }
}

pub fn add_2(a: i32) -> i32 {
    a + 2
}




#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn large_can_hold_smaller() {
        let larger  = Rectangle { length: 8, width: 7, };
        let smaller = Rectangle { length: 5, width: 1, };

        ///////////////////////////////////////////////////////////
        //
        // if bool expression is true , Test [Passed] :)
        //                            , otherwise Test [Failed] !
        //
        ///////////////////////////////////////////////////////////
        assert!( larger.can_hold(&smaller) );
    }

    #[test]
    fn smaller_cannot_hold_larger() {
        let larger  = Rectangle { length: 8, width: 7, };
        let smaller = Rectangle { length: 5, width: 1, };

        assert!( !smaller.can_hold(&larger) );
    }

    #[test]
    fn if_adds_two_test1() {
        assert_eq!(4,  add_2(2) );
    }

    #[test]
    fn if_adds_two_test2() {
        // FAILED
        assert_eq!(5,  add_2(2) );
    }

    #[test]
    fn if_adds_two_test3() {
        assert_ne!(5,  add_2(2) );
    }
}
