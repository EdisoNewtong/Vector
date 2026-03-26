use std::cell::RefCell;
use std::rc::{ Rc, Weak };

#[derive(Debug)]
enum List {
    Cons(i32, RefCell<Rc<List>>),
    Nil,
}

use crate::List::{ Cons, Nil };

impl List {
    fn tail(&self) -> Option<&RefCell<Rc<List>>> {
        match self {
            //
            // if self has next-link field , return it's next field
            //
            Cons(_, item) => Some(item),
            // Otherwise return  Nil
            Nil => None,
        }
    }
}



fn ref_cycle_case()
{
    //  a:   5 -> NULL
    let a = Rc::new( Cons(5, RefCell::new(Rc::new(Nil))));
    println!("1st. a.rc = {}", Rc::strong_count(&a) );
    println!("1st. a.next = {:?}", a.tail() );

    //  b:   10 ->       {  list_a  }
    //                        ↓
    //       10 ->       { 5 -> nil }
    let b = Rc::new( Cons(10, RefCell::new(Rc::clone(&a))));
    println!("2nd. a.rc count after b creation = {}", Rc::strong_count(&a) );
    println!("2nd. b.rc = {}", Rc::strong_count(&b) );
    println!("2nd. b.next = {:?}", b.tail() );

    /*****************************************************************
    
    let Some(link) = a.tail();
    if link {   // link is not None
        println!("in if   clause");
    } else {
        println!("in else clause");
    }
    
    *****************************************************************/
    //////////////////////////////////////////////////////////////////////////////////////
    //
    //  b:   10 ->       {  list_a -> list_b_itself                               }
    //                        ↓
    //       10 ->       { 5 -> { 10 -> 5 -> 10 -> 5 -> 10 -> 5 -> 10 -> 5 -> ... } // infinite reference cycle
    //
    //////////////////////////////////////////////////////////////////////////////////////
    if let Some(link) = a.tail() {
        *link.borrow_mut() = Rc::clone(&b); // ++b.rc;  // b.rc from 1 to 2 
    }
    let statement: &str = r#"
3rd. after execute the following statement : 
/////////////////////////////////////////
   if let Some(link) = a.tail() {
       *link.borrow_mut() = Rc::clone(&b);
   }
/////////////////////////////////////////
"#;

    println!("--------------------------------------------------");
    println!("{}", statement);
    println!("3rd. b.rc after changing a = {}", Rc::strong_count(&b) );
    println!("3rd. a.rc after changing a = {}", Rc::strong_count(&a) );
    //
    //  a and b will never be collected 
    //    due to their's reference counting 
    //  will keep its value on 1 rather than 0 
    //
    println!("--------------------------------------------------");
    //////////////////////////////////////////////////////////////////////////
    //
    // Uncomment the next line to see that we have a cycle;
    // it will overflow the stack.
    /*
            a next item = Some(RefCell { value: Cons(10, RefCell { value: Cons(5, RefCell { value: Cons(10, RefCell { value:
        Cons(5, RefCell { value: Cons(10, RefCell { value: Cons(5, RefCell { value: Cons(10, RefCell { value: Cons(5, RefCell 
        { value: Cons(10, RefCell { value: Cons(5, RefCell 

                            ...     
                            ...     
                            ...     

       Cons(5, RefCell { value: Cons(10, RefCell { value: Cons(5, RefCell { value: Cons(10, RefCell { value: Cons(5, R
      efCell { value: Cons(10, RefCell { value: Cons(5, RefCell { value: Cons(10, RefCell { value: Cons(5, RefCell { v
      alue: Cons(10, RefCell { value: Cons(5, RefCell { value: Cons(10, RefCell { value: Cons(5, RefCell { value: Cons
      (10, RefCell { value: Cons(5, RefCell { value: Cons(10, RefCell { value: Cons(5, RefCell { value: Cons(10, RefCe
      ll { value: Cons(5, RefCell { value: Cons(10, RefCell {
      
      thread 'main' has overflowed its stack
      error: process didn't exit successfully: `target\debug\ref_cycle.exe` (exit code: 0xc00000fd, STATUS_STACK_OVERF
      LOW)

    */
    // println!("a next item = {:?}", a.tail() );

}

#[derive(Debug)]
// #[warn(dead_code)]
struct TreeNode {
    value: i32,
    parent: RefCell<Weak<TreeNode>>,
    children: RefCell<Vec<Rc<TreeNode>>>,
}


fn build_tree_case()
{
    let leaf = Rc::new( TreeNode {
        value: 3,
        parent: RefCell::new( Weak::new() ),
        children: RefCell::new( vec![] ),
    });
    //                                                  upgrade()  : Weak<T> --> Rc<T>
    // println!("0. leaf.parent = {:?}", leaf.parent.borrow().upgrade() );
    println!("0. leaf.strong_rc = {}, weak_rc = {}", Rc::strong_count(&leaf), Rc::weak_count(&leaf),);

    
    {
        let branch = Rc::new( TreeNode {
            value: 5,
            parent: RefCell::new( Weak::new() ),
            children: RefCell::new(vec![Rc::clone(&leaf)]),
        });


        //                              downgrade(...)    :   Rc<TreeNode> -> Weak<TreeNode>
        *leaf.parent.borrow_mut() = Rc::downgrade(&branch);
        // println!("1. leaf.parent = {:?}", leaf.parent.borrow().upgrade() );
        // println!("\n--------------------------------------------------");
        // println!("--------------------------------------------------\n");


        println!("1. branch.strong_rc = {}, weak_rc = {}", Rc::strong_count(&branch), Rc::weak_count(&branch) );
        println!("1. leaf.strong_rc = {}, weak_rc = {}", Rc::strong_count(&leaf), Rc::weak_count(&leaf) );
    }

    // println!();
    println!("2. leaf.parent = {:?}", leaf.parent.borrow().upgrade() );
    println!("2. leaf.strong_rc = {}, weak_rc = {}", Rc::strong_count(&leaf), Rc::weak_count(&leaf) );
}



fn main() {
    ref_cycle_case();
    println!();
    build_tree_case();
}




