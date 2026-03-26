// terms :  test double (测试替代)
/*
    Sometimes during testing a programmer will use a type in place of another type, 
in order to observe particular behavior and assert it’s implemented correctly. 
This placeholder type is called a test double. 
Think of it in the sense of a “stunt double” in filmmaking, where a person steps in and substitutes for an actor to do a particular tricky scene. 
Test doubles stand in for other types when we’re running tests. 
Mock objects(模拟对象) are specific types of test doubles that record what happens during a test so you can assert that the correct actions took place.

*/
////////////////////////////////////////////////////////////////////////////////
pub trait Messenger {
    fn send(&self, msg: &str);
}

pub struct LimitTracker<'a, T: 'a + Messenger> {
    messenger: &'a T,
    value: usize,
    max: usize,
}

impl<'a, T> LimitTracker<'a, T>
where
      T: Messenger,
{
    pub fn new(messenger: &'a T, max: usize)  ->  LimitTracker<'a, T> {
        LimitTracker { messenger, value: 0, max, }
    }

    pub fn set_value(&mut self, value: usize) {
        self.value = value;

        let percentage_of_max = self.value as f64 / self.max as f64;
        if  percentage_of_max >= 1.0 {
            self.messenger.send("Error: You are over your quota!");
        } else if percentage_of_max >=0.9  {
            self.messenger.send("Urgent warning: You're used up over 90% of your quota.");
        } else if percentage_of_max >=0.75 {
            self.messenger.send("warning: You're used up over 75% of your quota.");
        }
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    use std::cell::RefCell;

    struct MockMessenger {
        // Compile time [ERROR]
        // sent_messages:         Vec<String>,
        sent_messages:    RefCell<Vec<String> >,
    }

    impl MockMessenger {
        fn new() -> MockMessenger {
            MockMessenger {
             // sent_messages:               vec![],
                sent_messages: RefCell::new( vec![] ),
            }
        }
    }

    impl Messenger for MockMessenger {
        fn send(&self, message: &str) {
         // self.sent_messages.push(String::from(message));
            self.sent_messages.borrow_mut().push(String::from(message) );
        }
    }

    #[test]
    fn it_sends_an_over_75_percent_warning_message() {
        let mock_messenger = MockMessenger::new();
        let mut limit_tracker = LimitTracker::new(&mock_messenger, 100);

        limit_tracker.set_value(80);

     // assert_eq!(mock_messenger.sent_messages.len(), 1);
        assert_eq!(mock_messenger.sent_messages.borrow().len(), 1);
    }
}


////////////////////////////////////////////////////////////////////////////////
fn test_1() 
{
    println!("---------- in test_1() ----------");
    let a = 3;

    // Rust设计理念 :  无法可变地 借用一个不可变的引用 
    // a是一个不可变的变量
    //    b 不能以可变的形式，借用a这个不可变的引用
    //
    // 否则 a 中保存的数值3, 什么时候发生了变化，a它自己都不知道!!!
    // 在a自己不知情的情况下，自己内部的数值发生了变化,  这是非常危险的操作
    //
    // [ERROR] at compile time
    // let b = &mut a;   // Compile [ERROR]
}

#[derive(Debug)]
enum List {
    Cons( Rc<RefCell<i32>>, Rc<List>),
    Nil,
}
    
use crate::List::{ Cons, Nil };
use std::rc::Rc;
use std::cell::RefCell;


fn test_2()
{
    println!("---------- in test_2() ----------");
    let value = Rc::new( RefCell::new(5) );
    let a     = Rc::new( Cons(Rc::clone(&value), Rc::new(Nil)));
    let b     = Cons( Rc::new(RefCell::new(6)),  Rc::clone(&a) );
    let c     = Cons( Rc::new(RefCell::new(10)), Rc::clone(&a) );

    // value.oldValue  =  5;
    // value          += 10;  => 5+10 => 15
    *value.borrow_mut() += 10;

    println!("a after = {:?}", a);
    println!("b after = {:?}", b);
    println!("c after = {:?}", c);
}

fn main() {
    test_1();
    println!();
    test_2();
}



