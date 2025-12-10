#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_NITEMS      1000000
#define MAX_NTHREADS    100

static unsigned long long SLEEP_TIME = (1000ull / 1000ul * 3ull);


int nitems;

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
void*  produce_1(void*);
void*  produce_2(void*);
void*  produce_3(void*);
void*  produce_4(void*);
void*  produce_5(void*);

void*  consume(void*);

int main(int argc, char* argv[], char* env[])
{
    int gCnt = 0;
    int i, nthreads, count[MAX_NTHREADS];
    pthread_t tid_produce[MAX_NTHREADS],  tid_consume;

    // init struct members
    for( i = 0; i < MAX_NITEMS; ++i ) {
        shared.buff[i] = 0;
    }
    shared.nput = 0;
    shared.nval = 0;

    if ( argc != 3 ) {
        printf("Usage : main   <#items>   <#threads>\n");
        return -1;
    }

    nitems   = ( atoi(argv[1]) < MAX_NITEMS   ? atoi(argv[1]) : MAX_NITEMS   );
    nthreads = ( atoi(argv[2]) < MAX_NTHREADS ? atoi(argv[2]) : MAX_NTHREADS );

    // set_concurrency( nthreads );
    // pthread_setconcurrency( nthreads );

    void*  (*funcAry[5])(void*) = { produce_1, produce_2, produce_3, produce_4, produce_5 };

    // start all the producer threads 
    for( i = 0; i < nthreads; ++i ) {
        count[i] = 0;
        // create 1 thread whose return thread_id will be stored into tid_produce[i]
        //                  rountine function: produce
        //                  rountine function argument: &count[i] 
        // pthread_create( &tid_produce[i], NULL, produce, &[i] );
        pthread_create( &tid_produce[i], NULL, funcAry[i], &count[i] );
    }

    // wait for all the producer threads finished
    for( i = 0; i < nthreads; ++i ) {
        pthread_join( tid_produce[i], NULL);
        printf("Thread #%d : filled slot count[%d] = %d\n",  i+1, i, count[i] );

        gCnt += count[i];
    }
    printf("\n");
    printf("    gCnt = %d, gCnt %s nitems [%s] \n", gCnt, (gCnt==nitems ? "==" : "!=") , (gCnt==nitems ? "OK" : "ERROR") );


    // start, then wait for the consumer thread */
    pthread_create( &tid_consume, NULL, consume, NULL);
    pthread_join(tid_consume, NULL);

    exit(0);
}


void* produce_1(void* arg)
{
    // <Infinite> for-loop style
    for( ; ; ) {
        // pthread_mutex_lock(&shared.mutex);

        // int l_ret = pthread_mutex_lock(&shared.mutex);
        // printf("Lock #1: ret = %d\n", l_ret );
        printf("Inside #1\n");
        printf("Lock #1: ret = %d\n",  pthread_mutex_lock(&shared.mutex) );

        if ( shared.nput >= nitems ) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }

        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;

        usleep( SLEEP_TIME );

        printf("UnLock #1\n");
        pthread_mutex_unlock( &shared.mutex );

        *( (int*)arg ) += 1;
    }
}

void* produce_2(void* arg)
{
    // <Infinite> for-loop style
    for( ; ; ) {
        // pthread_mutex_lock(&shared.mutex);

        // int l_ret = pthread_mutex_lock(&shared.mutex);
        // printf("Lock #2: ret = %d\n", l_ret );
        printf("Inside #2\n");
        printf("Lock #2: ret = %d\n",  pthread_mutex_lock(&shared.mutex) );

        if ( shared.nput >= nitems ) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }

        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;

        usleep( SLEEP_TIME );

        printf("UnLock #2\n");
        pthread_mutex_unlock( &shared.mutex );

        *( (int*)arg ) += 1;
    }
}

void* produce_3(void* arg)
{
    // <Infinite> for-loop style
    for( ; ; ) {
        // pthread_mutex_lock(&shared.mutex);

        // int l_ret = pthread_mutex_lock(&shared.mutex);
        // printf("Lock #3: ret = %d\n", l_ret );
        printf("Inside #3\n");
        printf("Lock #3: ret = %d\n",  pthread_mutex_lock(&shared.mutex) );

        if ( shared.nput >= nitems ) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }

        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;

        usleep( SLEEP_TIME );

        printf("UnLock #3\n");
        pthread_mutex_unlock( &shared.mutex );

        *( (int*)arg ) += 1;
    }
}

void* produce_4(void* arg)
{
    // <Infinite> for-loop style
    for( ; ; ) {
        // pthread_mutex_lock(&shared.mutex);

        // int l_ret = pthread_mutex_lock(&shared.mutex);
        // printf("Lock #4: ret = %d\n", l_ret );
        printf("Inside #4\n");
        printf("Lock #4: ret = %d\n",  pthread_mutex_lock(&shared.mutex) );

        if ( shared.nput >= nitems ) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }

        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;

        usleep( SLEEP_TIME );

        printf("UnLock #4\n");
        pthread_mutex_unlock( &shared.mutex );

        *( (int*)arg ) += 1;
    }
}

void* produce_5(void* arg)
{
    // <Infinite> for-loop style
    for( ; ; ) {
        // pthread_mutex_lock(&shared.mutex);

        // int l_ret = pthread_mutex_lock(&shared.mutex);
        // printf("Lock #5: ret = %d\n", l_ret );
        printf("Inside #5\n");
        printf("Lock #5: ret = %d\n",  pthread_mutex_lock(&shared.mutex) );

        if ( shared.nput >= nitems ) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }

        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;

        usleep( SLEEP_TIME );

        printf("UnLock #5\n");
        pthread_mutex_unlock( &shared.mutex );

        *( (int*)arg ) += 1;
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




