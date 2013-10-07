%{
#include <stdlib.h>
#include <stdio.h>
#include "astnodes.h"
#include "dump.c"
#include "y.tab.h"
void yyerror (char *);
int main (int argc, char **argv);

extern FILE *yyin;
extern int yylineno;
extern char* yytext;

static Program *program;

#define YYDEBUG 1
%}

%start program

%union {
    int ival;
    long hval;
    float fval;
    char *sval;

    struct Program *program;
    struct Decl *decl;
    struct NameList *name_list;
    struct Type *type;
    struct Params *params;
    struct Block *block;
};

%token <sval> ID
%token <ival> NUMBER
%token <hval> HEXA
%token <fval> FLOAT
%token <sval> STRING

%type<program> program;
%type<decl> decl decl_var decl_func;
%type<name_list> name_list;
%type<type> type base_type array_type;
%type<params> params;
%type<block> block;


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

program: { program = NULL; }
       | decl program { program = new_Program ($1); }
       ;

decl: decl_var
    | decl_func
    ;

decl_var: type name_list SEMICOL { $$ = new_Decl_Var ($1, $2); };

decl_func: type ID OPPAR params CLPAR block 
           { $$ = new_Decl_Func ($1, $2, $4, $6); }
         | TYPE_VOID ID OPPAR params CLPAR block
           { Type *type = new_Type (TypeVoid, 0);
             $$ = new_Decl_Func (type, $2, $4, $6); };

type : array_type
     | base_type
     ;

name_list:
      ID { $$ = new_Name_List ($1, NULL); }
    | name_list COMMA ID { $$ = new_Name_List ($3, $1); }
    ;

base_type : TYPE_INT { $$ = new_Type (TypeInt, 0); }
          | TYPE_CHAR { $$ = new_Type (TypeChar, 0); }
          | TYPE_FLOAT { $$ = new_Type (TypeFloat, 0); };

array_type: type OPSQB CLSQB { Type *type = (Type*)$1;
                               type->array = 1;
                               $$ = type; 
                             };

block : OPBRA decl_list commands CLBRA { $$ = NULL; } ;

decl_list: /* vazio */
         | decl_list decl
         ;

params : /* vazio */ { $$ = NULL; }
       | multi_param { $$ = NULL; }
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
	| NEW type OPSQB exp CLSQB
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
    yydebug = 0;
#endif

    yyin = fopen (argv[1], "r");

    if (yyin == NULL) {
        fprintf (stderr, "Bad file path: %s\n", argv[1]);
        return -1;
    }

    yyparse ();

    dump_Program (program); 

    fclose (yyin);
    exit(0);
}
