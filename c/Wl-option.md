[引用 -Wl 选项说明](https://www.cnblogs.com/yongdaimi/p/16619264.html)

# GCC-G++中的-Wl选项说明 
  在GCC编译程序时，由于GCC命令不经能够编译，也能够链接程序，GCC链接程序是通过ld命令实现的， 
如何将GCC的命令行参数传递给ld命令呢，这就是通过**==-Wl==**,来实现的。 

格式如下： 
```bash
gcc -Wl,param1,param2,param3,... 
```


在调用ld命令时，会等效于如下： 
```bash
ld param1 param2 param3 
```

注意：-Wl, 后的==逗号==（，）必不可少，如果要传递多个参数，参数间用，分隔

-Wl,**==-Bstatic==** 
指定后续ld在处理 -l 参数来链接库文件的链接方式首选**==静态库==**

-Wl,**==-Bdynamic==**
指定后续ld在处理 -l 参数来链接库文件的链接方式首选**==动态库==**

例如：

**-Wl,-Bstatic -l<font color="red">a</font> -l<font color="red">b</font> -l<font color="red">c</font>                                  -Wl,-Bdynamic -l<font color="#FF00FF">d</font>  -l <font color="#FF00FF">e\*\*</font>**
会被解释成 ：ld   <font color="blue">lib</font><font color="red">a</font>.a   <font color="blue">lib</font><font color="red">b</font>.a    <font color="blue">lib</font><font color="red">c</font>.a                                 <font color="blue">lib</font><font color="#FF00FF">d</font>.so   <font color="blue">lib</font><font color="#FF00FF">e</font>\*\*.so

**-Wl,-rpath -Wl,/data/workroom/libs/lib** 
会被解释成：ld -rpath /data/workroom/libs/lib
