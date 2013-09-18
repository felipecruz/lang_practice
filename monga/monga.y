%{
#include <stdlib.h>
#include <stdio.h>
#include "y.tab.h"
void yyerror (char *);
int main (int argc, char **argv);

extern FILE *yyin;
extern int yylineno;
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

%%

program: { printf ("PROGRAM\n"); }
       | decl
       | program decl
       ;

decl: decl_var
    | decl_func
    ;

decl_var: type name_list SEMICOL
          { printf ("DECL_VAR\n"); } ;

decl_func: return_type ID OPPAR CLPAR block
           { printf ("DECL_FUNC\n"); } ;

name_list: { printf ("NAME_LIST\n"); }
      ID
    | name_list COMMA ID
    ;

type : base_type | array_type ;

array_type: base_type OPSQB CLSQB { printf ("ARRAY_TYPE\n"); } ;

base_type : { printf ("BASE_TYPE\n"); } TYPE_INT | TYPE_CHAR | TYPE_FLOAT ;

return_type : TYPE_VOID { printf ("RETURN_TYPE\n"); } | type ;

block : { printf ("BLOCK\n"); } OPBRA CLBRA ;
/*
params :
       | param
       | multi_param
       ;
multi_param: param ',' param ;
param : type ID ;


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
    fprintf (stderr, "Line:%d - %s", yylineno, s);
    exit(-1);
}

int main (int argc, char **argv) {
    int tk;

    yylval.sval = NULL;
    yyin = fopen (argv[1], "r");

    if (yyin == NULL) {
        fprintf (stderr, "Bad file path: %s\n", argv[1]);
        return -1;
    }

    yyparse ();
    fclose (yyin);
    exit(0);
}
