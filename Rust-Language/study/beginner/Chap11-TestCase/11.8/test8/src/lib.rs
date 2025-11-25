pub fn add(left: i32, right: i32) -> i32 {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }


    #[test]
    #[ignore] // be used for ignore this test case
    fn expensive_test() {
        let mut result: i32 = 0;
        for cnt in 1..=999999 {
            result = result + add(2,2);
            println!("{}. result = {}", cnt, result);
        }
        assert_eq!(result, 4*999999);
    }

}



