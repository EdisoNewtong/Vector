


fn main()
{
    let day: i32 = 2;
	match day {
		1 => { 
            print!("Today is ");
            println!("Monday.");
        },

		2 => println!("Today is Tuesday."),
		3 => println!("Today is Wednesday."),
		4 => println!("Today is Thrusday."),
		5 => println!("Today is Friday."),
		6 | 7 => println!("Today is Weekends."),
        _ => println!("day = {day}, What day is it today ?")
	}

}


