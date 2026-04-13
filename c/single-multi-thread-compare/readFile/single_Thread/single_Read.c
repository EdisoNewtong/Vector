#include <stdio.h>
#include <stdlib.h>

#define THREAD_COUNT 100


struct score_data {
    int student_cnt;
    int* scores;
};


int main(int argc, char* argv[], char* env[])
{
    struct score_data data_ary[THREAD_COUNT];

    FILE* pFile = fopen("../../bk/100_x_2000000.bin", "rb");
    if ( pFile == NULL ) {
        printf("[ERROR] Can't open the given file. \n" );
        fclose(pFile);
        return -1;
    }

    // Skip the first 4 bytes for class count
    int ret = fseek(pFile, 4, SEEK_SET);
    if ( ret!= 0 ) {
        printf("[ERROR] Can't move cursor to the specified position [%d] . \n", 4 );
        fclose(pFile);
        return -1;
    }

    // read how many students are there in each class
    int student_cnt[THREAD_COUNT] = { 0 };
    size_t rdBlockCnt = fread( ((void*)student_cnt), sizeof(int), THREAD_COUNT, pFile );
    if ( rdBlockCnt != THREAD_COUNT ) {
        printf("[ERROR] Something error occurs during reading , readed_BlockCnt(%zu) != classCnt(%d)  . \n",   rdBlockCnt, THREAD_COUNT);
        fclose(pFile);
        return -1;
    }


    const int outputNo = 5;
    int iHasError = -1;
    // set the students' count , and offset of file to be read
    for ( int i = 0; i < THREAD_COUNT; ++i ) {
        data_ary[i].student_cnt = student_cnt[i];
        data_ary[i].scores = (int*)malloc( sizeof(int) * student_cnt[i] );
        if ( data_ary[i].scores == NULL ) {
            printf("[ERROR] : inside #%d thread , No enough memory to alloc", i);
            iHasError = i;
            break;
        }

        size_t rdBlockSize = fread( ((void*)data_ary[i].scores), sizeof(int), student_cnt[i], pFile);
        if ( rdBlockSize != student_cnt[i]  ) {
            printf("[ERROR] In class [%d] , something error occurs during reading , blockCnt(%zu)  !=  student_cnt(%d)  . \n", (i+1), rdBlockSize, student_cnt[i] );
            iHasError = i;
            break;
        }

        printf("#%3d. there are %d student in all. [ ",i+1, data_ary[i].student_cnt );
        double avg = 0.0;
        for ( int j = 0; j < data_ary[i].student_cnt; ++j ) {
            int s = data_ary[i].scores[j];
            if ( j < outputNo || (j>=(data_ary[i].student_cnt-outputNo) ) ) {
                printf(" %d %s ", s, ( (j<data_ary[i].student_cnt-1) ? "," : "") );
            } else if ( j == outputNo ) {
                printf("    ...   ");
            }

            avg += s;
        }

		avg /= data_ary[i].student_cnt;
        printf("  ].  avg = %.2f . [END]\n", avg);
    }

    if ( iHasError != -1 ) {
        printf("[ERROR] : Alloc Error @%d Quit!", iHasError);
        for ( int i = 0; i <= iHasError; ++i ) {
            if ( data_ary[i].scores != NULL ) {
                free( data_ary[i].scores );
                data_ary[i].scores = NULL;
            }
        }
        fclose(pFile);
        return -1;
    }


    // clean memory
    for( int i = 0; i < THREAD_COUNT; ++i ) {
        free(data_ary[i].scores);
        data_ary[i].scores = NULL;
    }
    fclose(pFile);

    return 0;
}



