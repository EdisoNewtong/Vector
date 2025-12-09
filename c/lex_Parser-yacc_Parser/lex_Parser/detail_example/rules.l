%{
/* C source code head part : in order to insert the following 2 lines into the most beginning part of the over coming file "lex.yy.c"  */
#include <stdio.h>
#include <stdlib.h>

/* global line count */
int lineno = 1;
%}

%%

\n                      { ++lineno; }


[ \t\r]+                { /* skip whitespaces */ }

"//".*                  { /* skip single-line comment */ }

"/*"([^*]|\*+[^*/])*\*+"*/"   { 
                          /* to calculate the internal number of the new-lines */
                          const char *p = yytext;
                          for (; *p; ++p) if (*p=='\n') ++lineno;
                          /* skip comment */
                        }

\"(\\.|[^"\\])*\"        {
                          printf("STRING(%s) (line %d)\n", yytext, lineno);
                        }

"if"                    { printf("KW_IF(%s) (line %d)\n", yytext, lineno); }
"else"                  { printf("KW_ELSE(%s) (line %d)\n", yytext, lineno); }
"while"                 { printf("KW_WHILE(%s) (line %d)\n", yytext, lineno); }
"return"                { printf("KW_RETURN(%s) (line %d)\n", yytext, lineno); }
"int"                   { printf("KW_INT(%s) (line %d)\n", yytext, lineno); }

[a-zA-Z_][a-zA-Z0-9_]*  { printf("IDENT(%s) (line %d)\n", yytext, lineno); }

[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?   { printf("NUMBER(%s) (line %d)\n", yytext, lineno); }
[0-9]+([eE][+-]?[0-9]+)?           { printf("NUMBER(%s) (line %d)\n", yytext, lineno); }

"=="                    { printf("OP_EQ(%s) (line %d)\n", yytext, lineno); }
"!="                    { printf("OP_NE(%s) (line %d)\n", yytext, lineno); }
"<="                    { printf("OP_LE(%s) (line %d)\n", yytext, lineno); }
">="                    { printf("OP_GE(%s) (line %d)\n", yytext, lineno); }
"<<"                    { printf("OP_LSH(%s) (line %d)\n", yytext, lineno); }
">>"                    { printf("OP_RSH(%s) (line %d)\n", yytext, lineno); }

"+"|"-"|"*"|"/"|"="|"(" | ")" | "{" | "}" | ";" | "," | "<" | ">"     { printf("CHAR(%s) (line %d)\n", yytext, lineno); }

.                       { printf("UNKNOWN(%s) (line %d)\n", yytext, lineno); }

%%

/* main function (optional) , in order to test    the program named "lexer"     */
int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (!f) { perror("fopen"); return 1; }
        yyin = f;
    }
    yylex();
    return 0;
}


/* optional error handled function  */
int yywrap(void) { return 1; }

