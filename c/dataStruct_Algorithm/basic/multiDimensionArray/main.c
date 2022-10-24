#include <stdio.h>


extern void static2DArraySample();
extern void static3DArraySample();

extern void dynamic2DArraySample();
extern void dynamic3DArraySample();


void test2D()
{
    static2DArraySample();
    printf("\n");
    dynamic2DArraySample();
    printf("\n\n");
}

void test3D()
{
    static3DArraySample();
    printf("\n");
    dynamic3DArraySample();
    printf("\n\n");
}



int main(int argc, char* argv[])
{
    printf("----------------------------------------------------------------------------------------------------\n");
    test2D();
    printf("\n----------------------------------------------------------------------------------------------------\n\n");

    test3D();
    printf("\n----------------------------------------------------------------------------------------------------\n");

    return 0;
}


