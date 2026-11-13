//////////////////////////////////////////////////
// util.rs
//////////////////////////////////////////////////
pub mod md_main {
    pub fn test() { println!("inside mod md_main,                                        call fn test()"); }

    pub mod md_nested_layer_0 {
        pub fn test() { println!("inside mod md_main::md_nested_layer_0,                     call fn test()"); }

        pub mod md_nested_layer_1 {
            pub fn test() { println!("inside mod md_main::md_nested_layer_0::md_nested_layer_1,  call fn test()"); }
        }
    }
}


