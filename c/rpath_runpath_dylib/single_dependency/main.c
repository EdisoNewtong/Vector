#include <stdio.h>

extern int myOp(int n1, int n2);

int main(int argc, char* argv[], char* env[])
{
    int a,b,ret;

    a = 17;
    b = 4;
    ret = myOp(a,b);
    printf("%d / %d = %d\n", a,b, ret);

    return 0;
}


