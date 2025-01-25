
// use pub_use_multiple_use::hosting;   // uncomment this line at #3 condition will make compiler passed
fn main() {
/******************************************************************************************************
    Core Core Core :
  in 'lib.rs'

######################################################################################################################################################
[ Version #1

  neither 
     use crate::front_of_house
  nor
     pub use crate::front_of_house

]
error[E0433]: failed to resolve: use of undeclared crate or module `hosting`
  --> src\main.rs:60:2
   |
60 |     hosting::add_to_wait_list();
   |     ^^^^^^^ use of undeclared crate or module `hosting`

For more information about this error, try `rustc --explain E0433`.

######################################################################################################################################################
[ Version #2 : private use
     use crate::front_of_house
]

error[E0433]: failed to resolve: use of undeclared crate or module `hosting`
  --> src\main.rs:60:2
   |
60 |     hosting::add_to_wait_list();
   |     ^^^^^^^ use of undeclared crate or module `hosting`

For more information about this error, try `rustc --explain E0433`.

######################################################################################################################################################

[ Version #3 : pub use 
     pub use crate::front_of_house
]

error[E0433]: failed to resolve: use of undeclared crate or module `hosting`
  --> src\main.rs:60:2
   |
60 |     hosting::add_to_wait_list();
   |     ^^^^^^^ use of undeclared crate or module `hosting`
   |
help: consider importing this module
   |
3  + use pub_use_multiple_use::hosting;
   |

For more information about this error, try `rustc --explain E0433`.

*******************************************************************************************************/

    // uncomment line 2 at #3 condition will make compiler passed
    hosting::add_to_wait_list();

/******************************************************************************************************

     You'd better do not name a directory with '-' because it will lead to ambiguous  
       the directory name is : 'pub-use_multiple-use'
     what's is the exp pub-use_multiple-use::hosting   mean ?
           pub - use_multiple - use ?  // minus operator
      or   pub-use_multiple-use::hosting  // a crate name ?

*******************************************************************************************************/

}


