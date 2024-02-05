#include <stdio.h>


//
// !!!Must!!! Add prefix declared statement Here
//            Otherwise compile Error
//
extern int my_add(int,int);
extern int my_minus(int,int);



int main(int argc, char* argv[])
{
	int a = 100;
	int b = 7;
	int addRet   = my_add(a,b);
	int addMinus = my_minus(a,b);
    printf("%d + %d = %d\n", a,b, addRet);
    printf("%d - %d = %d\n", a,b, addMinus);

	return 0;
}

