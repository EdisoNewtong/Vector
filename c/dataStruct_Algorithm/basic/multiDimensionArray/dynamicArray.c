#include <stdio.h>
#include <stdlib.h>


/**********************************************************************
   2D Array
***********************************************************************/
int** dynamicAlloc2DArray(int nRows, int nCols)
{
    int initNum = 1;
    int num = initNum;


    printf("Alloc a 2-D Array ......\n");
    /*
        or the following statement is Correct
    int** ret2DAry = (int**)malloc( sizeof(int*      ) * nRows ); */
    int** ret2DAry = (int**)malloc( sizeof(int[nCols]) * nRows ); /* Alloc ***All*** the  Rows Layers */
    for( int iRowIdx = 0; iRowIdx < nRows; ++iRowIdx ) {
        /* Alloc ***All*** the  columns  Layers */
        ret2DAry[iRowIdx] = (int*)malloc( sizeof(int) * nCols );

        /* assign elements value */
        for( int iColIdx = 0; iColIdx < nCols; ++iColIdx ) {
            ret2DAry[iRowIdx][iColIdx] = num++;
        }
    }

    return ret2DAry;

}

void dynamicFree2DArray(int** ary2d, int nRows)
{
    printf("Release a 2-D Array ......\n");
    for( int i = 0; i < nRows; ++i ) {
        free(ary2d[i]);
    }
    free(ary2d);
}


void dynamic2DArraySample()
{
    const int nRows = 3;
    const int nCols = 5;
    printf("========== in dynamic-2D-ArraySample ( %d  *  %d )  ==========\n", nRows, nCols);

    int** p2DAry = dynamicAlloc2DArray( nRows, nCols);
    printf("dynamic2DArray = {\n");
    for( int i = 0; i < nRows; ++i ) {
        printf("  { ");
        for( int j = 0; j < nCols; ++j ) {
            printf("%2d%s", p2DAry[i][j], ( j<(nCols-1) ? ", " : " ") );
        }
        printf("}%s\n", i<(nRows-1) ? "," : " ");
    }
    printf("};\n");



    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("Sample code for pointing to an 1-D <Dynamic-Alloced> array : ...\n");
    /* Core Core Core :
         The following statement is equalvant to the uncomment statement.  */
/*  int (*p1DAry)[nCols] = (int (*)[nCols])(  &p2DAry[1][0] );  */
    int (*p1DAry)[nCols] =  (int (*)[nCols])( p2DAry[1] );
    printf("ary2D[1]  ->  { ");
    for( int j = 0; j < nCols; ++j ) {
        printf("%2d%s", (*p1DAry)[j],  (j < nCols-1 ? ", " : "")  );
    }
    printf(" };\n\n");



    dynamicFree2DArray(p2DAry, nRows);
    p2DAry = NULL;
}


/**********************************************************************
   3D Array
***********************************************************************/
int*** dynamicAlloc3DArray(int n1D, int n2D, int n3D)
{
    int initNum = 1;
    int num = initNum;

    printf("Alloc a 3-D Array ......\n");
    /*   
        or the following statement is Correct
    int*** ret3DAry = (int***)malloc( sizeof(int**        ) * n1D ); */
    int*** ret3DAry = (int***)malloc( sizeof(int[n2D][n3D]) * n1D ); /* Alloc ***All***   the  `i`   Layers   */
    for( int i = 0; i < n1D; ++i ) {
        /*   
            or the following statement is Correct
        ret3DAry[i] = (int**)malloc( sizeof(  int*  ) * n2D ); */
        ret3DAry[i] = (int**)malloc( sizeof(int[n3D]) * n2D );  /* Alloc ***All***   the  `j`   Layers   */
        for( int j = 0; j < n2D; ++j ) {
            /* Alloc ***All***   the  `k`   Layers   */
            ret3DAry[i][j] = (int*)malloc( sizeof(int) * n3D );

            /* assign elements value */
            for( int k = 0; k < n3D; ++k ) {
                ret3DAry[i][j][k] = num++;
            }
        }
    }

    return ret3DAry;
}


void dynamicFree3DArray(int*** p3DAry, int n1D, int n2D)
{
    printf("Release a 3-D Array ......\n");
    for( int i = 0; i < n1D; ++i) {
        for( int j = 0; j < n2D; ++j) {
            free(p3DAry[i][j]);
        }
        free(p3DAry[i]);
    }

    free(p3DAry);
}


void dynamic3DArraySample()
{
    const int n1 = 3;
    const int n2 = 5;
    const int n3 = 7;
    printf("========== in in dynamic-3D-ArraySample ( %d  *  %d  *  %d )  ==========\n", n1, n2, n3); 

    int*** p3DAry = dynamicAlloc3DArray( n1, n2, n3);
    printf("dynamic3DArray = {\n");
    for( int i = 0; i < n1; ++i )
    {
        printf("  {\n");
        for( int j = 0; j < n2; ++j )
        {
            printf("    {");
            for( int k = 0; k < n3; ++k ) {
                printf("%3d%s", p3DAry[i][j][k], ( k<(n3-1) ? ", " : "  ") );
            }
            printf("}%s\n", j<(n2-1) ? "," : "");
        }
        printf("  }%s\n", i<(n1-1) ? "," : "");
    }
    printf("};\n");


    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("Sample code for pointing to a 2-D <Dynamic-Alloced> array : ...\n");
    printf("ary3D[1]  ->  \n{\n");
    int** p2DAry =  p3DAry[1];
    for( int j = 0; j < n2; ++j ) {
        printf("  {");
        int* p1DAry  = p2DAry[j];
        for( int k = 0; k < n3; ++k ) {
            printf("%3d%s", p1DAry[k],  (k < n3-1 ? ", " : " ")  );
        }
        printf("}%s\n", (j < n2-1 ? ", " : "") );
    }
    printf("};\n\n ");



    dynamicFree3DArray(p3DAry, n1, n2);
    p3DAry = NULL;
}


