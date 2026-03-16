# -fPIC 选项 
## 单独一个 dll 的情况
这是一个**==链接==**时的选项，而**==非==** **==编译==**时期的选项

```bash
$ gcc -Wall -c        -o util_1.o util.cpp   
$ gcc -Wall -c -fPIC  -o util_2.o util.cpp

#  => 2 objects files are binaryly identical !!
$ diff util_1.o util_2.o                       
```

## 2个及以上dll,而且存在 依赖关系 时
**==但是==** ， 当一个 DLL 依赖 另一个 DLL 时，情况就**==完全不一样==**了

Tool  depends on  "math"

那么此时，在编译 Tool 时，就需要在编译期间 ，添加 -fPIC 选项了

```bash
$ gcc -Wall -c math.cpp
$ gcc -Wall -shared -fPIC -o libmath.so    math.o

$ gcc -Wall           -fPIC -c tool.cpp
$ gcc -Wall   -shared -fPIC -o libtool.so    tool.o  

```


