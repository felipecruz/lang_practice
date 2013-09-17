%{
#include <stdio.h>
#include "y.tab.h"
void yyerror (char *);
%}

%start PROGRAM

%token TK_ID 300
%token TK_NUMBER 301
%token TK_HEXA 302
%token TK_FLOAT 303
%token TK_STRING 304

%token TK_IF 305
%token TK_ELSE 306
%token TK_WHILE 307

%token TK_TYPE_VOID 320
%token TK_TYPE_INT 321
%token TK_TYPE_FLOAT 322
%token TK_TYPE_CHAR 323

%token TK_RETURN 330
%token TK_NEW 331

%token TK_DBL_EQ 350
%token TK_LTE 351
%token TK_GTE 352
%token TK_LOG_AND 353
%token TK_LOG_OR 354

%token TK_SINGLE_EQ '='
%token TK_MULTI '*'
%token TK_DIV '/'
%token TK_PLUS '+'
%token TK_SINGLE_MINUS '-'
%token TK_LT '<'
%token TK_GT '>'
%token TK_OPPAR '('
%token TK_CLPAR ')'
%token TK_OPBRA '{'
%token TK_CLBRA '}'
%token TK_OPSQB '['
%token TK_CLSQB ']'
%token TK_SEMICOL ';'
%token TK_LOGNEG '!'
%token TK_COMMA ','
%token TK_COLON ':'
%token TK_QUESTION '?'
%token TK_PERIOD '.'

%token ERR_UNMATCHED 600
%token ERR_MALLOC 601
%token ERR_VAL 602

%%

PROGRAM: DECL_VAR
       | DECL_FUNC
       ;

TYPE: TK_TYPE_CHAR
    | TK_TYPE_FLOAT
    | TK_TYPE_INT
    | TK_TYPE_VOID
    ;

DECL_VAR: TYPE TK_ID '(' ')'
DECL_FUNC: TYPE TK_ID';'

%%

void yyerror (char *s) {
    fprintf (stderr, "%s\n", s);
}

int main (void) {
    yyparse ();
    return 0;
}
