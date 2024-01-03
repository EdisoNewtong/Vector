%{
#include <stdio.h>
%}


%%

[0-9]+      printf("NUMBER\n");
[a-zA-Z]+   printf("WORD\n");
.           printf("OTHER\n");

%%

