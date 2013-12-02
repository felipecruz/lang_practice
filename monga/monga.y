%{
#include <stdlib.h>
#include <stdio.h>
#include "astnodes.h"
#include "typesystem.h"
#include "dump.h"
#include "codegen.h"
#include "y.tab.h"
void yyerror (char *);
int main (int argc, char **argv);

extern FILE *yyin;
extern int yylineno;
extern char* yytext;
static int level = 0;

static Program *__program = NULL;
static struct Stack *stack = NULL;

#define YYDEBUG 1
%}

%start main_program

%union {
    int ival;
    long hval;
    float fval;
    char *sval;

    struct Program *program;
    struct Decl *decl;
    struct Type *type;
    struct Params *params;
    struct Block *block;
    struct Cmd *cmd;
    struct Exp *exp;
    struct Var *var;
    struct Call *call;
};

%token <sval> ID
%token <ival> NUMBER
%token <hval> HEXA
%token <fval> FLOAT
%token <sval> STRING

%type<program> program;
%type<decl> decl decl_var decl_func decl_list;
%type<type> type base_type array_type;
%type<params> params multi_param;
%type<block> regular_block scope_block;
%type<cmd> commands command if_cmd return;
%type<exp> exp exp_list var_exp;
%type<var> var;
%type<call> call;

%token IF
%token ELSE
%token WHILE

%token TYPE_VOID
%token TYPE_INT
%token TYPE_FLOAT
%token TYPE_CHAR

%token RETURN
%token NEW

%token EXTERN

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

main_program: program { __program = $1; };

program: { $$ = new_Program (); }
       | decl program { $$ = add_Decl ($2, $1); };

decl: decl_var SEMICOL
    | decl_func
    ;

decl_var: type ID
    { Decl *decl = has_name_same_level (stack, yylval.sval, level);
      if (decl)
          yyerror ("Variable Redeclaration\n");
      $$ = new_Decl_Var ($1, yylval.sval, NULL);
      add_declaration (stack, $$, level);
    }
        | decl_var COMMA ID
    { Decl *_decl = (Decl*) $1;
      Decl *redecl = has_name_same_level (stack, yylval.sval, level);
      if (redecl)
            yyerror ("Variable Redeclaration\n");
      $$ = new_Decl_Var (_decl->u.dv.type, $3, _decl);
      add_declaration (stack, $$, level);
    };

decl_func: type ID open_scope params CLPAR scope_block
          {  Decl *decl = has_name_same_level (stack, $2, level);
             if (decl)
                 yyerror ("Function Redeclaration\n");
             $$ = new_Decl_Func ($1, $2, $4, $6, 0);
             add_declaration (stack, $$, level);
             }
         | TYPE_VOID ID open_scope params CLPAR scope_block
           { Decl *decl = has_name_same_level (stack, $2, level);
             if (decl)
                 yyerror ("Function Redeclaration\n");
             Type *type = new_Type (TypeVoid, 0);
             $$ = new_Decl_Func (type, $2, $4, $6, 0);
             add_declaration (stack, $$, level);
             }
         | EXTERN TYPE_VOID ID OPPAR params CLPAR SEMICOL
           { Decl *decl = has_name_same_level (stack, $3, level);
             if (decl)
                 yyerror ("Function Redeclaration\n");
             Type *type = new_Type (TypeVoid, 0);
             $$ = new_Decl_Func (type, $3, $5, NULL, 1);
             add_declaration (stack, $$, level);
             };

type : array_type
     | base_type
     ;

base_type : TYPE_INT { $$ = new_Type (TypeInt, 0); }
          | TYPE_CHAR { $$ = new_Type (TypeChar, 0); }
          | TYPE_FLOAT { $$ = new_Type (TypeFloat, 0); };

array_type: type OPSQB CLSQB { Type *type = (Type*)$1;
                               type->array = 1;
                               $$ = type;
                             };

open_scope: OPPAR { level += 1; };
close_scope: CLBRA { remove_top_elements (stack, level); level -= 1; };

scope_block : OPBRA decl_list commands close_scope
              { $$ = new_Block($2, $3); };
regular_block : OPBRA decl_list commands CLBRA
              { $$ = new_Block($2, $3); };

decl_list: /* vazio */ { $$ = NULL; }
         | decl decl_list { Decl *_decl = (Decl*) $1;
                            Decl *_list = (Decl*) $2;
                            Decl *_original_list = (Decl*) $2;

                            if (_decl && _list) {
                                while (_list->next) {
                                    _list = _list->next;
                                }
                                _list->next = _decl;
                                $$ = _original_list;
                            } else {
                                $$ = _decl;
                            }
                          }
         ;

params : /* vazio */ { $$ = NULL; }
       | multi_param
       ;

multi_param: type ID {
             Decl *param = new_Decl_Var ($1, $2, NULL);
             add_declaration (stack, param, level);
             $$ = new_Param ($1, $2, NULL, param); }
           | type ID COMMA multi_param {
             Decl *param = new_Decl_Var ($1, $2, NULL);
             add_declaration (stack, param, level);
             $$ = new_Param ($1, $2, $4, param); }
           ;

commands: /* empty */ { $$ = NULL; }
        | command commands { Cmd *_cmd = (Cmd*) $1;
                             _cmd->next = $2;
                             $$ = _cmd;
                           };

command: if_cmd { $$ = $1; }
       | return { $$ = $1; }
       | var SINGLE_EQ exp SEMICOL { $$ = new_Assign_Cmd ($1, $3); }
       | call SEMICOL { $$ = new_Call_Cmd ($1); }
       | regular_block { $$ = new_Block_Cmd ($1); }
       | WHILE OPPAR exp CLPAR command { $$ = new_While_Cmd ($3, $5); }
       ;

if_cmd: IF OPPAR exp CLPAR command %prec IFX { $$ = new_If_Cmd ($3, $5, NULL); }
      | IF OPPAR exp CLPAR command ELSE command { $$ = new_If_Cmd ($3, $5, $7); }
      ;

return: RETURN SEMICOL { $$ = new_Return_Cmd (NULL); }
      | RETURN exp SEMICOL { $$ = new_Return_Cmd ($2); }
      ;

var : ID { Decl* decl = has_name (stack, $1);
           if (!decl)
               yyerror ("Variable not declared");
           $$ = new_Var ($1, NULL, decl); }
    | ID OPSQB exp CLSQB {
        Decl* decl = has_name (stack, $1);
        if (!decl)
            yyerror ("Variable not declared");
        Var *var = new_Var ($1, $3, decl);
        var->type = VarArray;
        $$ = var; }
    | var_exp exp CLSQB { $$ = new_Var_Array ($1, $2, NULL); };

var_exp: STRING OPSQB { $$ = new_Exp_String ($1, NULL); }
       | call OPSQB { $$ = new_Exp_Call ($1, NULL); }
       | OPPAR var_exp CLPAR OPSQB { $$ = $2; }
       ;

exp : NUMBER { $$ = new_Exp_Int ($1, NULL); }
    | HEXA { $$ = new_Exp_Hexa ($1, NULL); }
    | FLOAT { $$ = new_Exp_Float ($1, NULL); }
    | STRING { $$ = new_Exp_String ($1, NULL); }
    | var { $$ = new_Exp_Var ($1, NULL); }
    | call { $$ = new_Exp_Call ($1, NULL); }
	| OPPAR exp CLPAR { $$ = $2; }
	| NEW type OPSQB exp CLSQB { $$ = new_Exp_New ($2, $4, NULL); }
	| SINGLE_MINUS exp %prec MULTI { $$ = new_Exp_Unary (UnaArith_Minus, $2, NULL); }
	| LOGNEG exp %prec MULTI { $$ = new_Exp_Unary (UnaArith_Log_Neg, $2, NULL); }
	| exp PLUS exp { $$ = new_Exp_Binary (Arith_Plus, $1, $3, NULL); }
	| exp SINGLE_MINUS exp { $$ = new_Exp_Binary (Arith_Sub, $1, $3, NULL); }
	| exp MULTI exp { $$ = new_Exp_Binary (Arith_Mul, $1, $3, NULL); }
	| exp DIV exp { $$ = new_Exp_Binary (Arith_Div, $1, $3, NULL); }
	| exp DBL_EQ exp { $$ = new_Exp_Binary (Arith_Dbl_EQ, $1, $3, NULL); }
	| exp LTE exp { $$ = new_Exp_Binary (Arith_Lte, $1, $3, NULL); }
	| exp GTE exp { $$ = new_Exp_Binary (Arith_Gte, $1, $3, NULL); }
	| exp LT exp { $$ = new_Exp_Binary (Arith_Lt, $1, $3, NULL); }
	| exp GT exp { $$ = new_Exp_Binary (Arith_Ge, $1, $3, NULL); }
	| exp LOG_AND exp { $$ = new_Exp_Binary (Arith_Log_And, $1, $3, NULL); }
	| exp LOG_OR exp { $$ = new_Exp_Binary (Arith_Log_Or, $1, $3, NULL); }
    ;

call: ID OPPAR exp_list CLPAR { $$ = new_Call ($1, $3); }

exp_list: /* empty */ { $$ = NULL; }
        | exp
        | exp COMMA exp_list { Exp *_exp = (Exp*) $1;
                               _exp->next = $3;
                               $$ = _exp;
                              }
        ;

%%

void yyerror (char *s) {
    fprintf (stderr, "Line:%d - Token: %s- %s", yylineno, yytext, s);
    exit(-1);
}

int main (int argc, char **argv) {
    int rc = 0;
    int indent = 0;
    level = 0;
#if YYDEBUG
    yydebug = 0;
#endif

    stack = malloc (sizeof (Stack));
    stack->head = malloc (sizeof (ScopeElement));
    stack->head->decl = NULL;
    stack->head->level = 0;

    yyin = fopen (argv[1], "r");

    if (yyin == NULL) {
        fprintf (stderr, "Bad file path: %s\n", argv[1]);
        return -1;
    }

    yyparse ();

    //dump_Program (__program, indent);

    /* Na nossa construção da AST, quando encontramos uma chamada recursiva,
     * ou de uma função declarada que ainda não foi avaliada não conseguimos
     * encontrar a declaração para amarrar a chamada a declaração.
     * Essa etapa valida as chamadas verificando se existem as declarações
     * e termina no primeiro erro.
     */
    rc = link_missing_calls (__program);
    if (rc == -1) {
        fclose (yyin);
        exit(0);
    }

    fclose (yyin);

    rc = check_program (__program);
    if (rc == -1) {
        printf ("Type Error\n");
        return -1;
    }

    if (rc == -2) {
        printf ("Error - No main function\n");
        return -2;
    }

    generate_Program (__program);

    exit(0);
}
