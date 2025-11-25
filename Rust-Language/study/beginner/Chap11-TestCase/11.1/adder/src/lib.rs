#[cfg(test)]
mod tests {
    /**************************************************
       Attribute marked with #[test] is a test function 

       Use the following command line : 
       $ cargo test 

    ***************************************************/
    #[test]
    fn test_func_2() {
        // let result = add(2, 2);

        /**************************************************************
           the print content will not be shown on the output window
        ***************************************************************/
        println!("inside function test_successful()");
        assert_eq!(2+2, 4);
    }

    #[test]
    fn test_func_1() {
        // let result = add(2, 2);
        println!("inside function test_failed()");
        assert_eq!(2+3, 4);
    }
}


