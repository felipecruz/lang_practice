#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "astnodes.h"
#include "dump.h"

static int _label = 0;

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

char *get_label (int _label)
{
    char *label = malloc (sizeof (char) * 5);
    sprintf (label, "L%d", _label);
    return label;
}

int new_label ()
{
    return _label++;
}

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

void jmp_if_false (Exp *exp, int label)
{
    int l1;

    switch (exp->type) {
        case ExpConstInt:
            if (!exp->u.eci.val)
                printf ("    jmp %s\n", get_label (label));
            break;
        case ExpConstLong:
            if (!exp->u.ech.val);
                printf ("    jmp %s\n", get_label (label));
            break;
        case UnaExpArith:
            switch (exp->u.eu.op) {
                case UnaArith_Minus:
                    //TODO completar
                    break;
                case UnaArith_Log_Neg:
                    jmp_if_true (exp->u.eu.exp, label);
                    break;
                default:
                    printf ("Unknow Unary Arithmetic Expression");

            }
            break;
        case BinExpArith:
            switch (exp->u.eb.op) {
                case Arith_Log_And:
                    jmp_if_false (exp->u.eb.exp1, label);
                    jmp_if_false (exp->u.eb.exp2, label);
                    break;
                case Arith_Log_Or:
                    l1 = new_label ();
                    jmp_if_true (exp->u.eb.exp1, l1);
                    jmp_if_false (exp->u.eb.exp2, label);
                    printf ("%s", get_label (l1));
                    break;
                case Arith_Dbl_EQ:
                case Arith_Lte:
                case Arith_Gte:
                case Arith_Ge:
                case Arith_Lt:
                    export_value (exp->u.eb.exp1);
                    printf ("    push %%eax\n");
                    export_value (exp->u.eb.exp2);
                    printf ("    pop %%ecx\n");
                    printf ("    cmp %%eax, %%ecx\n");

                    printf ("Finish JUMPS");

                    break;
                case Arith_Plus:
                case Arith_Sub:

                    export_value (exp);
                    printf ("    mov $0, %%eax\n");
                    printf ("    jne %s\n", get_label (label));

                default:
                    printf ("Unsupported");
                    break;
            }
            break;
        case ExpConstFloat:
        case ExpConstString:
        case ExpVar:
        case ExpCall:
        case ExpNew:
            printf ("Unssuported\n");

    }
}

void jmp_if_true (Exp *exp, int label)
{
    int l1, l2;

    switch (exp->type) {
        case ExpConstInt:
            if (exp->u.eci.val)
                printf ("    jmp %s\n", get_label (label));
            break;
        case ExpConstLong:
            if (exp->u.ech.val);
                printf ("    jmp %s\n", get_label (label));
            break;
        case UnaExpArith:
            switch (exp->u.eu.op) {
                case UnaArith_Minus:
                    //TODO completar
                    break;
                case UnaArith_Log_Neg:
                    jmp_if_false (exp->u.eu.exp, label);
                    break;
                default:
                    printf ("Unknow Unary Arithmetic Expression");
            }
            //print_Exp (exp->u.eu.exp);
            break;
        case BinExpArith:
            switch (exp->u.eb.op) {
                case Arith_Log_And:
                    l1 = new_label ();
                    jmp_if_false (exp->u.eb.exp1, l1);
                    jmp_if_true (exp->u.eb.exp2, label);
                    printf ("%s", get_label (l1));
                    break;
                case Arith_Log_Or:
                    jmp_if_true (exp->u.eb.exp1, label);
                    jmp_if_true (exp->u.eb.exp2, label);
                    break;
                case Arith_Dbl_EQ:
                case Arith_Lte:
                case Arith_Gte:
                case Arith_Ge:
                case Arith_Lt:
                    export_value (exp->u.eb.exp1);
                    printf ("    push %%eax\n");
                    export_value (exp->u.eb.exp2);
                    printf ("    pop %%ecx\n");
                    printf ("    cmp %%eax, %%ecx\n");

                    printf ("Finish JUMPS");

                    break;
                case Arith_Plus:
                case Arith_Sub:

                    export_value (exp);
                    printf ("    mov $0, %%eax\n");
                    printf ("    jne %s\n", get_label (label));

                default:
                    printf ("Unsupported");
                    break;
            }
            break;
        case ExpConstFloat:
        case ExpConstString:
        case ExpVar:
        case ExpCall:
        case ExpNew:
            printf ("Unssuported\n");
    }
}

void export_to_int (Exp *exp)
{
    assert (exp->type == ExpConstInt || exp->type == ExpConstLong);

    switch (exp->type) {
        case ExpConstInt:
            printf ("    mov $%d, %%eax\n", exp->u.eci.val);
            break;
        case ExpConstLong:
            printf ("    mov $%ld, %%eax\n", exp->u.ech.val);
            break;
    }
}

void export_value (Exp *exp)
{
    int l1, l2;

    switch (exp->type) {
        case ExpConstInt:
            export_to_int (exp);
            break;
        case ExpConstLong:
            export_to_int (exp);
            break;
        case ExpConstFloat:
            printf ("\nFloat not implemented\n - %f", exp->u.ecf.val);
            break;
        case ExpConstString:
            // TODO Expressões String
            break;
        case ExpVar:
            // TODO Var
            break;
        case ExpCall:
            // TODO Call
            break;
        case ExpNew:
            // TODO New
            break;
        case UnaExpArith:
            switch (exp->u.eu.op) {
                case UnaArith_Minus:
                //    printf ("Unary Exp [ - ] ");
                    break;
                case UnaArith_Log_Neg:
              //      printf ("Unary Exp  [ ! ] ");
                    break;
                default:
                    printf ("Unknow Unary Arithmetic Expression");

            }
            //print_Exp (exp->u.eu.exp);
            break;
        case BinExpArith:
            switch (exp->u.eb.op) {
                case Arith_Plus:
                    export_to_int (exp->u.eb.exp1);
                    printf ("    push %%eax\n");
                    export_to_int (exp->u.eb.exp2);
                    printf ("    pop %%exc\n");
                    printf ("    add %%exc, %%eax\n");
                    break;
                case Arith_Sub:
                    export_to_int (exp->u.eb.exp1);
                    printf ("    push %%eax\n");
                    export_to_int (exp->u.eb.exp2);
                    printf ("    pop %%exc\n");
                    printf ("    sub %%exc, %%eax\n");
                    break;
                case Arith_Dbl_EQ:
                case Arith_Log_And:
                case Arith_Log_Or:
                default:
                    printf ("Unssuported\n");
                    break;
            }
            break;
        default:
            printf ("Invalid Expression");
            break;
    }
}

void generate_var (Var *var)
{
    /* Global */
    if (var->decl->_offset == 0) {
        printf ("    lea %s, %%eax\n", get_var_id (var));
    } else {
        switch (var->type) {
            case VarSingle:
                printf ("    lea %d[%%ebp], %%eax\n", var->decl->_offset);
                break;
            case VarArray:
                export_value (var->u.va.prefix_exp);
                printf ("    push %%eax\n");
                export_value (var->u.va.exp);
                printf ("    pop %%ecx\n");
                printf ("    lea (%%ecx, %%eax,%d), %%eax\n",
                        type_size (var->decl->u.dv.type));
                break;
        }
    }
}

void generate_command (Cmd *cmd)
{
    switch (cmd->type) {
        case CmdAss:
            //TODO Depende do tipo movlb ou movc (4 ou 1 byte)
            generate_var (cmd->u.ca.var);
            printf ("    push %%eax\n");
            export_value (cmd->u.ca.exp);
            printf ("    pop %%ecx\n");
            printf ("    mov %%eax, [%%ecx]\n");
            break;
        case CmdRet:
            export_value (cmd->u.cr.exp);
            break;
        case CmdIf:
        case CmdWhile:
        case CmdCall:
        case CmdBlock:
        default:
            printf ("Unssuported\n");
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
    Cmd *cmd;

    if (!decl)
        return;

    printf ("\n    .globl %s\n", decl->u.df.id);
    printf ("%s:\n", decl->u.df.id);
    printf ("    push %%ebp\n");
    printf ("    mov %%esp, %%ebp\n");

    /* calcula offset das variáveis locais */

    block = decl->u.df.block;
    decl = block->decl;
    while (decl) {
        offset = offset + type_size (decl->u.dv.type);
        decl = decl->next;
    }

    printf ("    sub $%d, %%esp\n", offset);

    // TODO miolo

    cmd = block->cmd;
    while (cmd) {
        generate_command (cmd);
        cmd = cmd->next;
    }

    // Saída

    printf ("    mov %%ebp, %%esp\n");
    printf ("    pop %%ebp\n");

    // TODO código do retorno com valor/var/exp - return x;
    printf ("    ret\n"); // return ;
}

void generate_Program (Program *program)
{
    Decl *decl;

    if (program == NULL) {
        printf ("Empty Program");
        return;
    }

    printf (".data\n");

    decl = program->decl;

    while (decl) {
        if (decl->type == DeclVar)
            generate_globals_Decl (decl);
        decl = decl->next;
    }

    decl = program->decl;

    printf ("\n.text\n");

    //TODO Verificar se tem main

    while (decl) {
        if (decl->type == DeclFunc)
            generate_functions_Decl (decl);
        decl = decl->next;
    }

    printf ("\n");
}
