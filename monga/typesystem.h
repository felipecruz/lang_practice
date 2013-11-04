#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "astnodes.h"

#ifndef _TYPESYSTEM_H_
#define _TYPESYSTEM_H_
int check_declaration_block (Decl *decl, Decl *globals, Decl *locals, Params *params);
Type *get_exp_type (Exp *exp, Decl *globals, Decl *locals, Params *params);
Type *resolve_type (char *id, Decl *globals, Decl *locals, Params *params);
int check_program (Program *program);
int match (Type *t1, Type *t2);
Type *get_expression_type (Exp *exp);
Type *get_parameter_type (Params *params, char *id);
Params* get_func_params (Call *call, Decl *globals, Decl *locals);
#endif
