# What is ownership ? 
Rust uses a third approach: memory is managed through a system of ownership with a set of rules that the compiler checks. 
If any of the rules are violated, the program won’t compile. None of the features of ownership will slow down your program while it’s running.

## Stack and Heap
All data stored on the stack must have **==a known, fixed size==**. 
Data with an unknown size at compile time or a size that might change must be stored on the heap instead 

Because the pointer to the heap is a known, fixed size, 
you can store the pointer on the stack, but when you want the actual data, you must follow the pointer.

Think of being seated at a restaurant. 
When you enter, you state the number of people in your group, 
and the host finds an empty table that fits everyone and leads you there. 
If someone in your group comes late, they can ask where you’ve been seated to find you.

Accessing data in the ==heap== is **slower than** accessing data on the ==stack==
because you have to follow a pointer to get there.

Continuing the analogy, consider a server at a restaurant taking orders from many tables. 
It’s most efficient to get all the orders at one table before moving on to the next table. 
Taking an order from table A, then an order from table B, then one from A again, and then one from B again 
would be a much slower process.


## Ownership Rules 

**==Keep these rules in mind==** as we work through the examples that illustrate them :

- Each value in Rust has an owner.
- There can only be one owner at a time.
- When the owner goes out of scope, the value will be dropped.



```rust
{           // s is not valid here, it’s not yet declared
        let s = "hello";   // s is valid from this point forward

        // do stuff with s
}           // this scope is now over, and s is no longer valid
```


In other words, there are two important points in time here:

- When s comes into scope, it is valid.
- It remains valid until it goes out of scope.

There is a natural point at which we can return the memory our String needs to the allocator: when s goes out of scope. 
When a variable goes out of scope, Rust calls a special function for us. 
This function is called drop, and it’s where the author of String can put the code to return the memory. 
Rust calls drop automatically at the closing curly bracket.


> Note: In C++, this pattern of deallocating resources at the end of an item’s lifetime is 
> sometimes called Resource Acquisition Is Initialization (RAII). The drop function in Rust will be 
> familiar to you if you’ve used RAII patterns.


```rust
    let s1 = String::from("hello");
    let s2 = s1;

    // the following statement will raise a compile error
    println!("{}, world!", s1);
```



```bash
$ cargo run
   Compiling ownership v0.1.0 (file:///projects/ownership)
error[E0382]: borrow of moved value: `s1`
 --> src/main.rs:5:28
  |
2 |     let s1 = String::from("hello");
  |         -- move occurs because `s1` has type `String`, which does not implement the `Copy` trait
3 |     let s2 = s1;
  |              -- value moved here
4 |
5 |     println!("{}, world!", s1);
  |                            ^^ value borrowed here after move
  |
  = note: this error originates in the macro `$crate::format_args_nl` which comes from the expansion of the macro `println` (in Nightly builds, run with -Z macro-backtrace for more info)
help: consider cloning the value if the performance cost is acceptable
  |
3 |     let s2 = s1.clone();
  |                ++++++++

For more information about this error, try `rustc --explain E0382`.
error: could not compile `ownership` due to previous error
```

Here is the correct version

```rust

    let s1 = String::from("hello");
    let s2 = s1.clone();

    // compile passed
    println!("s1 = {}, s2 = {}", s1, s2);
```



# References and Borrowing

```rust

fn main() {
    let s1 = String::from("hello");
    let (s2, len) = calculate_length(s1);
    println!("The length of '{}' is {}.", s2, len);
}

// give the ownership to the caller
fn calculate_length(s: String) -> (String, usize) {
    let length = s.len(); // len() returns the length of a String
    (s, length)
}


```


```rust
fn main() {
    let s1 = String::from("hello");
    // borrowing s1
    let len = calculate_length(&s1);
}


fn calculate_length(s: &String) -> usize {
    s.len()
}
```


## Mutable References

```rust
fn main() {
    let mut s = String::from("hello");
    change(&mut s);
}

fn change(some_string: &mut String) {
    some_string.push_str(", world");
}
```


Mutable references have **==one big restriction==**: 
if you have a mutable reference to a value, you can have **==no other==** references to that value. 
This code that attempts to create two mutable references to s will fail:

```rust
    let mut s = String::from("hello");

    let r1 = &mut s;
    let r2 = &mut s;

    println!("{}, {}", r1, r2);
```

```bash
$ cargo run
   Compiling ownership v0.1.0 (file:///projects/ownership)
error[E0499]: cannot borrow `s` as mutable more than once at a time
 --> src/main.rs:5:14
  |
4 |     let r1 = &mut s;
  |              ------ first mutable borrow occurs here
5 |     let r2 = &mut s;
  |              ^^^^^^ second mutable borrow occurs here
6 |
7 |     println!("{}, {}", r1, r2);
  |                        -- first borrow later used here

For more information about this error, try `rustc --explain E0499`.
error: could not compile `ownership` due to previous error
```

Rust enforces a similar rule for combining mutable and immutable references. 
This code results in an error:

```rust

    let mut s = String::from("hello");

    let r1 = &s; // no problem , immutable borrow
    let r2 = &s; // no problem , immutable borrow
    let r3 = &mut s; // BIG PROBLEM

    println!("{}, {}, and {}", r1, r2, r3);

```

Here’s the error:
```bash
$ cargo run
   Compiling ownership v0.1.0 (file:///projects/ownership)
error[E0502]: cannot borrow `s` as mutable because it is also borrowed as immutable
 --> src/main.rs:6:14
  |
4 |     let r1 = &s; // no problem , immutable borrow
  |              -- immutable borrow occurs here
5 |     let r2 = &s; // no problem , immutable borrow
6 |     let r3 = &mut s; // BIG PROBLEM
  |              ^^^^^^ mutable borrow occurs here
7 |
8 |     println!("{}, {}, and {}", r1, r2, r3);
  |                                -- immutable borrow later used here

For more information about this error, try `rustc --explain E0502`.
error: could not compile `ownership` due to previous error
```
Whew! We also **==cannot==** have **a mutable** reference while we have **an immutable** one to the same value.


Note that a reference’s scope starts from where it is introduced and continues through the last time that reference is used.
```rust
    let mut s = String::from("hello");

    let r1 = &s; // no problem , immutable reference
    let r2 = &s; // no problem , immutable reference
    println!("{} and {}", r1, r2);
    // variables r1 and r2 will not be used after this point

    let r3 = &mut s; // no problem
    println!("{}", r3);
```

Let’s recap what we’ve discussed about references:

- At any given time, you can have either one mutable reference or any number of immutable references.
- References must always be valid.


