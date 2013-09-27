#include <stdio.h>
#include <stdlib.h>
#include "astnodes.h"

static int indent = 0;
static char spaces[2] = "  ";

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

#ifdef DUMP_TEST
int main (int argc, char **argv)
{
    indent = 0;
    Cmd *cmd = malloc (sizeof (Cmd));
    Exp *exp = malloc (sizeof (Exp));
    Var *var = malloc (sizeof (Var));


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

    printf ("\n\n");
    return 0;
}
#endif
