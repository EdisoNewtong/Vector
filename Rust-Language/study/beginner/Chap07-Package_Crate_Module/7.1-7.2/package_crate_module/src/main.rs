//
// here : package_crate_module  is the project name ( or the directory name which has been created by the command 'cargo new' ) 
// Both functions are defined in file 'lib.rs' which is the root of the crate in the package named 'package_crate_module' ( create from cargo new 'package_crate_module' )
// 
use package_crate_module::{ eat_at_restaurant, take_order_at_restaurant };

fn main() {
	eat_at_restaurant();
	take_order_at_restaurant();
}

