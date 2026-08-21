%{
#include "calc.tab.h"   /* make lex know that the token enum */
%}

%%

[0-9]+            { yylval = atoi(yytext); return NUMBER; }
"*"               { return '*'; }
"/"               { return '/'; }
"+"               { return '+'; }
"-"               { return '-'; }
"("               { return '('; }
")"               { return ')'; }
[ \t\n\r]+        ;  /* ignore whitespace */
.                 { /* unknown char */ }

%%

int yywrap() { return 1; }