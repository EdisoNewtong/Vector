# Package Crate Module 
- Package (包)    the 1st Level  
    Q : How to create a pakcage ? 
    A : $ cargo new  <package-name> ( project-name )
        src/main.rs 就是项目的入口文件 

- Crate (板条箱)  the 2nd Level ( 它一定是 目录名 ( 或者项目名, 项目也是以目录的形式存在于文件系统中的 ) )
- Module (模块)   the 3rd Level ( 在代码内容中创建  mod <mod_name> { fn help_util1() { ... } fn help_util2() { }  }


