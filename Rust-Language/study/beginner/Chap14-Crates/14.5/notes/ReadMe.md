# 一个工作空间中有多个项目(crates)
- RustWorkspace
-- my_main             # enter in my_main ; execute : cargo build  ==> output dir is ../target    rather than   creating a new dir inside pwd     "./target"
-- add_one
-- add_two
-- target              # 3 crates share one build output target directory
-- Cargo.lock          # 3 crates share one Cargo.toml
-- Cargo.toml          # Config file for RustWorkspace which include 3 crates one binary crate : "my_main" and 2 library crates : "add_one", "add_two" , my_main depend on "add_one" and "add_two" 


