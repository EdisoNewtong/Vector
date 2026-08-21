%{
#include <stdio.h>
%}

%token NUMBER

%%

expr:
      expr '+' expr    { printf("%d\n", $1 + $3); }
    | expr '-' expr    { printf("%d\n", $1 - $3); }
    | NUMBER           { $$ = $1; }
    ;

%%

int main() {
    yyparse();
    return 0;
}
