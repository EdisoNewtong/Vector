//////////////////////////////////////////////////
// main.rs
//////////////////////////////////////////////////

// the module named 'garden'  is implemented in other source file

// Compile Passed #1 && #2
mod garden;

fn main() {
           garden::garden_a::water_flowers(1.2);  // Compile Passed #1 
    crate::garden::garden_a::water_flowers(1.2);  // Compile Passed #2
}


