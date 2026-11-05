use std::ops::Add;
use std::fmt;

#[derive(Debug, PartialEq)]
struct Point {
    x: i32,
    y: i32,
}

impl Add for Point {
    type Output = Point;

    // override the   +    operator
    fn add(self, other: Point) -> Point {
        Point {
            x: self.x + other.x,
            y: self.y + other.y,
        }
    }
}

#[derive(Debug, PartialEq)]
struct Millimeters(u32);

#[derive(Debug, PartialEq)]
struct Meters(u32);

impl Add<Meters> for Millimeters {
    type Output = Millimeters;

    //     self -> mm
    //                  other -> m
    fn add(self,        other: Meters) -> Millimeters {
        // return value = mm + m*1000
        Millimeters(self.0 + (other.0 * 1000) )
    }
}

trait Pilot  { fn fly(&self); }
trait Wizard { fn fly(&self); }
struct Human;

impl Pilot  for Human { fn fly(&self) { println!("Do Pilot::fly()");  } }
impl Wizard for Human { fn fly(&self) { println!("Do Wizard::fly()"); } }
impl Human            { fn fly(&self) { println!("Do Human::fly()");  } }

// Notes:         baby_name() is a function without any parameters 
//                        ( no self pointer specified )
trait Animal { fn baby_name() -> String; }
struct Dog;
impl Dog            { fn baby_name() -> String { String::from("Spot")  } }
impl Animal for Dog { fn baby_name() -> String { String::from("puppy") } }


//                :  dependency trait list seperate by  ','
trait OutlinePrint: fmt::Display {
    fn outline_print(&self) {
        // self must impl   'to_string' trait 
        //   ( because 'to_string' is defined inside  trait fmt::Display ) 
        let output = self.to_string();
        let len    = output.len();
        println!("{}", "*".repeat(len+4) );
        println!("*{}*", " ".repeat(len+2) );
        println!("* {} *",  output);
        println!("*{}*", " ".repeat(len+2) );
        println!("{}", "*".repeat(len+4) );
    }
}

impl OutlinePrint for Point { }
impl fmt::Display for Point { 
    fn fmt(&self, f:&mut fmt::Formatter) -> fmt::Result {
        write!(f, "({}, {})", self.x, self.y)
    }
}

/********************************************************************************************************
 
    I want to impl the trait 'display' for Vec
    But vec and Display are kinds of Standard Libary , 
    i have no right to impl that trait


    So i wrap vec into struct Wrapper ( with tuple )

********************************************************************************************************/
struct Wrapper(Vec<String>); // tuple struct

impl fmt::Display for Wrapper {
    fn fmt(&self, f:&mut fmt::Formatter) -> fmt::Result {
        write!(f, "[{}]", self.0.join(", ") )
    }
}



pub trait Iterator {
    type Item;

    fn next(&mut self) -> Option<Self::Item>;
}

pub trait Iterator2<T> {
    fn next(&mut self) -> Option<T>;
}

struct Counter {}
impl Iterator for Counter {
    type Item = u32;

    fn next(&mut self) -> Option<Self::Item> { None }
}

impl Iterator2<String> for Counter { fn next(&mut self) -> Option<String> { None } }
impl Iterator2<u32>    for Counter { fn next(&mut self) -> Option<u32>    { None } }





fn main() {
    assert_eq!(  Point { x:1, y:2 }
               + Point { x:3, y:4 },
                 Point { x:4, y:6 } ); // if x != 4   or y!=6   run-time assert occurs


    // mm + m   ->   mm
    let mm1: Millimeters = Millimeters(43);
    let  m1: Meters      = Meters(2);
    assert_eq!( mm1 + m1, Millimeters(2043)  ); 

    let person = Human;
    person.fly();         // can  Human::fly()
    Pilot::fly(&person);
    Wizard::fly(&person);


    //                               Spot
    println!("1. A baby dog is called a {}",              Dog::baby_name() );
    //                                 puppy
    println!("2. A baby dog is called a {}",  <Dog as Animal>::baby_name() );

    let pt: Point = Point{ x: 123, y:456 };
    pt.outline_print();
    //////////////////////////////////////////////////////
    // outline_print like the following : 
    /*

        **************
        *            *
        * (123, 456) *
        *            *
        **************
    
    */
    ////////////////////////////////////////////////////


    let w = Wrapper( vec![ String::from("hello"), String::from("world") ] );
    // w = [hello, world]
    println!("w = {}", w);

}




