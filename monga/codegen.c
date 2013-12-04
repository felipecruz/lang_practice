#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "typesystem.h"
#include "astnodes.h"
#include "dump.h"

static int _label = 0;

char *int_operand (int value)
{
    char *name = malloc (20);
    sprintf (name, "$%d", value);
    return name;
}

char *operand(char *nme, int is_value, int offset)
{
    char *name = malloc (20);
    if (!is_value && !offset)
        sprintf (name, "%%%s", nme);
    else if (is_value && !offset)
        sprintf (name, "(%%%s)", nme);
    else if (offset != 0)
        sprintf (name, "%d(%%%s)", offset, nme);
    return name;
}

char *leal_operand (char *n1, char *n2, int size)
{
    char *name = malloc (20);
    sprintf (name, "(%s, %s, %d)", n1, n2, size);
    return name;
}

Instruction *new_inst (char *op, char *src, char *dst)
{
    Instruction *instruction = malloc (sizeof (Instruction));
    instruction->op = op;
    instruction->src = src;
    instruction->dst = dst;
}

void generate_assembly (Instruction *inst, const char *cmt)
{
    if (!inst->dst)
        printf ("    %s %s          #%s\n", inst->op, inst->src, cmt);
    else
        printf ("    %s %s, %s        #%s\n", inst->op, inst->src, inst->dst, cmt);
    free (inst->src);
    if (!inst->dst)
        free (inst->dst);
    free (inst);
}

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
                    generate_expression (exp->u.eb.exp1);
                    printf ("    push %%eax\n");
                    generate_expression (exp->u.eb.exp2);
                    printf ("    pop %%ecx\n");
                    printf ("    cmp %%eax, %%ecx\n");

                    printf ("Finish JUMPS");

                    break;
                case Arith_Plus:
                case Arith_Sub:

                    generate_expression (exp);
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
                    generate_expression (exp->u.eb.exp1);
                    printf ("    push %%eax\n");
                    generate_expression (exp->u.eb.exp2);
                    printf ("    pop %%ecx\n");
                    printf ("    cmp %%eax, %%ecx\n");

                    printf ("Finish JUMPS");

                    break;
                case Arith_Plus:
                case Arith_Sub:

                    generate_expression (exp);
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
    Instruction *inst;

    assert (exp->type == ExpConstInt || exp->type == ExpConstLong);

    switch (exp->type) {
        case ExpConstInt:
            inst = new_inst (MOV, int_operand (exp->u.eci.val),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "Int const Exp");
            break;
        case ExpConstLong:
            inst = new_inst (MOV, int_operand (exp->u.ech.val),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "Int const Exp");
            break;
    }
}

int is_var (Exp *exp)
{
    if (exp->type == ExpVar)
        return 1;
    return 0;
}

int is_int_const (Exp *exp)
{
    if (exp->type == ExpConstInt || exp->type == ExpConstLong)
        return 1;
    return  0;
}

void generate_expression_val (Exp *exp)
{
    char *cmt = malloc (50);
    Var *var = exp->u.ev.var;
    Instruction *inst;

    inst = new_inst (MOV, operand (EBP, 1, var->decl->_offset),
                          operand (EAX, 0, 0));

    sprintf (cmt, "Local %s", var->decl->u.dv.id);
    generate_assembly (inst, cmt);

    //printf ("    movl %d(%%ebp), %%eax       # local %s\n",
    //        var->decl->_offset, var->decl->u.dv.id);
    free (cmt);
}

void generate_bin_exp (Exp *exp)
{
    Instruction *inst;
    int left_val = 0;

    if (exp->u.eb.exp1->type == BinExpArith &&
        exp->u.eb.exp2->type == BinExpArith) {
        printf ("\n");
        generate_bin_exp (exp->u.eb.exp1);

        inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
        generate_assembly (inst, "left exp");

        generate_bin_exp (exp->u.eb.exp2);

        inst = new_inst (POP, operand (ECX, 0, 0), NULL);
        generate_assembly (inst, "");


        printf ("            # end Binary expression\n");
    } else if (exp->u.eb.exp1->type == BinExpArith &&
               exp->u.eb.exp2->type != BinExpArith) {
        printf ("# Left -> BinExp\n");
        generate_bin_exp (exp->u.eb.exp1);

        inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
        generate_assembly (inst, "left exp");

        if (!is_int_const (exp->u.eb.exp2))
            generate_expression_val (exp->u.eb.exp2);
        else
            generate_expression (exp->u.eb.exp2);

        inst = new_inst (POP, operand (ECX, 0, 0), NULL);
        generate_assembly (inst, "");


        printf (" # end Binary expression\n");
        return;
    } else if (exp->u.eb.exp1->type != BinExpArith &&
               exp->u.eb.exp2->type == BinExpArith) {
        printf ("# Right -> BinExp\n");

        if (!is_int_const (exp->u.eb.exp1))
            generate_expression_val (exp->u.eb.exp1);
        else
            generate_expression (exp->u.eb.exp1);

        inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
        generate_assembly (inst, "left exp");

        generate_bin_exp (exp->u.eb.exp2);

        inst = new_inst (POP, operand (ECX, 0, 0), NULL);
        generate_assembly (inst, "");


        printf ("# fim bin exp 2 - result em EAX\n");
        return;
    } else if (!is_int_const (exp->u.eb.exp1) &&
        !is_int_const (exp->u.eb.exp2)) {
        generate_expression (exp->u.eb.exp1);

        inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
        generate_assembly (inst, "left exp");

        generate_expression_val (exp->u.eb.exp2);

        inst = new_inst (POP, operand (ECX, 0, 0), NULL);
        generate_assembly (inst, "");

        left_val = 1;


    } else if (!is_int_const (exp->u.eb.exp1) &&
               is_int_const (exp->u.eb.exp2)) {
        generate_expression_val (exp->u.eb.exp1);

        inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
        generate_assembly (inst, "left exp");

        generate_expression (exp->u.eb.exp2);

        inst = new_inst (POP, operand (ECX, 0, 0), NULL);
        generate_assembly (inst, "");


    } else if (is_int_const (exp->u.eb.exp1) &&
               !is_int_const (exp->u.eb.exp2)) {
        generate_expression (exp->u.eb.exp1);

        inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
        generate_assembly (inst, "left exp");

        generate_expression_val (exp->u.eb.exp2);

        inst = new_inst (POP, operand (ECX, 0, 0), NULL);
        generate_assembly (inst, "");


    } else if (is_int_const (exp->u.eb.exp1) &&
               is_int_const (exp->u.eb.exp2)) {
        generate_expression (exp->u.eb.exp1);

        inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
        generate_assembly (inst, "left exp");

        generate_expression (exp->u.eb.exp2);

        inst = new_inst (POP, operand (ECX, 0, 0), NULL);
        generate_assembly (inst, "");


    } else {
        printf ("Unssuported\n");
    }

    switch (exp->u.eb.op) {
        case Arith_Plus:
            inst = new_inst (ADD, operand (ECX, left_val, 0),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case Arith_Sub:
            inst = new_inst (SUB, operand (ECX, left_val, 0),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case Arith_Mul:
            inst = new_inst (IMUL, operand (ECX, left_val, 0),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case Arith_Div:
            inst = new_inst (IDIV, operand (ECX, left_val, 0),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case Arith_Dbl_EQ:
        case Arith_Log_And:
        case Arith_Log_Or:
        default:
            printf ("Unssuported\n");
            break;
    }
}

void generate_expression (Exp *exp)
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
            generate_var (exp->u.ev.var);
            break;
        case ExpCall:
            generate_call (exp->u.ec.call);
            break;
        case ExpNew:
            generate_expression (exp->u.en.exp);
            printf ("    pushl %%eax\n");
            printf ("    movl $%d, %%eax\n", type_size (exp->u.en.type));
            printf ("    popl %%ecx\n");
            printf ("    imull %%ecx, %%eax\n");
            printf ("    call malloc\n");
            break;
        case UnaExpArith:
            switch (exp->u.eu.op) {
                case UnaArith_Minus:
                    //TODO Verificar se pode usar imul $-1, %eax
                    generate_expression (exp->u.eu.exp);
                    printf ("    imull $-1, %%eax\n");
                    break;
                case UnaArith_Log_Neg:
                default:
                    printf ("Unknow Unary Arithmetic Expression");

            }
            break;
        case BinExpArith:
            generate_bin_exp (exp);
            break;
        default:
            printf ("Invalid Expression");
            break;
    }
}

void reverse_exp_list (Exp **exp)
{
    Exp *prev = NULL;
    Exp *current = *exp;
    Exp *next;

    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    *exp = prev;
}

void generate_call (Call *call)
{
    int stack_clean_val = 0;
    Type *type;
    Exp *exp = call->exp_list;
    reverse_exp_list (&exp);
    printf ("                             # Chamada\n");

    while (exp) {
        type = get_exp_type (exp);
        stack_clean_val += type_size (type);
        generate_expression (exp);
        if (is_var (exp))
            printf ("    push (%%eax)\n");
        else
            printf ("    push %%eax\n");
        exp = exp->next;
    }

    if (call->decl->u.df._extern)
        printf ("    call %s\n", call->id);
    else
        printf ("    call %s\n", call->id);
    printf ("    addl $%d, %%esp\n", stack_clean_val);
    printf ("                            # Fim Chamada\n");
}

void generate_var (Var *var)
{
    /* Global */
    if (var->decl->_offset == 0) {
        printf ("    lea %s, %%eax\n", get_var_id (var));
    } else {
        switch (var->type) {
            case VarSingle:
                printf ("    lea %d(%%ebp), %%eax       # local %s\n",
                        var->decl->_offset, var->decl->u.dv.id);
                break;
            case VarArray:
                printf ("    lea %d(%%ebp), %%eax       # local %s\n",
                        var->decl->_offset, var->decl->u.dv.id);
                printf ("    push %%eax\n");
                generate_expression (var->u.va.exp);
                printf ("    pop %%ecx\n");
                printf ("    lea (%%ecx, %%eax,%d), %%eax\n    # local %s\n",
                        type_size (var->decl->u.dv.type), var->decl->u.dv.id);
                break;
        }
    }
}

void generate_command (Cmd *cmd)
{
    switch (cmd->type) {
        case CmdAss:
            //TODO Depende do tipo movlb ou movc (4 ou 1 byte)
            printf ("                      # Atribuicao\n");
            generate_var (cmd->u.ca.var);
            printf ("    push %%eax\n");
            generate_expression (cmd->u.ca.exp);
            printf ("    pop %%ecx\n");
            printf ("    mov %%eax, (%%ecx)\n");
            printf ("                      # fim Atribuicao\n");
            break;
        case CmdRet:
            if (!is_int_const (cmd->u.cr.exp))
                generate_expression_val (cmd->u.cr.exp);
            else
                generate_expression (cmd->u.cr.exp);
            break;
        case CmdIf:
        case CmdWhile:
        case CmdCall:
            generate_call (cmd->u.cc.call);
            break;
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

    printf ("    int_pattern: .ascii \"%%d\\12\\0\"\n");
    printf ("    string_pattern: .ascii \"%%s\\12\\0\"\n");

    decl = program->decl;

    while (decl) {
        if (decl->type == DeclVar)
            generate_globals_Decl (decl);
        decl = decl->next;
    }

    decl = program->decl;

    printf ("\n.text\n");

    printf (".globl print_int\n");
    printf ("print_int:\n");
	printf ("    pushl	%%ebp\n");
	printf ("    movl	%%esp, %%ebp\n");
    printf ("    movl 8(%%ebp), %%eax\n");
    printf ("    push %%eax\n");
    printf ("    lea int_pattern, %%eax\n");
    printf ("    push %%eax\n");
    printf ("    call printf\n");
    printf ("    add $8, %%esp\n");
    printf ("    leave\n");
    printf ("    ret\n");

    while (decl) {
        if (decl->type == DeclFunc && !decl->u.df._extern)
            generate_functions_Decl (decl);
        decl = decl->next;
    }

    printf ("\n");
}
