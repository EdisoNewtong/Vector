#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_NITEMS      1000000
#define MAX_NTHREADS    100


int nitems;
int recordAry[MAX_NTHREADS][MAX_NITEMS];

typedef struct ThStruct {
    int id;
    int* pArg;
} ThStruct;


// declare an anonymous struct with a given varible 
//         named   "shared"
struct {
    pthread_mutex_t mutex;

    int buff[MAX_NITEMS];
    int nput;
    int nval;
}                   shared = { 
    PTHREAD_MUTEX_INITIALIZER  // mutex = PTHREAD_MUTEX_INITIALIZER
};


// declare 2 thread rountine function
void*  produce(void*);
void*  consume(void*);

int main(int argc, char* argv[], char* env[])
{
    int gCnt = 0;
    int i,j, nthreads, count[MAX_NTHREADS];
    pthread_t tid_produce[MAX_NTHREADS],  tid_consume;
    ThStruct thAry[MAX_NTHREADS];

    // init struct members
    for( i = 0; i < MAX_NITEMS; ++i ) {
        shared.buff[i] = 0;
    }
    shared.nput = 0;
    shared.nval = 0;

    // init record 2D-Array
    for( i = 0; i < MAX_NTHREADS; ++i ) {
        for( j = 0; j < MAX_NITEMS; ++j ) {
            recordAry[i][j] = 0;
        }

        thAry[i].id = i;
        thAry[i].pArg = &count[i];
    }


    if ( argc != 3 ) {
        printf("Usage : main   <#items>   <#threads>\n");
        return -1;
    }

    nitems   = ( atoi(argv[1]) < MAX_NITEMS   ? atoi(argv[1]) : MAX_NITEMS   );
    nthreads = ( atoi(argv[2]) < MAX_NTHREADS ? atoi(argv[2]) : MAX_NTHREADS );

    // set_concurrency( nthreads );
    // pthread_setconcurrency( nthreads );


    // start all the producer threads 
    for( i = 0; i < nthreads; ++i ) {
        count[i] = 0;
        // create 1 thread whose return thread_id will be stored into tid_produce[i]
        //                  rountine function: produce
        //                  rountine function argument: &count[i] 
        // pthread_create( &tid_produce[i], NULL, produce, &[i] );
        pthread_create( &tid_produce[i], NULL, produce, &thAry[i] );
    }

    // wait for all the producer threads finished
    for( i = 0; i < nthreads; ++i ) {
        pthread_join( tid_produce[i], NULL);
        printf("count[%d] = %d\n\t", i, count[i] );

        for ( j = 0; j < nitems; ++j ) {
            if ( recordAry[i][j] == 1 ) {
                printf(" %d |", j );
            }
        }
        printf("\n");

        gCnt += count[i];
    }
    printf("gCnt = %d\n", gCnt);


    // start, then wait for the consumer thread */
    pthread_create( &tid_consume, NULL, consume, NULL);
    pthread_join(tid_consume, NULL);

    exit(0);
}

void* produce(void* arg)
{
    // <Infinite> for-loop style
    for( ; ; ) {
        pthread_mutex_lock(&shared.mutex);
        if ( shared.nput >= nitems ) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }

        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;
        pthread_mutex_unlock( &shared.mutex );

        *(((ThStruct*)arg)->pArg) += 1;
        recordAry[((ThStruct*)arg)->id][ shared.nput-1 ] = 1;
        // pthread_mutex_unlock( &shared.mutex );
    }

}



void* consume(void* arg)
{
    (void)arg;

    int i;
    for ( i = 0; i < nitems; ++i ) {
        if ( shared.buff[i] != i ) {
            printf("buff[%d] = %d\n", i, shared.buff[i] );
        }
    }

    return NULL;
}




