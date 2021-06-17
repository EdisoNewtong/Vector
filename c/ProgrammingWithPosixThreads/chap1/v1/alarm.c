/*
 * alarm.c
 */

#include "errors.h"


int main(int argc, char* argv[], char* env[])
{

	int seconds = 0;
	char line[128] = { 0 };
	char message[64] = { 0 };

	while(1) {
		printf("Alarm> ");
		/* 
		   Read the input from console => save into varible  "line"
		   e.g.

		       12  12 seconds elapsed<Enter>
		*/
		if( fgets(line, sizeof(line), stdin) == NULL ) { 
			exit (0);	/* declear in header <stdlib.h> */
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
			/* in second , not in millsecond */
			sleep(seconds);
			printf("(%d) %s\n", seconds, message);
		}
	}


	return 0;
}
