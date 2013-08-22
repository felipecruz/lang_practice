%{
#include <stdio.h>

#define TK_ID 300
#define TK_NUMBER 301
#define TK_HEXA 302
%}

N [0-9]+
H 0x[a-fA-F0-8]+
ID [a-zA-Z_][a-zA-Z_0-9]*

COMMENTS "/*"([^"*"]|("*"[^/]))*?"*/"

%%

{ID} { printf("TK_ID: %s\n", yytext); }
{N} { printf("TK_NUMBER: %s\n", yytext); }
{H} { printf("TK_HEXA: %s\n", yytext); }

{COMMENTS} { }

[ \t\n] ;
. {}

%%
