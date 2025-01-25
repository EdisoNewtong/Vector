// import the source code file whose name is  front_of_house.rs
mod front_of_house;

pub use crate::front_of_house::hosting;

pub fn eat_at_restaurant()
{
    hosting::add_to_waitlist();
}

