pub fn add_2(a: i32) -> i32 {
    a + 2
}

pub fn add_3(a: i32) -> i32 {
    a + 3
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add_2(2);
        assert_eq!(result, 4);
    }
}
