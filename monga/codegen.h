#include <stdio.h>
#include <stdlib.h>
#include "astnodes.h"

#ifndef _CODEGEN_H_
#define _CODEGEN_H_


#define PUSH    "pushl"
#define POP     "popl"

#define ADD     "addl"
#define SUB     "subl"
#define IMUL    "imull"
#define IDIV    "idivl"
#define CDQ     "cdq"
#define NEG     "neg"

#define AND     "andl"
#define OR      "orl"
#define NOT     "not"

#define MOV     "movl"
#define MOVB    "movb"
#define LEA     "leal"

#define CALL    "call"
#define RET     "ret"

#define CMP     "cmpl"
#define JMP     "jmp"
#define JNE     "jne"
#define JE      "je"
#define JG      "jg"
#define JGE     "jge"
#define JL      "jl"
#define JLE     "jle"

#define EAX     "eax"
#define AL      "al"
#define ECX     "ecx"
#define EBP     "ebp"
#define ESP     "esp"

typedef struct Instruction {
    char *op;
    char *src;
    char *dst;
} Instruction;

void generte_assembly (Instruction inst, char *cmt);
void generate_Program (Program *program);
void generate_globals_Decl (Decl *decl);
void generate_functions_Decl (Decl *decl);
void jmp_if_false (Exp *exp, int label);
void jmp_if_true (Exp *exp, int label);
void jmp_if_true_binexp (Exp *exp, int label);
void jmp_if_false_binexp (Exp *exp, int label);
void generate_expression (Exp *exp);
void generate_command (Cmd *cmd);
void generate_call (Call *call);
void generate_var (Var *var);
#endif
