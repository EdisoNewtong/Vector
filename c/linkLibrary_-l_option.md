# About gcc -L / -l option
- **==-L==** option special the library search path : 
    If you have more than 2 search paths to be specified
    Please use the following syntax : -LPath1 -LPath2 -LPath3

    ```bash
    #############################################
    #
    # -lmyUtil => the library's name is   libmyUtil.(a|so)  
    # search the library   libmyUtil.a  or  libmyUtil.so  in the order of  
    #    Path1
    #    Path2 
    #    Path3 
    #
    # Notes there is no space between -L and Path1. 
    #        it's similar to the rule of -I option  : "-IIncPath1  -IIncPath2"
    #
    #############################################
    $ gcc -Wall -o main main.c  -LPath1 -LPath2 -LPath2   -lmyUtil
    ```


- **==-l==** option special the library to be linked : 
    -lmyUtil      => the linker need a library file whose name is either  libmyUtil.a   or libmyUtil.so

    //
    // **==Break the Restriction==** that the library to be linked must has a prefix "lib"
    //
    // 
    //**==:==**    The colon symbol is a seperator to specify the complete library name
    **==\-l:myUtil.a==**  => tell the linker explicitly that the library file's name is   myUtil.a  rather than libmyUtil.(a|so)


