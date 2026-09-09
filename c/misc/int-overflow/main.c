#include <stdio.h>

/***********************************************************************
unsigned short a = 0xFFFF; // ( = 65535 ( in decimal form ) ) 
++a; //   a = 0;   not 65536

a = 0;
--a;  // ( a != -1    , a  = 65535 )
***********************************************************************/

void int_overflow_loop()
{
	const int everyN_newLine = 64;
	// const unsigned short TEST_MAX = 256;
	const unsigned short TEST_MAX = 0xFFFF;

	/***********************************************************
	  Output from 65535(included) to 0(included)
    ***********************************************************/
	int cnt = 0;
    unsigned short num; 
	num = TEST_MAX;
	do {
		printf("%d ", num);
		if ( ++cnt % everyN_newLine == 0 ) {
			printf("\n");
		}
	} while( num-- >  0 ); 
	//       num-- >= 0     // will raise  int circuity ( 是迂回  not overflow )  

	/***********************************************************
	printf("\n\n");
	// 256 ~ 1 ( 0 is not printed )
	num = TEST_MAX;
	while( num > 0 )
	{
		printf("%d ", num--);
		if ( ++cnt % everyN_newLine == 0 ) {
			printf("\n");
		}
	}
    ***********************************************************/

	printf("\n\n");
	// 256 ~ 1 ( 0 is not printed )
	/***********************************************************
	for( num = TEST_MAX; num > 0; ) {
		printf("%d ", num--);
		if ( ++cnt % everyN_newLine == 0 ) {
			printf("\n");
		}
	}
    ***********************************************************/


	/***********************************************************

    Conclusion :
	        do {} while ( conditon );         [ It Does a GOOD Job ]
			while( conditon ) {}              [ FAILED ]
			for( ... ; ... ; ... ) {}         [ FAILED ]

    ***********************************************************/
}


int main(int argc, char* argv[], char* env[])
{
    int_overflow_loop();
	return 0;
}






