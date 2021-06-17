/*
 * alarm_fork.c
 */

#include <sys/types.h>
#include <sys/wait.h>

#include "errors.h"


int main(int argc, char* argv[], char* env[])
{

	/* int status = 0; */
	char line[128] = { 0 };
	int seconds = 0;
	pid_t pid = 0;
	char message[64] = { 0 };

	printf("before while(1)  ,  pid = %d\n", getpid());
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

		/*
		 * Parse input line into seconds (%d) and a message
		 * (%64[^\n]), consisting of up to 64 characters
		 * separated form the seconds by whitespace.
		 */

		if( sscanf(line, "%d %64[^\n]", &seconds, message) < 2 ) {
			fprintf(stderr, "Bad command\n");
		} else {
			/* fork() is declared in header file  <unistd.h> */
			pid = fork();
			printf("fork pid = %d\n", pid);
			if( pid == (pid_t)-1) {
				errno_abort("Fork");
			}

			if( pid == (pid_t)0 ) {
				/*
				 * In the child, wait and then print a message
				 */

			    /* in second , not in millsecond */
				printf("if sleep ,  pid = %d\n", getpid());
				sleep(seconds);
				printf("(%d) %s\n", seconds, message);
				exit (0);
			} else {
				/*
				 * In the parent call waitpid(...) to collect children
				 * that have already terminated.
				 */
				printf("else wait 1 ,  pid = %d\n", getpid());
				do {
					pid = waitpid( (pid_t)-1, NULL, WNOHANG);
					if( pid == (pid_t)-1 ) {
						errno_abort("Wait for child");
					}
				} while( pid != (pid_t)0 );
				printf("else wait 2,  pid = %d\n", getpid());
				
			}
		}
	}
}


