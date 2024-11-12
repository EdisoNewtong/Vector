#include <stdio.h>
#include <stdlib.h>

#include "myAdd.h"


int main(int argc, char* argv[])
{
    int ret = my_add(1,2);
    printf("1+2 = 3, my_add(1,2) = %d. Why the 2 values are not same ?\n"
           "Because the implementation of function int my_add(a,b) is very special.\n"
           ,ret);

    return 0;
}



