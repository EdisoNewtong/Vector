/*
 * alarm_thread.c
 */


#include <pthread.h>

#include "errors.h"


typedef struct alarm_tag {
	int  seconds;
	char message[64];
} alarm_t;

void* alarm_thread(void* arg)
{
	alarm_t* alarm = (alarm_t*)arg;
	int status = pthread_detach( pthread_self() );

	if( status != 0 ) {
		err_abort(status, "Detach thread");
	}

	if( alarm!=NULL ) {
		sleep(alarm->seconds);
		printf("(%d) %s\n", alarm->seconds, alarm->message);
		free(alarm);
	}

	return NULL;
}

int main(int argc, char* argv[], char* env[])
{

	int status = 0;
	char line[128] = { 0 };
	alarm_t* alarm = NULL;
	pthread_t thread;

	while(1) 
	{
		printf("Alarm> ");
		if( fgets(line, sizeof(line), stdin) == NULL ) { 
			exit (0);	/* function exit(...) is decleared in header <stdlib.h> */
		}

		/* strlen is decleared in header <string.h> */
		if( strlen(line) <=1 ) {
			continue;
		}

		/* create an alarm  */
		alarm = (alarm_t*)malloc( sizeof(alarm_t) );
		if( alarm == NULL ) {
			errno_abort("Allocate alarm");
		}

		/*
		 * Parse input line into seconds (%d) and a message
		 * (%64[^\n]), consisting of up to 64 characters
		 * separated from the seconds by whitespace.
		 */

		if( sscanf(line, "%d %64[^\n]", &alarm->seconds, alarm->message) < 2 ) {
			fprintf(stderr, "Bad command\n");
			free(alarm);
		} else {
             /****************************************************************************************************
			   Arguments List
             ****************************************************************************************************
             *
			 * 1st.  &thread pthread_t ,the reveal the thread-id if create successfully
			 * 2nd.  ???
			 * 3rd.  the executed function of the creating thread
			 * 4rd.  the only argument void* arg of the running thread , the real argument is the alloced struct pointer to alarm
             *
             ****************************************************************************************************
			 */
			status = pthread_create(&thread, NULL, alarm_thread, alarm);
			if( status !=0 ) {
				err_abort(status, "Create alarm thread");
			}
		}
	}
}



