#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "typesystem.h"
#include "astnodes.h"
#include "dump.h"

static int _label = 0;

void expand_escapes(char* dest, const char* src)
{
  char c;

  while (c = *(src++)) {
    switch(c) {
      case '\a':
        *(dest++) = '\\';
        *(dest++) = 'a';
        break;
      case '\b':
        *(dest++) = '\\';
        *(dest++) = 'b';
        break;
      case '\t':
        *(dest++) = '\\';
        *(dest++) = 't';
        break;
      case '\n':
        *(dest++) = '\\';
        *(dest++) = 'n';
        break;
      case '\v':
        *(dest++) = '\\';
        *(dest++) = 'v';
        break;
      case '\f':
        *(dest++) = '\\';
        *(dest++) = 'f';
        break;
      case '\r':
        *(dest++) = '\\';
        *(dest++) = 'r';
        break;
      case '\\':
        *(dest++) = '\\';
        *(dest++) = '\\';
        break;
      case '\"':
        *(dest++) = '\\';
        *(dest++) = '\"';
        break;
      default:
        *(dest++) = c;
     }
  }

  *dest = '\0'; /* Ensure nul terminator */
}

char* expand_escapes_alloc(const char* src)
{
   char* dest = malloc(2 * strlen(src) + 1);
   expand_escapes(dest, src);
   return dest;
}

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

char *const_operand (char *label)
{
    char *name = malloc (20);
    sprintf (name, "$%s", label);
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
    sprintf (name, "(%%%s, %%%s, %d)", n1, n2, size);
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
    if (!inst->src)
        printf ("    %-31s # %s\n", inst->op, cmt);
    else if (!inst->dst)
        printf ("    %-8s %-23s # %s\n", inst->op, inst->src, cmt);
    else
        printf ("    %-8s %-15s, %-6s # %s\n", inst->op, inst->src, inst->dst, cmt);

    if (inst->src)
        free (inst->src);

    if (inst->dst)
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

int var_size (Var *var)
{
    Type *type = resolve_type (var);
    if (type->array == 1)
        return 4;
    return type_size (type);
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
            generate_assembly (inst, "");
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
            generate_assembly (inst, "");
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

    switch (exp->u.eb.op) {
        case Arith_Div:
            if (exp->u.eb.exp2->type == BinExpArith)
                generate_bin_exp (exp->u.eb.exp2);
            else
                generate_expression (exp->u.eb.exp2);

            inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
            generate_assembly (inst, "left exp");

            if (exp->u.eb.exp1->type == BinExpArith)
                generate_bin_exp (exp->u.eb.exp1);
            else
                generate_expression (exp->u.eb.exp1);

            inst = new_inst (CDQ, NULL, NULL);
            generate_assembly (inst, "");

            inst = new_inst (POP, operand (ECX, 0, 0), NULL);
            generate_assembly (inst, "");

            inst = new_inst (IDIV, operand (ECX, 0, 0), NULL);
            generate_assembly (inst, "");
            return;
    }

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
            l1 = new_label ();
            printf (".data\n");
            printf ("    %s: .string \"%s\\0\"\n", get_label (l1),
                                                  expand_escapes_alloc (exp->u.ecs.val));
            printf (".text\n");
            inst = new_inst (MOV, const_operand (get_label (l1)),
                                  operand (EAX, 0, 0));
            generate_assembly (inst, "global");
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
    Instruction *inst;

    reverse_exp_list (&exp);
    printf ("\n");

    while (exp) {
        type = get_exp_type (exp);
        stack_clean_val += 4;
        generate_expression (exp);
        inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
        generate_assembly (inst, "Push Param");
        exp = exp->next;
    }

    if (call->id == NULL)
        return;

    inst = new_inst (CALL, label_operand (call->id), NULL);
    generate_assembly (inst, "");

    inst = new_inst (ADD, int_operand (stack_clean_val), operand (ESP, 0, 0));
    generate_assembly (inst, "");

    printf ("\n");
}

void generate_var (Var *var)
{
    int offset;
    Instruction *inst;

    /* Global */
    if (var->decl->_offset == 0) {
        inst = new_inst (LEA, label_operand (get_var_id (var)),
                              operand (EAX, 0, 0));
        generate_assembly (inst, "global");
    } else {
        switch (var->type) {
            case VarSingle:
                inst = new_inst (LEA, operand (EBP, 1, var->decl->_offset),
                                      operand (EAX, 0, 0));
                generate_assembly (inst, var->decl->u.dv.id);
                break;
            case VarArray:
                inst = new_inst (LEA, operand (EBP, 1, var->decl->_offset),
                                      operand (EAX, 0, 0));
                generate_assembly (inst, var->decl->u.dv.id);

                inst = new_inst (MOV, operand (EAX, 1, 0), operand (EAX, 0, 0));
                generate_assembly (inst, "");

                inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
                generate_assembly (inst, "");

                generate_expression (var->u.va.exp);

                inst = new_inst (POP, operand (ECX, 0, 0), NULL);
                generate_assembly (inst, "");

                offset = var_size (var);
                inst = new_inst (LEA, leal_operand (ECX, EAX, offset),
                                      operand (EAX, 0, 0));
                generate_assembly (inst, "");
                break;
        }
    }
}

void generate_command (Cmd *cmd)
{
    int l1, l2;
    int size;
    Instruction *inst;
    Decl *decl;
    Type *type;

    while (cmd) {
        switch (cmd->type) {
            case CmdAss:
                //TODO Depende do tipo movlb ou movc (4 ou 1 byte)
                printf ("                                     # Atribuicao\n");
                generate_var (cmd->u.ca.var);

                inst = new_inst (PUSH, operand (EAX, 0, 0), NULL);
                generate_assembly (inst, "");

                generate_expression (cmd->u.ca.exp);

                inst = new_inst (POP, operand (ECX, 0, 0), NULL);
                generate_assembly (inst, "");

                size = var_size (cmd->u.ca.var);

                if (size == 4)
                    inst = new_inst (MOV, operand (EAX, 0, 0), operand (ECX, 1, 0));
                else
                    inst = new_inst (MOVB, operand (AL, 0, 0), operand (ECX, 1, 0));

                generate_assembly (inst, "");
                printf ("                                     # fim Atribuicao\n");
                break;
            case CmdRet:
                printf ("                                     # Retorno\n");
                if (cmd->u.cr.exp != NULL)
                    generate_expression (cmd->u.cr.exp);

                inst = new_inst (MOV, operand (EBP, 0, 0), operand (ESP, 0, 0));
                generate_assembly (inst, "");

                inst = new_inst (POP, operand (EBP, 0, 0), NULL);
                generate_assembly (inst, "");

                inst = new_inst (RET, NULL, NULL);
                generate_assembly (inst, "");
                break;
            case CmdIf:
                printf ("                                     # If\n");
                l1 = new_label ();
                jmp_if_false (cmd->u.cif.cond, l1);
                generate_command (cmd->u.cif.then);
                l2 = new_label ();
                inst = new_inst (JMP, label_operand (get_label (l2)), NULL);
                generate_assembly (inst, "");
                printf ("%s:\n", get_label (l1));
                generate_command (cmd->u.cif._else);
                printf ("%s:\n", get_label (l2));
                break;
            case CmdWhile:
                l1 = new_label ();
                l2 = new_label ();
                inst = new_inst (JMP, label_operand (get_label (l1)), NULL);
                generate_assembly (inst, "");

                printf ("%s:\n", get_label (l2));
                generate_command (cmd->u.cw.body);

                printf ("%s:\n", get_label (l1));
                jmp_if_true (cmd->u.cw.cond, l2);
                break;
            case CmdCall:
                printf ("                                     # Chamada\n");
                generate_call (cmd->u.cc.call);
                break;
            case CmdBlock:
                printf ("                                     # Bloco\n");
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
    Instruction *inst;

    if (!decl)
        return;

    // TODO remover printf
    printf ("\n    .globl %s\n", decl->u.df.id);
    printf ("%s:\n", decl->u.df.id);

    inst = new_inst (PUSH, operand (EBP, 0, 0), NULL);
    generate_assembly (inst, "");

    inst = new_inst (MOV, operand (ESP, 0, 0), operand (EBP, 0, 0));
    generate_assembly (inst, "");

    inst = new_inst (SUB, int_operand(decl->u.df.max_offset),
                          operand (ESP, 0, 0));
    generate_assembly (inst, "");

    cmd = decl->u.df.block->cmd;
    generate_command (cmd);

    inst = new_inst (MOV, operand (EBP, 0, 0), operand (ESP, 0, 0));
    generate_assembly (inst, "");

    inst = new_inst (POP, operand (EBP, 0, 0), NULL);
    generate_assembly (inst, "");

    inst = new_inst (RET, NULL, NULL);
    generate_assembly (inst, "");
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
    printf ("    char_pattern: .ascii \"%%c\\12\\0\"\n");

    decl = program->decl;

    while (decl) {
        if (decl->type == DeclVar)
            generate_globals_Decl (decl);
        decl = decl->next;
    }

    decl = program->decl;

    printf ("\n.text\n");

    printf (".globl print_char\n");
    printf ("print_char:\n");
    printf ("    pushl	%%ebp\n");
    printf ("    movl	%%esp, %%ebp\n");
    printf ("    movl 8(%%ebp), %%eax\n");
    printf ("    push %%eax\n");
    printf ("    lea char_pattern, %%eax\n");
    printf ("    push %%eax\n");
    printf ("    call printf\n");
    printf ("    add $8, %%esp\n");
    printf ("    leave\n");
    printf ("    ret\n");

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
