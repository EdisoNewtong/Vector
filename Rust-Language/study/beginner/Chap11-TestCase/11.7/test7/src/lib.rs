pub fn add_2(n: i32) -> i32 {
    n + 2
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn add_one_and_two() {
        assert_eq!(3, add_2(1) );
    }

    #[test]
    fn add_three_and_two() {
        assert_eq!(5, add_2(3) );
    }


    #[test]
    fn one_hundred() {
        assert_eq!(102, add_2(100) );
    }


}
