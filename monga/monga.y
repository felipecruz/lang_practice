%{
#include <stdio.h>
#include "y.tab.h"
void yyerror (char *);
int main (int argc, char **argv);
%}

%start program

%token ID 300
%token NUMBER 301
%token HEXA 302
%token FLOAT 303
%token STRING 304

%token IF 305
%token ELSE 306
%token WHILE 307

%token VOID 320
%token INT 321
%token FLOAT 322
%token CHAR 323

%token RETURN 330
%token NEW 331

%token DBL_EQ 350
%token LTE 351
%token GTE 352
%token LOG_AND 353
%token LOG_OR 354

%token SINGLE_EQ '='
%token MULTI '*'
%token DIV '/'
%token PLUS '+'
%token SINGLE_MINUS '-'
%token LT '<'
%token GT '>'
%token OPPAR '('
%token CLPAR ')'
%token OPBRA '{'
%token CLBRA '}'
%token OPSQB '['
%token CLSQB ']'
%token SEMICOL ';'
%token LOGNEG '!'
%token COMMA ','
%token COLON ':'
%token QUESTION '?'
%token PERIOD '.'

%token ERR_UNMATCHED 600
%token ERR_MALLOC 601
%token ERR_VAL 602

%%

program 
    : decl
    | program decl
    ;

decl 
    : decl_var 
    | decl_func 
    ;

decl_varc: type name_list ';' ;

name_list 
    : single_name 
    | ID ',' ID
    ;

single_name: ID ;

type : base_type | type '[' ']' ;

base_type : 'int' | 'char' | 'float' ;

decl_func : return_type ID '(' params ')' block ;

return_type : type | 'void' ;

params : /* vazio */
       | param
       | multi_param
       ;

multi_param: param ',' param ;

param : type ID ;

block : '{' decl '}'
      ;
      

var : ID 
    | exp '[' exp ']'
    ;

exp : N
    | H
    | STRING
    | ID
	| var
	/*| '(' exp ')'
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
	| exp '||' exp*/
    ;
    
call: ID '(' exp_list ')' ;

exp_list : /* vazio */ 
         | exp
         | exp_many
         ;

exp_many: exp ',' exp ;

%%

void yyerror (char *s) {
    fprintf (stderr, "%s\n", s);
}

int main (int argc, char **argv) {
    yyparse ();
    return 0;
}
