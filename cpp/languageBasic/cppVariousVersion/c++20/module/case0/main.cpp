import HelloModule;

import <iostream>;

using namespace std;

int main(int argc, char* argv[], char* env[])
{
    // use module's exported varible
    cout << "varible_PI          = " << varible_PI << endl;
    varible_PI *= 2.0;
    cout << "varible_PI * 2      = " << varible_PI << endl;

    cout << "const_varible_PI    = " << const_varible_PI << endl;
    /*
    main.cpp:15:22: 错误：向只读变量‘const_varible_PI@HelloModule’赋值
       21 |     const_varible_PI *= 2.0;
          |     ~~~~~~~~~~~~~~~~~^~~~~~

    */
    // const_varible_PI *= 2.0;
    // cout << "const_varible_PI * 2 = " << const_varible_PI << endl;



    // use module's exported function
    int ret1 = my_pow(2,10);
    cout << "use my_pow(2,10)    : 2 ^ 10 = " << ret1 << endl;

    /*
    main.cpp:24:16: 错误：‘innerUtil’在此作用域中尚未声明
       35 |     int ret2 = innerUtil(2,10);
          |                ^~~~~~~~~
    */
    // int ret2 = innerUtil(2,10);
    // cout << "use innerUtil(2,10) : 2 ^ 10 = " << ret2 << endl;
    

    return 0;
}

