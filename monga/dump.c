#include <stdio.h>
#include <stdlib.h>
#include "astnodes.h"

static int indent = 0;

void print_Cmd (Cmd *cmd) {
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

#ifdef DUMP_TEST
int main (int argc, char **argv)
{
    indent = 0;

    Cmd *cmd = malloc (sizeof (Cmd));
    cmd->type = CmdIf;
    cmd->u.cif.cond = NULL;
    cmd->u.cif.then = NULL;
    cmd->u.cif._else = NULL;
    print_Cmd (cmd);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdWhile;
    cmd->u.cw.cond = NULL;
    cmd->u.cw.body = NULL;
    print_Cmd (cmd);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdAss;
    cmd->u.ca.var = NULL;
    cmd->u.ca.exp = NULL;
    print_Cmd (cmd);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdRet;
    cmd->u.cr.exp = NULL;
    print_Cmd (cmd);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdCall;
    cmd->u.cc.name = NULL;
    cmd->u.cc.exp = NULL;
    print_Cmd (cmd);
    free (cmd);
    printf ("\n");

    cmd = malloc (sizeof (Cmd));
    cmd->type = CmdBlock;
    cmd->u.cb.block = NULL;
    print_Cmd (cmd);
    free (cmd);
    printf ("\n");

    printf ("\n\n");
    return 0;
}
#endif
