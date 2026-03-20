use add_one;

pub fn add_two(n: i32) -> i32 {
    add_one::add_one(n) + 1
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add_two(5, 2);
        assert_eq!(result, 7);
    }
}
