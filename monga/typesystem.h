#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "astnodes.h"

#ifndef _TYPESYSTEM_H_
#define _TYPESYSTEM_H_

#ifdef DEBUG
#define printdebug      printf
#else
#define printdebug(...) /**/
#endif

int check_declaration_block (Decl *decl);
int link_missing_calls (Program *program);
int link_and_validate_calls(Decl *decl, Decl *globals);
int check_program (Program *program);
int match (Type *t1, Type *t2);
Decl *resolve_decl (char *id, Decl *decl);
Type *get_exp_type (Exp *exp);
Type *resolve_type (Var *var);
Params* get_func_params (Call *call);
#endif
