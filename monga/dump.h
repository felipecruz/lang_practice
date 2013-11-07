#include <stdio.h>
#include <stdlib.h>
#include "astnodes.h"

#ifndef _DUMP_H_
#define _DUMP_H_
void error (const char *message);
void dump_Program (Program *program, int level);
void print_Decl (Decl *decl, int level);
void print_Params (Params *_params, int level);
void print_Call (Call *call, int level);
void print_Exp (Exp *exp, int level);
void print_Cmd (Cmd *cmd, int level);
void print_Var (struct Var *var, int level);
void print_Type (Type *type, int level);
void print_Block (Block *block, int level);
#endif
