%{
#include <stdio.h>

#define TK_ID 300
#define TK_NUMBER 301
#define TK_HEXA 302
#define TK_STRING 303

#define TK_IF 304
#define TK_ELSE 305
#define TK_WHILE 306

#define TK_TYPE_VOID 320
#define TK_TYPE_INT 321
#define TK_TYPE_FLOAT 322
#define TK_TYPE_CHAR 323

#define TK_RETURN 330
#define TK_NEW 331

%}

VOID    "void"
CHAR    "char"
INT     "int"
FLOAT   "float"

IF      "if"
ELSE    "else"
WHILE   "while"

RETURN  "return"
NEW     "new"

N [0-9]+
H 0x[a-fA-F0-8]+
ID [a-zA-Z_][a-zA-Z_0-9]*
STRING "\""([^"\\\n]|(\\[nt\n"\\]))*"\""
COMMENTS "/*"([^"*"]|("*"[^/]))*?"*/"

%%

{VOID}    { return TK_TYPE_VOID; }
{CHAR}    { return TK_TYPE_CHAR; }
{FLOAT}   { return TK_TYPE_FLOAT; }
{INT}     { return TK_TYPE_INT; }
{IF}      { return TK_IF; }
{ELSE}    { return TK_ELSE; }
{WHILE}   { return TK_WHILE; }
{RETURN}  { return TK_RETURN; }
{NEW}     { return TK_NEW; }

{ID} { printf("TK_ID: %s\n", yytext); }
{N} { printf("TK_NUMBER: %s\n", yytext); }
{H} { printf("TK_HEXA: %s\n", yytext); }
{STRING} { printf("TK_STRING: %s\n", yytext); }

{COMMENTS} { printf("TK_COMMENT: %s\n", yytext); }

[ \t\n] ;
. { printf("UNMATCHED: %s\n", yytext); }

%%
