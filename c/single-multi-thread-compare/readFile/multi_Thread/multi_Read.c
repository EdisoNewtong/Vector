#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_COUNT 100
#define ITEM_COUNT   2000000


struct score_data {
    int t_id;
    int student_cnt;
    unsigned long long offset;
    // int* scores[ITEM_COUNT];
    int* scores;
};


void *getLineData(void* arg)
{
    struct score_data* dt = (struct score_data*)(arg);
    FILE* pFile = fopen("../../bk/100_x_2000000.bin", "rb");
    if ( pFile == NULL || dt == NULL ) {
        printf("[ERROR] In thread [%d] , Can't open the given file. \n", dt->t_id );
        return NULL;
    }

    int ret = fseek(pFile, dt->offset, SEEK_SET);
    if ( ret!= 0 ) {
        printf("[ERROR] In thread [%d] , Can't move cursor to the specified position . \n", dt->t_id );
        return NULL;
    }

    size_t blockCnt = fread(&(dt->scores[0]), sizeof(int), dt->student_cnt, pFile );
    if ( blockCnt != dt->student_cnt ) {
        printf("[ERROR] In thread [%d] , something error occurs during reading , blockCnt(%zu)  !=  student_cnt(%d)  . \n", dt->t_id,  blockCnt,  dt->student_cnt );
        return NULL;
    }
    fclose(pFile);

    return NULL;
}



int main(int argc, char* argv[], char* env[])
{
    pthread_t tid_ary[THREAD_COUNT];
    struct score_data data_ary[THREAD_COUNT];

    FILE* pFile = fopen("../../bk/100_x_2000000.bin", "rb");
    if ( pFile == NULL ) {
        printf("[ERROR] Can't open the given file. \n" );
        fclose(pFile);
        return -1;
    }
    int ret = fseek(pFile, 4, SEEK_SET);
    if ( ret!= 0 ) {
        printf("[ERROR] Can't move cursor to the specified position [%d] . \n", 4 );
        fclose(pFile);
        return -1;
    }

    // int student_cnt[THREAD_COUNT] = {
    //      1999979, 1999983, 1999977,     1999983, 1999971,     1999967,    1999972,   1999990, 1999971,  1999997,     1999964,    1999976, 1999983, 1999957, 1999977, 1999984, 1999982, 1999988, 1999977, 1999971, 1999968, 1999960, 1999977, 1999951, 
    //      1999981, 1999980, 1999982, 1999983, 1999954, 1999950, 1999978, 1999986, 1999951, 1999967, 1999988, 1999978, 1999979, 1999999, 1999992, 1999988, 1999961, 1999982, 1999972, 1999974, 1999971, 2000000, 1999973, 1999997, 
    //      1999998, 1999988, 1999960, 1999999, 1999967, 1999964, 1999999, 1999994, 1999991, 1999959, 1999953, 1999969, 1999970, 1999998, 1999966, 1999967, 1999956, 1999958, 1999999, 1999993, 1999971, 1999954, 1999981, 1999978, 
    //      1999975, 1999990, 1999988, 1999990, 1999989, 1999971, 1999956, 1999997, 1999992, 1999958, 1999964, 1999983, 2000000, 2000000, 1999956, 1999982, 1999975, 1999960, 1999953, 2000000, 1999958, 1999974, 1999958, 1999998, 2000000, 1999969, 2000000, 1999990 
    // };
    int student_cnt[THREAD_COUNT] = { 0 };
    size_t rdBlockCnt = fread( ((void*)student_cnt), sizeof(int), THREAD_COUNT, pFile );
    if ( rdBlockCnt != THREAD_COUNT ) {
        printf("[ERROR] Something error occurs during reading , readed_BlockCnt(%zu) != classCnt(%d)  . \n",   rdBlockCnt, THREAD_COUNT);
        fclose(pFile);
        return -1;
    }
    fclose(pFile);

    unsigned long long beg_offset = 4 + (THREAD_COUNT * sizeof(int) );

    // set the students' count , and offset of file to be read
    int iHasError = -1;
    for ( int i = 0; i < THREAD_COUNT; ++i ) {
        data_ary[i].t_id = i;
        data_ary[i].student_cnt = student_cnt[i];
        data_ary[i].scores = (int*)malloc( sizeof(int) * ITEM_COUNT );
        if ( data_ary[i].scores == NULL ) {
            printf("[ERROR] : inside #%d thread , no enough memory to alloc", i);
            iHasError = i;
            break;
        }

        if ( i == 0 ) {
            data_ary[i].offset = beg_offset;
        } else {
            data_ary[i].offset = data_ary[i-1].offset + (student_cnt[i-1] * sizeof(int) );
        }
        // printf("[%d] Offset = %llu , read %d int numbers . \n", i, data_ary[i].offset , student_cnt[i] );
    }

    if ( iHasError != -1 ) {
        printf("[ERROR] : Quit!");
        return 0;
    }

    // printf("[INFO] after calc offset. \n");

    // create  <THREAD_COUNT>   threads
    for( int i = 0; i < THREAD_COUNT; ++i ) {
        pthread_create( &tid_ary[i], NULL, getLineData,  &data_ary[i] );
    }
    // printf("[INFO] after create %d threads. \n", THREAD_COUNT);

    // wait for all thread finished
    for( int i = 0; i < THREAD_COUNT; ++i ) {
        pthread_join( tid_ary[i], NULL);
    }
    // printf("[INFO] All thread finished.\n ");


    //
    // main thread
    //
    const int outputNo = 5;
    // output info && avg  into   console
    for( int i = 0; i < THREAD_COUNT; ++i ) {
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

        free(data_ary[i].scores);
        data_ary[i].scores = NULL;
    }

    return 0;
}



