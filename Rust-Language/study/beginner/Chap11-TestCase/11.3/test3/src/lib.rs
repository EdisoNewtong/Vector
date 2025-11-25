pub fn greeting(name: &str) -> String {
    format!("Hello {}!", name)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn greetings_contain_name() {
        let result = greeting("Carol");
        assert!( result.contains("Carol") );
    }


    #[test]
    fn greetings_contain_name_with_error_msg() {
        let result = greeting("Cerol");
        assert!( result.contains("Carol"), 
            "greeting didn't contains word 'Carol', greeting = \"{}\"",  
            result);
    }
}


