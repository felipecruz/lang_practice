#include <stdio.h>
#include <stdlib.h>
#include "astnodes.h"
#include "dump.h"

/*
void error (const char *message)
{
    printf ("Error %s\n", message);
    exit(-1);
}

void print_Call (Call *call, int level)
{
    Exp *exp;
    printf ("\n%*s Call: %s ", level, "", call->id);
    exp = call->exp_list;
    while (exp) {
        print_Exp (exp, level);
        exp = exp->next;
        if (exp)
            printf (", ");
    }
}

void print_Exp (Exp *exp, int level)
{
    printf ("EXP: ");
    switch (exp->type) {
        case ExpConstInt:
            printf ("Const int: %d", exp->u.eci.val);
            break;
        case ExpConstLong:
            printf ("Const long: %ld", exp->u.ech.val);
            break;
        case ExpConstFloat:
            printf ("Const float: %f", exp->u.ecf.val);
            break;
        case ExpConstString:
            printf ("Const string: %s", exp->u.ecs.val);
            break;
        case ExpVar:
            print_Var (exp->u.ev.var, level);
            break;
        case ExpCall:
            print_Call (exp->u.ec.call, level);
            break;
        case ExpNew:
            printf ("New ");
            print_Type (exp->u.en.type, level);
            printf ("[");
            print_Exp (exp->u.en.exp, level);
            printf ("]");
            break;
        case UnaExpArith:
            switch (exp->u.eu.op) {
                case UnaArith_Minus:
                    printf ("Unary Exp [ - ] ");
                    break;
                case UnaArith_Log_Neg:
                    printf ("Unary Exp  [ ! ] ");
                    break;
                default:
                    printf ("Unknow Unary Arithmetic Expression");

            }
            print_Exp (exp->u.eu.exp, level);
            break;
        case BinExpArith:
            printf ("Binary Exp ");
            print_Exp (exp->u.eb.exp1, level);
            print_Exp (exp->u.eb.exp2, level);
            break;
        default:
            printf ("Invalid Expression");
            break;
    }
}

void print_Cmd (Cmd *cmd, int level)
{
    if (!cmd)
        return;

    switch (cmd->type) {
        case CmdIf:
            printf ("\n%*s If ", level, "");
            level += 1;
            print_Exp (cmd->u.cif.cond, level);
            printf ("\n%*sThen: ", level, "");
            print_Cmd (cmd->u.cif.then, level);
            if (cmd->u.cif._else) {
                printf ("\n%*sElse: ", level, "");
                print_Cmd (cmd->u.cif._else, level);
            }
            break;
        case CmdWhile:
            printf ("\n%*s While ", level, "");
            print_Exp (cmd->u.cw.cond, level);
            print_Cmd (cmd->u.cw.body, level);
            break;
        case CmdAss:
            printf ("\n%*s Assignment ", level, "");
            print_Var (cmd->u.ca.var, level);
            printf ("<- ");
            print_Exp (cmd->u.ca.exp, level);
            break;
        case CmdRet:
            printf ("\n%*s Return ", level, "");
            if (cmd->u.cr.exp)
                print_Exp (cmd->u.cr.exp, level);
            printf ("\n");
            break;
        case CmdCall:
            print_Call (cmd->u.cc.call, level);
            break;
        case CmdBlock:
            print_Block (cmd->u.cb.block, level);
            break;
    }
}

void print_Var (struct Var *var, int level)
{
    switch (var->type) {
        case VarSingle:
            printf ("Var: %s ", var->u.vs.id);
            break;
        case VarArray:
            print_Var (var->u.va.var, level);
            printf ("[");
            print_Exp (var->u.va.exp, level);
            printf ("]");
            break;
    }
}

void print_Type (Type *type, int level)
{
    switch (type->type) {
        case TypeInt:
            printf ("Int");
            break;
        case TypeChar:
            printf ("Char");
            break;
        case TypeFloat:
            printf ("Float");
            break;
        case TypeVoid:
            printf ("Void");
            break;
        default:
            error ("Invalid Type");
    }

    if (type->array)
        printf ("[] ");
    else
        printf (" ");
}

void print_Block (Block *block, int level)
{
    Decl *decl;
    Cmd *cmd;

    if (!block)
        return;

    level += 1;

    decl = block->decl;
    while (decl) {
        print_Decl (decl, level);
        decl = decl->next;
    }

    cmd = block->cmd;
    while (cmd) {
        print_Cmd (cmd, level);
        cmd = cmd->next;
    }
}

void print_Params (Params *_params, int level)
{
    Params *params = _params;
    while (params) {
        print_Type (params->type, level);
        printf ("%s", params->id);
        params = params->next;
        if (params)
            printf (", ");
    }
}
*/

const char* type_id (Type *type)
{
    switch (type->type) {
        case TypeInt:
            return ".long";
            break;
        case TypeChar:
            return ".ascii";
            break;
        case TypeFloat:
            return ".float";
            break;
        default:
            return "ERROR";
    }
}

int type_size (Type *type)
{
    switch (type->type) {
        case TypeInt:
            return 4;
            break;
        case TypeChar:
            return 1;
            break;
        case TypeFloat:
            return 4;
            break;
        default:
            /* TODO rever isso */
            return 4;
    }
}


void generate_globals_Decl (Decl *decl)
{
    if (!decl)
        return;

    printf ("    %s: %s\n", decl->u.dv.id, type_id(decl->u.dv.type));
}

void generate_functions_Decl (Decl *decl)
{
    int offset = 0;
    Block *block;

    if (!decl)
        return;

    printf ("\n    .globl %s\n", decl->u.df.id);
    printf ("%s:\n", decl->u.df.id);
    printf ("    push %%ebp\n");
    printf ("    mov %%esp, %%ebp\n");

    /* calcula offset das variáveis locais */

    block = decl->u.df.block;
    print_Block (block, 0);
    decl = block->decl;
    while (decl) {
        print_Decl (decl, 0);
        offset = offset + type_size (decl->u.dv.type);
        decl = decl->next;
    }
    
    printf ("    sub $%d, %%esp\n", offset);

    //printf ("\nFunc Decl - Name: %s Return Type: ", decl->u.df.id);
    //print_Type (decl->u.df.type, level);
    //printf ("\n          - Paramenters ");
    //print_Params (decl->u.df.params, level);
    //print_Block (decl->u.df.block, level);
}

void generate_Program (Program *program)
{
    Decl *decl;

    if (program == NULL) {
        printf ("Empty Program");
        return;
    }

    printf (".text\n");

    decl = program->decl;

    while (decl) {
        generate_Decl (decl);
        decl = decl->next;
    }

    printf ("\n");
}
