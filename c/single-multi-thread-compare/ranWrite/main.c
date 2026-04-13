#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
    
/***********************************************************************************

=========================
*  File Content Struct  *
=========================

[BEGIN]

class_count 4 bytes int
      class_count x student_count( 4 bytes int )
    - students_count# 0  
    - students_count# 1  
    - students_count# 2  
           ...
    - students_count# (class_count-3)
    - students_count# (class_count-2)  
    - students_count# (class_count-1)

students_count #0    score#0 , score#1 , score#2 ,    ...   ,   score# (students_count #0 -1)
students_count #1    score#0 , score#1 , score#2 ,    ...   ,   score# (students_count #1 -1)
     ...

students_count #(class_count-1)    score#0 , score#1 , score#2 ,    ...   ,     score#  ( students_count #(class_count-1)  -1 )

[END]

***********************************************************************************/

int main(int argc, char* argv[])
{
    if ( argc < 3 ) {
        printf("[ERROR] Missing required arguments.  \n"
               "    Usage : ./main   <class_count>   <student_max_count>\n" 
               "Program abort! ");
        return -1;
    }

    /***********************************************************************************
      There are n(classCnt) classes ( n's range is [1,100])
      Each class have at most m(studentMaxCnt) students  (m's range is [ 51, +infinate ] )
      each students score's range is  [0, 50000] ( an int number)
    ***********************************************************************************/
    int classCnt = atoi( argv[1] );
    if ( classCnt <= 0 ) {
        printf("[ERROR] The class count argument must be greater than 0.\n");
        return -1;
    } else if ( classCnt > 100 ) {
        printf("[ERROR] The class count must be no more than 100.\n");
        return -1;
	}

    const int studentMaxCnt = atoi( argv[2] );
    if ( studentMaxCnt <= 0 ) {
        printf("[ERROR] The student max count must be greater than 0.\n");
        return -1;
    }

    char fileName[1024] = { 0 };
    sprintf(fileName, "%d_x_%d.bin" , classCnt, studentMaxCnt);

    FILE* f = fopen(fileName, "wb");
    if ( f == NULL ) {
        printf("Can't open file : %s. Program abort! \n", fileName );
        return -1;
    }
	// #1st. Write Class Count
	fwrite( (void*)&classCnt, sizeof(classCnt), 1, f );


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Store how many students are there in a give class which id is specified by the array's index 
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	int studentsCntAry[100] = { 0 };
    unsigned long long int nTotalScoreCnt = 0;
	double avg = 0.0f;
    unsigned long long int singleTotal = 0;

    const int outputNo = 5;
    const int deltaCnt = 50;
    // update random seed
    srand( (unsigned int)time(0) );

    // random the studnents count inside a given class 
    for ( int i = 0; i < classCnt; ++i ) {
        int n = (studentMaxCnt - deltaCnt) + ( rand() % (deltaCnt+1) );
		studentsCntAry[i] = n;
		// #2nd.  Write students count in a class 
		fwrite( (void*)&n, sizeof(n), 1, f );
	}


	for ( int i = 0; i < classCnt; ++i ) {
		int nn = studentsCntAry[i];
        nTotalScoreCnt += nn;
        printf("#%3d. there are %d student in all. [ ",i+1, studentsCntAry[i] );
        
        singleTotal = 0;
        for( int j = 0; j < nn; ++j ) {
            int score = rand() % 50001;   // [ 0, 50000 ]
			singleTotal += score;
            if ( j < outputNo || ( j >= (nn-outputNo) ) ) {
                printf(" %d %s ", score, ((j<nn-1) ? "," : "") );
            } else if ( j == outputNo ) {
                printf("    ...   ");
            }
            fwrite( (void*)&score, sizeof(score), 1, f);
        }

		avg = singleTotal * 1.0 / nn;
        printf("  ].  avg = %.2f . [END]\n", avg);
        fflush( f );
    }
    fclose(f);
    printf("[DONE] Write file : %s Finished. Totally %d with %lld numbers. \n", fileName, classCnt,  nTotalScoreCnt );

    return 0;
}



