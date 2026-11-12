# Chap 7 : Managing Growing Projects with Packages, Crates, and Modules 
## Core Conceptions 
As a project grows, you should organize code by splitting it into multiple modules and then multiple files.
==**A package**== can contain 
  ==**multiple**== binary crates + ==**optionally one**== library crate 

- Packages        : A Cargo feature that lets you build, test, and share crates
- Crates          : A tree of modules that produces a library or executable
- Modules and use : Let you control the organization, scope, and privacy of paths
- Paths           : A way of naming an item, such as a struct, function, or module

## Packages and Crates 
### Crate 
  A crate is the ==**smallest**== amount of code that the Rust compiler considers at a time. 
Even if you run rustc rather than cargo and pass a single source code file, 
the ==**compiler**== considers that ==**file**== to be a ==**crate**==. 
  Crates can contain ==**modules**==, and the modules may be defined in other files that get compiled with the crate, 
as we’ll see in the coming sections. 

  A crate can come in one of two forms: 
- a ==**binary**==  crate ( with fn main() { ... } )   or  ( generate a xxx.exe )
- a ==**library**== crate.                                 ( generate a xxx.rlib ) 

  Most of the time when Rustaceans say “crate”, 
they mean ==**library**== crate, and they use “crate” interchangeably with the general programming concept of a “library".
The ==**crate root**== is a source file that the Rust compiler starts from and makes up the root module of your crate

There are 2 convention ==**crate roots**== : 
    1. main.rs  
    1. lib.rs


### Package 
  A package is a bundle of ==**one or more crates**== that provides a set of functionality. 
A package contains a ==**Cargo.toml**== file that describes how to build those crates. 
  A package can contain as ==**many binary crates**== as you like, 
but at most ==**only one**== library crate. 
  A package must contain ==**at least one**== crate, whether that’s a library or binary crate.

### Cargo 
  Cargo follows a ==**convention**== that src/main.rs is the ==**crate root**== of a binary crate with the same name as the package. 
Likewise, Cargo knows that if the package directory contains src/lib.rs, the package contains a library crate with the same name as the package, 
and ==**src/lib.rs**== is its ==**crate root**==.  
  Cargo passes the crate root files to rustc to build the library or binary.


# Defining Modules to Control Scope and Privacy 
How to use   'use' / 'pub' / 'as' keyword

## Modules Cheat Sheet 
==**The Rules**== : 
- Start from the crate root: 
	When compiling a crate, the compiler ==**first**== looks in the ==**crate root**== file (usually ==**src/lib.rs**== for a library crate or ==**src/main.rs**== for a binary crate) for code to compile.
- Declaring modules: 
	In the ==**crate root**== file, you can declare new modules; say, you declare a ==**"garden" module**== with ==**mod garden;**== . 
	The compiler will look for the module’s code in these places:
		- Inline, within curly brackets that replace the semicolon following mod garden
		- In the file ==**src/garden.rs**==
		- In the file ==**src/garden/mod.rs**==

