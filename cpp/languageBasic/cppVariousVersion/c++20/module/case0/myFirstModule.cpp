export module HelloModule;

export              double       varible_PI = 3.14;
export extern const double const_varible_PI = 3.14;

/*
myFirstModule.cpp: In function ‘int my_pow(int, int)’:
myFirstModule.cpp:11:12: 错误：‘innerUtil’在此作用域中尚未声明
   18 |     return innerUtil(base, times);
      |            ^~~~~~~~~

*/
// must declare the function prototype before calling it
int innerUtil(int base, int times);

export int my_pow(int base, int times)
{
    return innerUtil(base, times);
}


int innerUtil(int base, int times)
{
    int ret = 1;
    for( auto i = 0; i < times; ++i ) {
        ret *= base;
    }
    return ret;
}

