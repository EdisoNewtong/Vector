# Shell Command Examples 

==**To Search the same string with different search option( Case Sensitive / Case Insensitive )**==


```bash
$ cargo run to poem.txt
   Compiling minigrep v0.1.0 (E:\work\src\github\myrepo\Vector\Rust-Language\study\beginner\Chap12-CmdArgs_Envs\12.7\minigrep)
    Finished dev [unoptimized + debuginfo] target(s) in 0.30s
     Running `target\debug\minigrep.exe to poem.txt`
Are you nobody, too?
How dreary to be somebody!
```



```bash
$ CASE_INSENSITIVE=1 cargo  run to poem.txt
    Finished dev [unoptimized + debuginfo] target(s) in 0.00s
     Running `target\debug\minigrep.exe to poem.txt`
Are you nobody, too?
How dreary to be somebody!
To tell your name the livelong day
To an admiring bog!
```

