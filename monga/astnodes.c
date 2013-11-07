#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "astnodes.h"
#include "dump.h"

char* decl_name_str (Decl *decl)
{
    if (decl->type == DeclVar)
        return decl->u.dv.id;
    else
        return decl->u.df.id;
}

void add_declaration(Stack *stack, Decl *decl, int level)
{
    Decl *_decl = stack->head->decl;
    ScopeElement *current = stack->head;

    if (!_decl) {
        stack->head->decl = decl;
        stack->head->level = level;
        return;
    }

    while (current->next) {
        current = current->next;
    }

    current->next = malloc (sizeof (ScopeElement));
    current->next->decl = decl;
    current->next->level = level;
}

void remove_top_elements (Stack *stack, int level)
{
    Decl *decl;
    ScopeElement *prev = stack->head;
    ScopeElement *current = stack->head;
    while (current && current->decl) {
        if (current->next) {
            if (current->next->level == level) {
                current->next = NULL;
                return;
            }
        }
        current = current->next;
    }
}

void _traverse_declarations(Stack *stack)
{
    printf ("\nTraversing\n");
    Decl *decl;
    ScopeElement *current = stack->head;
    while (current && current->decl) {
        decl = current->decl;
        printf ("Decl: %s level: %d\n", decl_name_str (decl), current->level);
        current = current->next;
    }
}

Decl* has_name_same_level (Stack *stack, char *id, int level)
{
    Decl *decl;
    ScopeElement *current = stack->head;
    while (current && current->decl) {
        decl = current->decl;
        if (strcmp (decl_name_str(decl), id) == 0 &&
            current->level == level)
            return decl;
        current = current->next;
    }
    return NULL;
}

Decl* has_name (Stack *stack, char *id)
{
    Decl *decl;
    ScopeElement *current = stack->head;
    while (current && current->decl) {
        decl = current->decl;
        if (strcmp (decl_name_str(decl), id) == 0)
            return decl;
        current = current->next;
    }
    return NULL;
}

char *get_var_id (Var *var)
{
    if (var == NULL)
        return NULL;

    if (var->type == VarSingle)
        return var->u.vs.id;
    else
        return get_var_id (var->u.va.var);
}

Program* new_Program ()
{
    Program *program = (Program*) malloc (sizeof (Program));
    program->decl = NULL;
    return program;
}

Program *add_Decl (Program *program, Decl *decl)
{
    decl->next = program->decl;
    program->decl = decl;
    return program;
}

Type *new_Type (TypeType typetype, int array)
{
    Type *type = malloc (sizeof (Type));

    type->type = typetype;
    type->array = array;
    return type;
}

Params *new_Param (Type *type, char *id, Params *params)
{
    Params *param = (Params*) malloc (sizeof (Params));
    param->type = type;
    param->id = strdup (id);
    param->next = params;
    return param;
}

Decl* new_Decl_Var (Type *type, char *id, Decl* next)
{
    Decl *decl = (Decl*) malloc (sizeof (Decl));
    decl->type = DeclVar;
    decl->next = NULL;

    assert (type->type != TypeVoid);

    decl->u.dv.type = type;
    decl->u.dv.id = strdup (id);
    decl->next = next;

    return decl;
}

Decl* new_Decl_Func (Type *type, char *id, Params *params, Block *block)
{
    Decl *decl = (Decl*) malloc (sizeof (Decl));
    decl->type = DeclFunc;
    decl->next = NULL;

    decl->u.df.type = type;
    decl->u.df.params = params;
    decl->u.df.block = block;
    decl->u.df.id = strdup (id);

    return decl;
}

Block *new_Block (Decl* decl, Cmd *cmd)
{
    Block *block = (Block*) malloc (sizeof (Block));
    block->decl = decl;
    block->cmd = cmd;
    return block;
}

Cmd *new_If_Cmd (Exp *exp, Cmd *if_command, Cmd *else_cmd)
{
    Cmd *cmd = (Cmd*) malloc (sizeof (Cmd));
    cmd->type = CmdIf;
    cmd->u.cif.cond = exp;
    cmd->u.cif.then = if_command;
    cmd->u.cif._else = else_cmd;
    cmd->next = NULL;
    return cmd;
}

Cmd *new_While_Cmd (Exp *exp, Cmd *command)
{
    Cmd *cmd = (Cmd*) malloc (sizeof (Cmd));
    cmd->type = CmdWhile;
    cmd->u.cw.cond = exp;
    cmd->u.cw.body = command;
    cmd->next = NULL;
    return cmd;

}

Cmd *new_Return_Cmd (Exp *exp)
{
    Cmd *cmd = (Cmd*) malloc (sizeof (Cmd));
    cmd->type = CmdRet;
    cmd->u.cr.exp = exp;
    cmd->next = NULL;
    return cmd;

}

Cmd *new_Assign_Cmd (Var *var, Exp *exp)
{
    Cmd *cmd = (Cmd*) malloc (sizeof (Cmd));
    cmd->type = CmdAss;
    cmd->u.ca.var = var;
    cmd->u.ca.exp = exp;
    return cmd;

}

Cmd *new_Call_Cmd (Call *call)
{
    Cmd *cmd = (Cmd*) malloc (sizeof (Cmd));
    cmd->type = CmdCall;
    cmd->u.cc.call = call;
    cmd->next = NULL;
    return cmd;
}

Cmd *new_Block_Cmd (Block *block)
{
    Cmd *cmd = (Cmd*) malloc (sizeof (Cmd));
    cmd->type = CmdBlock;
    cmd->u.cb.block = block;
    cmd->next = NULL;
    return cmd;
}

Exp *new_Exp_Int (int val, Exp *next)
{
    Exp *exp = (Exp*) malloc (sizeof (Exp));
    exp->type = ExpConstInt;
    exp->u.eci.val = val;
    exp->next = next;
    return exp;
}

Exp *new_Exp_Hexa (long val, Exp *next)
{
    Exp *exp = (Exp*) malloc (sizeof (Exp));
    exp->type = ExpConstLong;
    exp->u.ech.val = val;
    exp->next = next;
    return exp;
}

Exp *new_Exp_Float (float val, Exp *next)
{
    Exp *exp = (Exp*) malloc (sizeof (Exp));
    exp->type = ExpConstFloat;
    exp->u.ecf.val = val;
    exp->next = next;
    return exp;
}

Exp *new_Exp_String (char *val, Exp *next)
{
    Exp *exp = (Exp*) malloc (sizeof (Exp));
    exp->type = ExpConstString;
    exp->u.ecs.val = strdup (val);
    exp->next = next;
    return exp;
}

Exp *new_Exp_Var (Var *var, Exp *next)
{
    Exp *exp = (Exp*) malloc (sizeof (Exp));
    exp->type = ExpVar;
    exp->u.ev.var = var;
    return exp;
}

Exp *new_Exp_Call (Call *call, Exp *next)
{
    Exp *exp = (Exp*) malloc (sizeof (Exp));
    exp->type = ExpCall;
    exp->u.ec.call = call;
    exp->next = next;
    return exp;
}

Exp *new_Exp_New (Type *type, Exp *_exp, Exp *next)
{
    Exp *exp = (Exp*) malloc (sizeof (Exp));
    exp->type = ExpNew;
    exp->u.en.type = type;
    exp->u.en.exp = _exp;
    exp->next = next;
    return exp;
}
Exp *new_Exp_Unary (UnaArithOps op, Exp *_exp, Exp *next)
{
    Exp *exp = (Exp*) malloc (sizeof (Exp));
    exp->type = UnaExpArith;
    exp->u.eu.op = op;
    exp->u.eu.exp = _exp;
    return exp;

}
Exp *new_Exp_Binary (BinArithOps op, Exp *expl, Exp *expr, Exp *next)
{
    Exp *exp = (Exp*) malloc (sizeof (Exp));
    exp->type = BinExpArith;
    exp->u.eb.op = op;
    exp->u.eb.exp1 = expl;
    exp->u.eb.exp2 = expr;
    return exp;
}

Call *new_Call (char *id, Exp *exp_list)
{
    Call *call = (Call*) malloc (sizeof (Call));
    call->id = strdup (id);
    call->exp_list = exp_list;
    return call;
}

Var *new_Var (char *id, Exp *exp, Decl *decl)
{
    Var *var = (Var*) malloc (sizeof (Var));
    var->type = VarSingle;
    var->u.vs.id = strdup (id);
    var->decl = decl;
    return var;
}

Var *new_Var_Array (Var *_var, Exp *exp, Decl *decl)
{
    Var *var = (Var*) malloc (sizeof (Var));
    var->type = VarArray;
    var->u.va.var = _var;
    var->u.va.exp = exp;
    var->decl = decl;
    return var;
}
