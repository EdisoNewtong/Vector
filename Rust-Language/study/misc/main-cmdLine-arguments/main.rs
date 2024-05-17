
use std::env;

fn main() {
    // Collect the command-line arguments into a vector
    let args: Vec<String> = env::args().collect();

    // The first argument is the name of the program
    println!("Program name: {}", args[0]);


	//
    // Print the rest of the arguments , index start from [1] rather than [0]
	//
    for (index, argument) in args.iter().enumerate().skip(1) {
        println!("Argument {}: {}", index, argument);
    }


}


