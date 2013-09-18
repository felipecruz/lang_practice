%{
#include <stdio.h>
#include "y.tab.h"
void yyerror (char *);
int main (int argc, char **argv);

extern FILE *yyin;
%}

%start program

%token ID
%token NUMBER
%token HEXA
%token FLOAT
%token STRING

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

%union {
    int ival;
    long hval;
    float fval;
    char *sval;
};

%%

program
    : decl
    | program decl
    ;

decl
    : decl_var
    | decl_func
    ;

decl_var: type name_list ';' ;

name_list
    : single_name
    | ID ',' ID
    ;

single_name: ID ;

type : base_type | type '[' ']' ;

base_type : TYPE_INT | TYPE_CHAR | TYPE_FLOAT ;

decl_func : return_type ID '(' params ')' block ;

return_type : TYPE_VOID ;

params : /* vazio */
       | param
       | multi_param
       ;

multi_param: param ',' param ;

param : type ID ;

block : '{' decl '}'
      ;

/*
var : ID
    | exp '[' exp ']'
    ;

exp : NUMBER
    | HEXA
    | FLOAT
    | STRING
    | ID
	| var
	| '(' exp ')'
	| call
	| 'new' type '[' exp ']'
	| '-' exp
	| exp '+' exp
	| exp '-' exp
	| exp '*' exp
	| exp '/' exp
	| exp '==' exp
	| exp '<=' exp
	| exp '>=' exp
	| exp '<' exp
	| exp '>' exp
	| '!' exp
	| exp '&&' exp
	| exp '||' exp
    ;

call: ID '(' exp_list ')' ;

exp_list : vazio
         | exp
         | exp_many
         ;

exp_many: exp ',' exp ;
*/

%%

void yyerror (char *s) {
    fprintf (stderr, "%s\n", s);
}

int main (int argc, char **argv) {
    int tk;

    yylval.sval = NULL;
    yyin = fopen (argv[1], "r");

    if (yyin == NULL) {
        fprintf (stderr, "Bad file path: %s\n", argv[1]);
        return -1;
    }

    do {
        tk = yylex ();
    } while (tk != 0 &&
             tk != ERR_UNMATCHED &&
             tk != ERR_VAL &&
             tk != ERR_MALLOC);

    if (tk == ERR_UNMATCHED) {
        fprintf (stderr, "Line:%d Error - Invalid token: %s\n", yylineno, yylval.sval);
        return -1;
    }

    if (tk == ERR_VAL)
        return -1;

    if (tk == ERR_MALLOC)
        return -1;

    fprintf(stdout, "Valid file\n");
    fclose (yyin);
}
