#include <stdio.h>

extern int myadd(int a,int b,int c,int d);

int main(int argc, char* argv[])
{
    int ret = myadd(1,2,3,4);
    printf("myadd(1,2,3,4) = %d\n", ret );
    return 0;
}



