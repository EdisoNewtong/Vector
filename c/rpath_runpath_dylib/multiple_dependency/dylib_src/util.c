#include <stdio.h>
extern int commonOp(int a,int b);
int myOp(int a,int b)
{
    printf("In [No Set] / libutil.so ... \n");
    return commonOp(a,b);
}
