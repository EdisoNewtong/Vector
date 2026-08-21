%{
#include <stdio.h>
#include <stdlib.h>

void yyerror(const char *s);
int yylex();
%}

/* define token type */
%token NUMBER

/* indicate the priority of the following operator */
%left '+' '-'
%left '*' '/'
%right UMINUS

%%

input:
      /* Empty line */
    | input line
    ;

line:
      expr '\n'     { printf("= %d\n", $1); }
    | '\n'
    ;

expr:
      expr '+' expr     { $$ = $1 + $3; }
    | expr '-' expr     { $$ = $1 - $3; }
    | expr '*' expr     { $$ = $1 * $3; }
    | expr '/' expr     { $$ = $1 / $3; }

    /* paranthesis */
    | '(' expr ')'      { $$ = $2; }

    /* number */
    | NUMBER            { $$ = $1; }

    /* single negative */
    | '-' expr %prec UMINUS  { $$ = -$2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
