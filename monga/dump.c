#include <stdio.h>
#include <stdlib.h>
#include "astnodes.h"
#include "dump.h"

void error (const char *message)
{
    printf ("Error %s\n", message);
    exit(-1);
}

void print_Call (Call *call, int level)
{
    Exp *exp;
    printf ("Call: Name: %s\n", call->id);
    exp = call->exp_list;
    while (exp) {
        print_Exp (exp, level);
        exp = exp->next;
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
            printf ("Const long: %l", exp->u.ech.val);
            break;
        case ExpConstFloat:
            printf ("Const float: %f", exp->u.ecf.val);
            break;
        case ExpConstString:
            printf ("Const string: %s", exp->u.ecs.val);
            break;
        case ExpVar:
            printf ("Var ");
            print_Var (exp->u.ev.var, level);
            break;
        case ExpCall:
            printf ("Call ");
            print_Call (exp->u.ec.call, level);
            break;
        case ExpNew:
            printf ("New ");
            print_Type (exp->u.en.type, level);
            print_Exp (exp->u.en.exp, level);
            break;
        case UnaExpArith:
            printf ("Unary Exp ");
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

    printf ("Command ");
    switch (cmd->type) {
        case CmdIf:
            printf ("If");
            print_Exp (cmd->u.cif.cond, level);
            print_Cmd (cmd->u.cif.then, level);
            print_Cmd (cmd->u.cif._else, level);
            break;
        case CmdWhile:
            printf ("While");
            print_Exp (cmd->u.cw.cond, level);
            print_Cmd (cmd->u.cw.body, level);
            break;
        case CmdAss:
            printf ("Assignment");
            print_Var (cmd->u.ca.var, level);
            print_Exp (cmd->u.ca.exp, level);
            break;
        case CmdRet:
            printf ("Return");
            print_Exp (cmd->u.cr.exp, level);
            break;
        case CmdCall:
            printf ("Call");
            print_Call (cmd->u.cc.call, level);
            break;
        case CmdBlock:
            printf ("Block");
            print_Block (cmd->u.cb.block, level);
            break;
    }
}

void print_Var (struct Var *var, int level)
{
    switch (var->type) {
        case VarSingle:
            printf ("Var name: %s", var->u.vs.id);
            break;
        case VarArray:
            printf ("Var array name: %s [", var->u.vs.id);
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

void print_NameList (NameList *name_list, int level)
{
    NameList *list = name_list;
    while (list) {
        printf ("Name %s ", list->id);
        list = list->next;
    }
    printf ("\n");
}

void print_Block (Block *block, int level)
{
    Decl *decl;
    Cmd *cmd;
    printf ("Block:\n");

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

    printf ("\n");

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
    printf ("\n");
}

void print_Decl (Decl *decl, int level)
{
    switch (decl->type) {
        case DeclVar:
            printf ("Variable Decl ");
            print_Type (decl->u.dv.type, level);
            print_NameList (decl->u.dv.names, level);
        break;
        case DeclFunc:
            printf ("Function Decl - Return Type: ");
            print_Type (decl->u.df.type, level);
            printf ("\n              - Paramenters ");
            print_Params (decl->u.df.params, level);
            print_Block (decl->u.df.block, level);
        break;
        default:
            error ("Invalid Declaration");
        break;
    }
}

void dump_Program (Program *program, int level)
{
    Decl *decl;

    if (program == NULL) {
        printf ("Empty Program");
        return;
    }

    decl = program->decl;

    while (decl) {
        print_Decl (decl, level);
        decl = decl->next;
    }

    printf ("\n");
}

#ifdef DUMP_TEST
int main (int argc, char **argv)
{
    indent = 0;
    Cmd *cmd = malloc (sizeof (Cmd));
    Exp *exp = malloc (sizeof (Exp));
    Var *var = malloc (sizeof (Var));
    Program *program = new_Program ();

    Decl *decl;
    NameList *name_list;
    Type *type;


    cmd->type = CmdIf;
    cmd->u.cif.cond = NULL;
    cmd->u.cif.then = NULL;
    cmd->u.cif._else = NULL;
    print_Cmd (cmd, 0);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdWhile;
    cmd->u.cw.cond = NULL;
    cmd->u.cw.body = NULL;
    print_Cmd (cmd, 0);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdAss;
    cmd->u.ca.var = NULL;
    cmd->u.ca.exp = NULL;
    print_Cmd (cmd, 0);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdRet;
    cmd->u.cr.exp = NULL;
    print_Cmd (cmd, 0);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdCall;
    cmd->u.cc.name = NULL;
    cmd->u.cc.exp = NULL;
    print_Cmd (cmd, 0);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdBlock;
    cmd->u.cb.block = NULL;
    print_Cmd (cmd, 0);
    free (cmd);
    printf ("\n");

    exp->type = ExpConstInt;
    exp->u.eci.val = 1000;
    print_Exp (exp, 0);
    free (exp);
    printf ("\n");

    exp = malloc (sizeof (Exp));
    exp->type = ExpConstFloat;
    exp->u.ecf.val = 3.0;
    print_Exp (exp, 0);
    free (exp);
    printf ("\n");

    exp = malloc (sizeof (Exp));
    exp->type = ExpConstString;
    exp->u.ecs.val = "const string";
    print_Exp (exp, 0);
    free (exp);
    printf ("\n");

    var->type = VarSingle;
    var->u.vs.id = "VarName";
    print_Var (var, 0);
    free (var);
    printf ("\n");

    exp = malloc (sizeof (Exp));
    exp->type = ExpConstString;
    exp->u.ecs.val = "const string";
    var->type = VarArray;
    var->u.va.id = "VarName";
    var->u.va.exp = exp;
    print_Var (var, 0);
    printf ("\n");

    exp = malloc (sizeof (Exp));
    exp->type = ExpConstInt;
    exp->u.eci.val = 0;
    var = malloc (sizeof (Var));
    var->type = VarArray;
    var->u.va.id = "array";
    var->u.va.exp = exp;
    print_Var (var, 0);


    program->decl = NULL;

    printf ("\n\n");
    return 0;
}
#endif
