#include <stdio.h>
#include <stdlib.h>

#define N1   3
#define N2   5
#define N3   7

void static2DArraySample()
{
    /*
    const int N1 = 3;
    const int N2 = 5;
    const int N3 = 7;
    */

    printf("========== in in static-2D-ArraySample ( %d  *  %d )  ==========\n", N1, N2);
    int ary2D[N1][N2] = {
        {  1,  2,  3,  4,  5 },
        {  6,  7,  8,  9, 10 },
        { 11, 12, 13, 14, 15 }
    };

    printf("staticAry2D = ");
    printf("{\n");
    for( int i = 0; i < N1; ++i ) {
        printf("  { ");
        for( int j = 0; j < N2; ++j ) {
            printf("%2d%s", ary2D[i][j], j<N2-1 ? ", " : " ");
        }
        printf("}%s\n", i<N1-1 ? ", " : " ");
    }
    printf("};\n");

    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("Sample code for pointing to an 1-D <Static-Fixed> array : ...\n");
    /* Core Core Core :
         The following statement is equalvant to the uncomment statement.  */
/*  int (*p1DAry)[N2] = (int (*)[N2])( ary2D + 1 );   */
/*  int (*p1DAry)[N2] = (int (*)[N2])( &ary2D[1] );   */
    int (*p1DAry)[N2] =              ( &ary2D[1] ); 
    printf("ary2D[1]  ->  { ");
    for( int j = 0; j < N2; ++j ) {
        printf("%2d%s", (*p1DAry)[j],  (j < N2-1 ? ", " : "")  );
    }
    printf(" };\n ");

}



void static3DArraySample()
{
    /*
    const int N1 = 3;
    const int N2 = 5;
    const int N3 = 7;
    */

    int studyCase = 0;

    printf("========== in in static-3D-ArraySample ( %d  *  %d  *  %d )  ==========\n", N1, N2, N3);

    int ary3D[N1][N2][N3] = {
        {
           {   1,     2,    3,    4,    5,    6,    7   },      
           {   8,     9,   10,   11,   12,   13,   14   },      
           {   15,   16,   17,   18,   19,   20,   21   },      
           {   22,   23,   24,   25,   26,   27,   28   },      
           {   29,   30,   31,   32,   33,   34,   35   }
        },

        {
           {   36,   37,   38,   39,   40,   41,   42   },
           {   43,   44,   45,   46,   47,   48,   49   },
           {   50,   51,   52,   53,   54,   55,   56   },
           {   57,   58,   59,   60,   61,   62,   63   },
           {   64,   65,   66,   67,   68,   69,   70   }
        },

        {
           {   71,    72,    73,    74,    75,    76,    77   },
           {   78,    79,    80,    81,    82,    83,    84   },
           {   85,    86,    87,    88,    89,    90,    91   },
           {   92,    93,    94,    95,    96,    97,    98   },
           {   99,   100,   101,   102,   103,   104,   105   }
        }
    };

    printf("staticAry3D = {\n");
    for( int i = 0; i < N1; ++i ) {
        printf("  {\n");
        for( int j = 0; j < N2; ++j ) {
            printf("    {");
            for( int k = 0; k < N3; ++k ) {
                printf("%3d%s", ary3D[i][j][k], ( k < N3-1 ? ", " : " ") );
            }
            printf(" }%s\n", j < N2-1 ? ", " : "");
        }
        printf("  }%s\n", i < N1-1 ? ", " : "");
    }
    printf("};\n");



    if ( studyCase ) {
        /**************************************************
         
              The correct  data type for the given expression
             
         **************************************************/
        int (*p3DAry)[N1][N2][N3] = &ary3D;
        int (*p2DAry_1)[N2][N3]   =  ary3D;
        int (*p2DAry_2)[N2][N3]   = &ary3D[0];

        int (*p1DAry_1)[N3]   =  ary3D[0];
        int (*p1DAry_2)[N3]   = &ary3D[0][0];
        int* pElement         = ary3D[2][0];   // pointer to the number : 71

        int (**pp1DAry)[N3]   = &p1DAry_1;  // a pointer that point to a 1D array's pointer

        // printf("0. *pElement = %d\n", *pElement);
        // *pElement = 123;
        // printf("1. ary3D[2][0][0] = %d\n",  ary3D[2][0][0]); // output 123
        (void)p3DAry;
        (void)p2DAry_1;
        (void)p2DAry_2;
        (void)p1DAry_1;
        (void)p1DAry_2;
        (void)pElement;

        (void)pp1DAry;




        printf("---------- | Study Case of 3D Array address |----------\n");
        printf(" ary3D    = %p\n", ary3D);
        printf("&ary3D    = %p\n",&ary3D);
        printf("&ary3D[0] = %p\n",&ary3D[0]);

        if ( ((void*)ary3D) == &ary3D  && ary3D == &ary3D[0] ) {
            printf("in if, ary == ary3D == &ary3D[0] . \n");
        } else {
            printf("in else, ary3D != &ary3D != &ary3D[0] . \n");
        }

        int i = 0;
        int j = 0;
        printf("\n");
        for( i = 0; i < N1; ++i ) {
            printf("   ary3D[%d]    = %p\n", i,   ary3D[i]);
            printf("  &ary3D[%d][0] = %p\n", i,  &ary3D[i][0]);
            printf("  &ary3D[%d]    = %p\n", i,  &ary3D[i]);
            printf("\n");
        }

        printf("\n");
        for( i = 0; i < N1; ++i ) {
            for( j = 0; j < N2; ++j ) {
                printf("     ary3D[%d][%d]    = %p\n", i,j,   ary3D[i][j]);
                printf("    &ary3D[%d][%d][0] = %p\n", i,j,   &ary3D[i][j][0]);
                printf("    &ary3D[%d][%d]    = %p\n", i,j,   &ary3D[i][j]);
                printf("\n");
            }
            printf("\n");
        }


    }
    // return;






    /**************************************************

       Compare with line:160 in file dynamicArray.c

    ***************************************************/
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("Sample code for pointing to a 2-D <Static-Fixed> array : ...\n");
    int (*p2DAry)[N2][N3] = &ary3D[1];
    printf("ary3D[1]  ->  \n{\n");
    for( int j = 0; j < N2; ++j ) {
        printf("  {");
        /* type is    ( int (*)[N3] )   */
        
        /* avaliable exp 1 :            */
    /*  int (*p1DAry)[N3] = ( int (*)[N3] )( &( (*p2DAry)[j] ) ) ;  */

        /* avaliable exp 2 :            */        
     /* int (*p1DAry)[N3] = ( int (*)[N3] )( &( *(*p2DAry + j) ) ) ;  */

        /* avaliable exp 3 :            */        
        int (*p1DAry)[N3] = ( int (*)[N3] )( *p2DAry + j );
        for( int k = 0; k < N3; ++k ) {
            printf("%3d%s", (*p1DAry)[k],  (k < N3-1 ? ", " : " ")  );
        }
        printf("}%s\n", (j < N2-1 ? ", " : "") );
    }
    printf("};\n ");


}

