%{
/* C 头部：在生成的 lex.yy.c 顶部插入 */
#include <stdio.h>
#include <stdlib.h>

/* 全局行计数 */
int lineno = 1;
%}

%%
/* ---------- 规则区：正则 -> C 动作 ---------- */

/* 换行用于跟踪行号 */
\n                      { ++lineno; }

/* 空白（空格、制表）跳过 */
[ \t\r]+                { /* skip whitespaces */ }

/* 单行注释 // ... */
"//".*                  { /* skip single-line comment */ }

/* 多行注释 /* ... */  (简单但有效的版本) */
"/*"([^*]|\*+[^*/])*\*+"*/"   { 
                          /* 计算注释内部的换行数 */
                          const char *p = yytext;
                          for (; *p; ++p) if (*p=='\n') ++lineno;
                          /* skip comment */
                        }

/* 字符串（双引号），支持简单反斜转义（\" \\ \n） */
\"(\\.|[^"\\])*\"        {
                          printf("STRING(%s) (line %d)\n", yytext, lineno);
                        }

/* 关键字（用词汇表优先识别） */
"if"                    { printf("KW_IF(%s) (line %d)\n", yytext, lineno); }
"else"                  { printf("KW_ELSE(%s) (line %d)\n", yytext, lineno); }
"while"                 { printf("KW_WHILE(%s) (line %d)\n", yytext, lineno); }
"return"                { printf("KW_RETURN(%s) (line %d)\n", yytext, lineno); }
"int"                   { printf("KW_INT(%s) (line %d)\n", yytext, lineno); }

/* 标识符：字母或下划线开头，后跟字母数字下划线 */
[a-zA-Z_][a-zA-Z0-9_]*  { printf("IDENT(%s) (line %d)\n", yytext, lineno); }

/* 浮点数或整数：简化处理 */
[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?   { printf("NUMBER(%s) (line %d)\n", yytext, lineno); }
[0-9]+([eE][+-]?[0-9]+)?           { printf("NUMBER(%s) (line %d)\n", yytext, lineno); }

/* 多字符运算符（顺序重要：长的要先写） */
"=="                    { printf("OP_EQ(%s) (line %d)\n", yytext, lineno); }
"!="                    { printf("OP_NE(%s) (line %d)\n", yytext, lineno); }
"<="                    { printf("OP_LE(%s) (line %d)\n", yytext, lineno); }
">="                    { printf("OP_GE(%s) (line %d)\n", yytext, lineno); }
"<<"                    { printf("OP_LSH(%s) (line %d)\n", yytext, lineno); }
">>"                    { printf("OP_RSH(%s) (line %d)\n", yytext, lineno); }

/* 单字符运算符与界符（单个字符） */
"+"|"-"|"*"|"/"|"="|"(" | ")" | "{" | "}" | ";" | "," | "<" | ">" {
                          printf("CHAR(%s) (line %d)\n", yytext, lineno);
                        }

/* 其他无法识别的字符 -> 报错或打印 */
.                       { printf("UNKNOWN(%s) (line %d)\n", yytext, lineno); }



