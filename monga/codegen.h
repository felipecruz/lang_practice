#include <stdio.h>
#include <stdlib.h>
#include "astnodes.h"

#ifndef _CODEGEN_H_
#define _CODEGEN_H_
void generate_Program (Program *program);
void generate_Decl (Decl *decl, int level);

//void error (const char *message);
//void print_Params (Params *_params, int level);
//void print_Call (Call *call, int level);
//void print_Exp (Exp *exp, int level);
//void print_Cmd (Cmd *cmd, int level);
//void print_Var (struct Var *var, int level);
//void print_Type (Type *type, int level);
//void print_Block (Block *block, int level);
#endif
