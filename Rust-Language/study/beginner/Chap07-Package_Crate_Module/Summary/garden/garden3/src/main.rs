//////////////////////////////////////////////////
// main.rs
//////////////////////////////////////////////////

// the module named 'garden'  is implemented in other source file

// Compile Passed #7 && #8
mod garden;

// use crate::garden::garden_a::water_flowers;  // Compile Passed #7
use           garden::garden_a::water_flowers;  // Compile Passed #8

fn main() {
                        water_flowers(1.2);  // Compile Passed #7 
    //garden::garden_a::water_flowers(1.5);  // Compile Passed #8
    //        garden_a::water_flowers(1.8);  // Compile [ERROR] this line
}

