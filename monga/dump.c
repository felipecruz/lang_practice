#include <stdio.h>
#include <stdlib.h>
#include "astnodes.h"

static int indent = 0;
static char spaces[2] = "  ";

void error (const char *message)
{
    printf ("Error %s\n", message);
    exit(-1);
}

void print_Exp (Exp *exp, int level)
{
    printf ("EXP: ");
    switch (exp->type) {
        case ExpConstInt:
            printf ("Const int: %d", exp->u.eci.val);
            break;
        case ExpConstFloat:
            printf ("Const float: %f", exp->u.ecf.val);
            break;
        case ExpConstString:
            printf ("Const string: %s", exp->u.ecs.val);
            break;
        default:
            printf ("Invalid Expression");
            break;
    }
}

void print_Cmd (Cmd *cmd, int level)
{
    switch (cmd->type) {
        case CmdIf:
            printf ("IF");
            break;
        case CmdWhile:
            printf ("WHILE");
            break;
        case CmdAss:
            printf ("ASSGINMENT");
            break;
        case CmdRet:
            printf ("RETURN");
            break;
        case CmdCall:
            printf ("CALL");
            break;
        case CmdBlock:
            printf ("BLOCK");
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

void print_Type (Type *type)
{
    switch (type->type) {
        case TypeInt:
            printf ("Int Type\n");
            break;
        case TypeChar:
            printf ("Char Type\n");
            break;
        case TypeFloat:
            printf ("Float Type\n");
            break;
        default:
            error ("Invalid Type\n");
    } 

    if (type->array)
        printf ("Array\n");
}

void print_NameList (NameList *name_list)
{
    NameList *list = name_list;
    while (list) {
        printf ("Name %s ", list->id);
        list = list->next;
    }
    printf ("\n");
}

void print_Block (Block *block)
{
    
}

void print_Params (Params *params)
{

}

void print_Decl (Decl *decl)
{
    switch (decl->type) {
        case DeclVar:
            printf ("Declaration: Variable:");
            print_Type (decl->u.dv.type);
            print_NameList (decl->u.dv.names);
        break;
        case DeclFunc:
            printf ("Declaration: Function:");
            print_Type (decl->u.df.type);
            print_Params (decl->u.df.params);
            print_Block (decl->u.df.block);
        break;
        default:
            error ("Invalid Declaration");
        break;
    }
}

void dump_Program (Program *program)
{
    Decl *decl;

    if (program == NULL) {
        printf ("Empty Program");
        return;
    }

    decl = program->decl;

    while (decl) {
        print_Decl (decl);
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
