%{
#include <stdlib.h>
#include <stdio.h>
#include "y.tab.h"
void yyerror (char *);
int main (int argc, char **argv);

extern FILE *yyin;
extern int yylineno;
extern char* yytext;

#define YYDEBUG 1
%}

%start program

%union {
    int ival;
    long hval;
    float fval;
    char *sval;
};

%token <sval> ID
%token <ival> NUMBER
%token <hval> HEXA
%token <fval> FLOAT
%token <sval> STRING

%token IF
%token ELSE
%token WHILE

%token TYPE_VOID
%token TYPE_INT
%token TYPE_FLOAT
%token TYPE_CHAR

%token RETURN
%token NEW

%token DBL_EQ
%token LTE
%token GTE
%token LOG_AND
%token LOG_OR

%token SINGLE_EQ
%token MULTI
%token DIV
%token PLUS
%token SINGLE_MINUS
%token LT
%token GT
%token OPPAR
%token CLPAR
%token OPBRA
%token CLBRA
%token OPSQB
%token CLSQB
%token SEMICOL
%token LOGNEG
%token COMMA
%token COLON
%token QUESTION
%token PERIOD

%token ERR_UNMATCHED
%token ERR_MALLOC
%token ERR_VAL

%left LOG_AND LOG_OR
%left DBL_EQ
%left GTE LTE GT LT
%left PLUS SINGLE_MINUS
%left MULTI DIV

%nonassoc IFX
%nonassoc ELSE

%%

program:
       | decl program
       ;

decl: decl_var
    | decl_func
    ;

decl_var: type name_list SEMICOL ;

decl_func: type ID OPPAR params CLPAR block
         | TYPE_VOID ID OPPAR params CLPAR block;

type : array_type
     | base_type
     ;

name_list:
      ID
    | name_list COMMA ID
    ;

base_type : TYPE_INT | TYPE_CHAR | TYPE_FLOAT ;
array_type: base_type OPSQB CLSQB ;

block : OPBRA decl_list commands CLBRA ;

decl_list: /* empty */
         | decl_list decl
         ;

params : /* vazio */
       | multi_param
       ;

multi_param: param
           | multi_param COMMA param
           ;

param : type ID ;

commands: /* empty */
        | commands command
        ;

command: if_cmd
       | WHILE OPPAR exp CLPAR command
       | var SINGLE_EQ exp SEMICOL
       | return
       | call SEMICOL
       | block

if_cmd: IF OPPAR exp CLPAR command %prec IFX
      | IF OPPAR exp CLPAR command ELSE command

return: RETURN SEMICOL
      | RETURN exp SEMICOL

var : ID | var OPSQB exp CLSQB;

exp : NUMBER
    | HEXA
    | FLOAT
    | STRING
    | var
    | call
	| OPPAR exp CLPAR
	| NEW base_type OPSQB exp CLSQB
	| SINGLE_MINUS exp %prec MULTI
	| exp PLUS exp
	| exp SINGLE_MINUS exp
	| exp MULTI exp
	| exp DIV exp
	| exp DBL_EQ exp
	| exp LTE exp
	| exp GTE exp
	| exp LT exp
	| exp GT exp
	| LOGNEG exp %prec MULTI
	| exp LOG_AND exp
	| exp LOG_OR exp
    ;

call: ID OPPAR exp_list CLPAR

exp_list: /* empty */
        | exp
        | exp_list COMMA exp
        ;

%%

void yyerror (char *s) {
    fprintf (stderr, "Line:%d - Token: %s- %s", yylineno, yytext, s);
    exit(-1);
}

int main (int argc, char **argv) {
#if YYDEBUG
    yydebug = 1;
#endif

    yyin = fopen (argv[1], "r");

    if (yyin == NULL) {
        fprintf (stderr, "Bad file path: %s\n", argv[1]);
        return -1;
    }

    yyparse ();
    fclose (yyin);
    exit(0);
}
