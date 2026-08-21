%{
#include "y.tab.h"   /* make lex know that the token enum */
%}

%%

[0-9]+            { yylval = atoi(yytext); return NUMBER; }
"+"               { return '+'; }
"-"               { return '-'; }
.                 { /* unknown char */ }

%%

int yywrap() { return 1; }