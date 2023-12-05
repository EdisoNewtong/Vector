# How to use Cargo to create a project  

```Bash Shell  
cargo new hello-cargo
```  


it will generate the following file tree :

hello-cargo                   // Directory
            -- Cargo.toml     // '.toml' stand for   Tom’s Obvious, Minimal Language  
            -- src            // Directory
                  -- main.rs



# How to use Cargo to build a Cargo Project

```Bash Shell  
cd hello-cargo
cargo build
```  

```Bash Shell  
cargo build --release
```  


# How to use Cargo to run the program


```Bash Shell  
cargo run
```  

Cargo also provides a command called cargo check. This command quickly checks your code to make sure it compiles but doesn’t produce an executable:

```Bash Shell  
cargo check
```  
