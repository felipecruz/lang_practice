#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "astnodes.h"
#include "dump.h"

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

    if (typetype == TypeInt)
        type->type = TypeInt;

    if (typetype == TypeChar)
        type->type = TypeChar;

    if (typetype == TypeFloat)
        type->type = TypeFloat;

    if (typetype == TypeVoid)
        type->type = TypeVoid;

    type->array = array;
    return type;
}

NameList *new_Name_List (char *id, NameList *next)
{
    NameList *name_list = (NameList*) malloc (sizeof (NameList));
    name_list->next = next;
    name_list->id = strdup (id);
    return name_list;
}

Params *new_Param (Type *type, char *id, Params *params)
{
    Params *param = (Params*) malloc (sizeof (Params));
    param->type = type;
    param->id = strdup (id);
    param->next = params;
    return param;
}

Decl* new_Decl_Var (Type *type, NameList *name_list)
{
    Decl *decl = (Decl*) malloc (sizeof (Decl));
    decl->type = DeclVar;
    decl->next = NULL;

    assert (type->type != TypeVoid);

    decl->u.dv.type = type;
    decl->u.dv.names = name_list;

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
    exp->u.eu.exp = exp;
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

Var *new_Var (char *id, Exp *exp)
{
    Var *var = (Var*) malloc (sizeof (Var));
    var->type = VarSingle;
    var->u.vs.id = strdup (id);
    return var;
}

Var *new_Var_Array (Var *_var, Exp *exp)
{
    Var *var = (Var*) malloc (sizeof (Var));
    var->type = VarArray;
    var->u.va.var = _var;
    var->u.va.exp = exp;
    return var;
}
