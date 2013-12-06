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

char *label_operand (char *label)
{
    char *name = malloc (20);
    sprintf (name, "%s", label);
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

char *imul_operand (int val, char *n1)
{
    char *name = malloc (20);
    sprintf (name, "$%d, %%%s", val, n1);
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
        printf ("    %-8s %-20s #%s\n", inst->op, inst->src, cmt);
    else
        printf ("    %-8s %-12s, %-6s #%s\n", inst->op, inst->src, inst->dst, cmt);
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

void jmp_if_false_binexp (Exp *exp, int label)
{
    int l1;
    Instruction *inst;

    switch (exp->u.eb.op) {
        case Arith_Log_And:
            jmp_if_false (exp->u.eb.exp1, label);
            jmp_if_false (exp->u.eb.exp2, label);
            return;
        case Arith_Log_Or:
            l1 = new_label ();
            jmp_if_true (exp->u.eb.exp1, l1);
            jmp_if_false (exp->u.eb.exp2, label);
            printf ("%s:\n", get_label (l1));
            return;
    }

    switch (exp->u.eb.op) {
        case Arith_Mul:
        case Arith_Div:
        case Arith_Plus:
        case Arith_Sub:
            generate_expression (exp);
            inst = new_inst (MOV, int_operand (0), operand (ECX, 0, 0));
            generate_assembly (inst, "");
            inst = new_inst (CMP, operand (EAX, 0, 0), operand (ECX, 0, 0));
            generate_assembly (inst, "");
            inst = new_inst (JE, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            return;
    }

    generate_expression (exp->u.eb.exp1);
    inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
    generate_assembly (inst, "");
    generate_expression (exp->u.eb.exp2);
    inst = new_inst (POP, operand (ECX, 0, 0), NULL);
    generate_assembly (inst, "");
    inst = new_inst (CMP, operand (EAX, 0, 0), operand (ECX, 0, 0));
    generate_assembly (inst, "");

    switch (exp->u.eb.op) {
        case Arith_Dbl_EQ:
            inst = new_inst (JNE, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
        case Arith_Lte:
            inst = new_inst (JG, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
        case Arith_Gte:
            inst = new_inst (JL, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
        case Arith_Gt:
            inst = new_inst (JLE, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
        case Arith_Lt:
            inst = new_inst (JGE, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
    }

}

void jmp_if_false (Exp *exp, int label)
{
    int l1;
    Instruction *inst;

    switch (exp->type) {
        case ExpConstInt:
            if (!exp->u.eci.val) {
                inst = new_inst (JMP, label_operand (get_label (label)), NULL);
                generate_assembly (inst, "");
            }
            return;
        case ExpConstLong:
            if (!exp->u.ech.val) {
                inst = new_inst (JMP, label_operand (get_label (label)), NULL);
                generate_assembly (inst, "");
            }
            return;
    }

    switch (exp->type) {
        case UnaExpArith:
            if (exp->u.eu.op == UnaArith_Log_Neg) {
                jmp_if_true (exp->u.eu.exp, label);
                return;
            }
        case BinExpArith:
            jmp_if_false_binexp (exp, label);
            return;
    }

    switch (exp->type) {
        case UnaExpArith:
            if (exp->u.eu.op == UnaArith_Minus)
                generate_expression (exp->u.eu.exp);
            break;
        case ExpVar:
            generate_var (exp->u.ev.var);
            inst = new_inst (MOV, operand (EAX, 1, 0), operand (EAX, 0, 0));
            break;
        case ExpCall:
            generate_call (exp->u.ec.call);
            break;
        case ExpConstFloat:
        case ExpConstString:
        case ExpNew:
            printf ("Unssuported\n");

    }

    inst = new_inst (MOV, int_operand (0), operand (ECX, 0, 0));
    generate_assembly (inst, "");
    inst = new_inst (CMP, operand (EAX, 0, 0), operand (ECX, 0, 0));
    generate_assembly (inst, "");
    inst = new_inst (JE, label_operand (get_label (label)), NULL);
    generate_assembly (inst, "");
}

void jmp_if_true_binexp (Exp *exp, int label)
{
    int l1;
    Instruction *inst;

    switch (exp->u.eb.op) {
        case Arith_Log_And:
            l1 = new_label ();
            jmp_if_false (exp->u.eb.exp1, l1);
            jmp_if_true (exp->u.eb.exp2, label);
            printf ("%s:\n", get_label (l1));
            return;
        case Arith_Log_Or:
            jmp_if_true (exp->u.eb.exp1, label);
            jmp_if_true (exp->u.eb.exp2, label);
            return;
    }

    switch (exp->u.eb.op) {
        case Arith_Mul:
        case Arith_Div:
        case Arith_Plus:
        case Arith_Sub:
            generate_expression (exp);
            inst = new_inst (MOV, int_operand (0), operand (ECX, 0, 0));
            generate_assembly (inst, "");
            inst = new_inst (CMP, operand (EAX, 0, 0), operand (ECX, 0, 0));
            generate_assembly (inst, "");
            inst = new_inst (JNE, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            return;
    }

    generate_expression (exp->u.eb.exp1);
    inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
    generate_assembly (inst, "");
    generate_expression (exp->u.eb.exp2);
    inst = new_inst (POP, operand (ECX, 0, 0), NULL);
    generate_assembly (inst, "");
    inst = new_inst (CMP, operand (EAX, 0, 0), operand (ECX, 0, 0));
    generate_assembly (inst, "");

    switch (exp->u.eb.op) {
        case Arith_Dbl_EQ:
            inst = new_inst (JE, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
        case Arith_Lte:
            inst = new_inst (JLE, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
        case Arith_Gte:
            inst = new_inst (JGE, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
        case Arith_Gt:
            inst = new_inst (JG, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
        case Arith_Lt:
            inst = new_inst (JL, label_operand (get_label (label)), NULL);
            generate_assembly (inst, "");
            break;
    }
}

void jmp_if_true (Exp *exp, int label)
{
    int l1, l2;
    Instruction *inst;

    switch (exp->type) {
        case ExpConstInt:
            if (exp->u.eci.val) {
                inst = new_inst (JMP, label_operand (get_label (label)), NULL);
                generate_assembly (inst, "");
            }
            return;
        case ExpConstLong:
            if (exp->u.ech.val) {
                inst = new_inst (JMP, label_operand (get_label (label)), NULL);
                generate_assembly (inst, "");
            }
            return;
    }

    switch (exp->type) {
        case UnaExpArith:
            if (exp->u.eu.op == UnaArith_Log_Neg) {
                jmp_if_false (exp->u.eu.exp, label);
                return;
            }
        case BinExpArith:
            jmp_if_true_binexp (exp, label);
            return;
    }

    switch (exp->type) {
        case UnaExpArith:
            if (exp->u.eu.op == UnaArith_Minus)
                generate_expression (exp->u.eu.exp);
            break;
        case ExpVar:
            generate_var (exp->u.ev.var);
            inst = new_inst (MOV, operand (EAX, 1, 0), operand (EAX, 0, 0));
            break;
        case ExpCall:
            generate_call (exp->u.ec.call);
            break;
        case ExpConstFloat:
        case ExpConstString:
        case ExpNew:
            printf ("Unssuported\n");

    }

    inst = new_inst (MOV, int_operand (0), operand (ECX, 0, 0));
    generate_assembly (inst, "");
    inst = new_inst (CMP, operand (EAX, 0, 0), operand (ECX, 0, 0));
    generate_assembly (inst, "");
    inst = new_inst (JNE, label_operand (get_label (label)), NULL);
    generate_assembly (inst, "");
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

void generate_bin_exp (Exp *exp)
{
    int l1, l2;
    Instruction *inst;

    printf ("\n");

    if (exp->u.eb.exp1->type == BinExpArith)
        generate_bin_exp (exp->u.eb.exp1);
    else
        generate_expression (exp->u.eb.exp1);

    inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
    generate_assembly (inst, "left exp");

    if (exp->u.eb.exp2->type == BinExpArith)
        generate_bin_exp (exp->u.eb.exp2);
    else
        generate_expression (exp->u.eb.exp2);

    inst = new_inst (POP, operand (ECX, 0, 0), NULL);
    generate_assembly (inst, "");

    switch (exp->u.eb.op) {
        case Arith_Plus:
            inst = new_inst (ADD, operand (ECX, 0, 0),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case Arith_Sub:
            inst = new_inst (SUB, operand (EAX, 0, 0),
                                  operand (ECX, 0, 0));
            generate_assembly (inst, "");
            inst = new_inst (MOV, operand (ECX, 0, 0),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case Arith_Mul:
            inst = new_inst (IMUL, operand (ECX, 0, 0),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case Arith_Div:
            inst = new_inst (IDIV, operand (ECX, 0, 0),
                                   operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case Arith_Lte:
        case Arith_Gte:
        case Arith_Gt:
        case Arith_Lt:
        case Arith_Dbl_EQ:
            l1 = new_label ();
            jmp_if_false (exp, l1);
            inst = new_inst (MOV, int_operand (1),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");
            l2 = new_label ();
            inst = new_inst (JMP, label_operand (get_label (l2)), NULL);
            generate_assembly (inst, "");
            
            // TODO remover printf
            printf ("%s:\n", get_label (l1));
            
            inst = new_inst (MOV, int_operand (0),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");

            // TODO remover printf
            printf ("%s:\n", get_label (l2));

            inst = new_inst (CMP, operand (ECX, 0, 0),
                                  operand (EAX, 0, 0));


            generate_assembly (inst, "");
            break;
        case Arith_Log_And:
            inst = new_inst (AND, operand (ECX, 0, 0),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case Arith_Log_Or:
            inst = new_inst (OR, operand (ECX, 0, 0),
                                 operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        default:
            printf ("Unssuportedddd\n");
            break;
    }

    printf ("\n");
}

void generate_expression (Exp *exp)
{
    int l1, l2;
    Instruction *inst;

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
            inst = new_inst (MOV, operand (EAX, 1, 0), operand (EAX, 0, 0));
            generate_assembly (inst, "");
            break;
        case ExpCall:
            generate_call (exp->u.ec.call);
            break;
        case ExpNew:
            generate_expression (exp->u.en.exp);
            inst = new_inst (IMUL, imul_operand (type_size (exp->u.en.type), EAX),
                                   operand (EAX, 0, 0));
            generate_assembly (inst, "");
            
            inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
            generate_assembly (inst, "");

            inst = new_inst (CALL, label_operand ("malloc"), NULL);
            generate_assembly (inst, "");
            
            inst = new_inst (ADD, int_operand (4), operand (ESP, 0, 0));
            generate_assembly (inst, "");
            break;
        case UnaExpArith:
            switch (exp->u.eu.op) {
                generate_expression (exp->u.eu.exp);
                case UnaArith_Minus:
                    inst = new_inst (NEG, operand (EAX, 0, 0), NULL);
                    generate_assembly (inst, "");
                    break;
                case UnaArith_Log_Neg:
                    inst = new_inst (NOT, operand (EAX, 0, 0), NULL);
                    generate_assembly (inst, "");
                    break;
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
    printf ("\n");

    while (exp) {
        type = get_exp_type (exp);
        stack_clean_val += type_size (type);
        generate_expression (exp);
        printf ("    push %%eax\n");
        exp = exp->next;
    }
    
    if (call->id == NULL)
        return;

    printf ("    call %s\n", call->id);
    printf ("    addl $%d, %%esp\n", stack_clean_val);
    printf ("\n");
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
                printf ("    movl (%%eax), %%eax\n");
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
    int l1, l2;

    while (cmd) {
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
                generate_expression (cmd->u.cr.exp);
                break;
            case CmdIf:
                l1 = new_label ();
                jmp_if_false (cmd->u.cif.cond, l1);
                generate_command (cmd->u.cif.then);
                l2 = new_label ();
                printf ("    jmp %s\n", get_label (l2));
                printf ("%s:\n", get_label (l1));
                generate_command (cmd->u.cif._else);
                printf ("%s:\n", get_label (l2));
                break;
            case CmdWhile:
            case CmdCall:
                generate_call (cmd->u.cc.call);
                break;
            case CmdBlock:
                generate_command (cmd->u.cb.block->cmd);
                break;
            default:
                printf ("Unssuported\n");
        }
        cmd = cmd->next;
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
    printf ("    sub $%d, %%esp\n", decl->u.df.max_offset);

    cmd = decl->u.df.block->cmd;
    generate_command (cmd);

    printf ("    mov %%ebp, %%esp\n");
    printf ("    pop %%ebp\n");
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

    printf (".globl print_str\n");
    printf ("print_str:\n");
	printf ("    pushl	%%ebp\n");
	printf ("    movl	%%esp, %%ebp\n");
    printf ("    movl 8(%%ebp), %%eax\n");
    printf ("    push %%eax\n");
    printf ("    lea string_pattern, %%eax\n");
    printf ("    push %%eax\n");
    printf ("    call printf\n");
    printf ("    add $8, %%esp\n");
    printf ("    leave\n");
    printf ("    ret\n");

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
