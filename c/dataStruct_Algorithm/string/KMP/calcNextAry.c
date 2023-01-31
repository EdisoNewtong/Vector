#include <stdio.h>
#include <stdlib.h>


void get_Next_Ary( const char* str, int* nextAry, int aryLen)
{
    int i = 0;
    int k = -1;
    nextAry[0] = -1;

    while ( i < (aryLen-1) )
    {
        if ( k == -1    ||    str[i] == str[k] ) {
            ++i;
            ++k;
            nextAry[i] = k;
        } else {
            k = nextAry[k];
        }
    }


}


void get_NextVal_Ary( const char* src, int* nextValAry, int aryLen)
{
    int i = 0;
    int k = -1;
    nextValAry[0] = -1;

    while ( i < aryLen )
    {
        if ( k == -1    ||    src[i] == src[k] ) {
            ++i;
            ++k;
            if ( src[i] != src[k] ) {
                nextValAry[i] = k;
            } else {
                nextValAry[i] = nextValAry[k];
            }
        } else {
            k = nextValAry[k];
        }
    }
}





int main(int argc, char* argv[], char* env[])
{
    if ( argc < 2 ) {
        printf("[ERROR] : Missing input string to calc Next-Array and  NextValue-Array\n");
        return -1;
    }

    char* inputStr = argv[1];
    int len = 0;
    while ( inputStr[len] != '\0' ) {
        ++len;
    }

    printf("\"%s\" length = %d\n", inputStr, len);
    int* nextAry    = (int*)malloc( len * sizeof(int) );
    int* nextValAry = (int*)malloc( len * sizeof(int) );



    get_Next_Ary( inputStr, nextAry, len);
    get_NextVal_Ary( inputStr, nextValAry, len);

    printf("\tNext Array : \n");
    printf("--------------------------------------------------\n");

    for( int i = 0; i < len; ++i ) { printf("%-3d ", i); }
    printf("\n--------------------------------------------------\n");
    for( int i = 0; i < len; ++i ) { printf("%c   ", inputStr[i] ); }
    printf("\n--------------------------------------------------\n");
    for( int i = 0; i < len; ++i ) { printf("%-3d ", nextAry[i] ); }


    printf("\n\n\n");




    printf("\tNextValue Array : \n");
    printf("--------------------------------------------------\n");

    for( int i = 0; i < len; ++i ) { printf("%-3d ", i); }
    printf("\n--------------------------------------------------\n");
    for( int i = 0; i < len; ++i ) { printf("%c   ", inputStr[i] ); }
    printf("\n--------------------------------------------------\n");
    for( int i = 0; i < len; ++i ) { printf("%-3d ", nextValAry[i] ); }
    printf("\n");



    free(nextAry);    nextAry = NULL;
    free(nextValAry); nextValAry = NULL;
    return 0;

}

