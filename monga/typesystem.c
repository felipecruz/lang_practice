#include "typesystem.h"
#include "dump.h"

static Type *VOID_TYPE;
static Type *INT_TYPE;
static Type *FLOAT_TYPE;
static Type *CHAR_TYPE;
static Type *INT_TYPE_AR;
static Type *FLOAT_TYPE_AR;
static Type *CHAR_TYPE_AR;

static int current_max = 0;

Params* get_func_params (Call *call)
{
    return call->decl->u.df.params;
}

Decl *resolve_decl (char *id, Decl *decl)
{
    while (decl) {
        if (decl->type == DeclFunc)
            if (strcmp (decl->u.df.id, id) == 0)
                return decl;
        if (decl->type == DeclVar)
            if (strcmp (decl->u.dv.id, id) == 0)
                return decl;
        decl = decl->next;
    }
    return NULL;
}

Type *get_call_type (Call *call)
{
    if (call->decl->type == DeclVar)
        return NULL;
    return call->decl->u.df.type;
}

Type *resolve_type (Var *var)
{
    Type *type;
    if (var->type == VarArray && var->u.va.exp != NULL) {
        if (var->decl->type == DeclVar)
            type = var->decl->u.dv.type;
        if (var->decl->type == DeclFunc)
            type = var->decl->u.df.type;
        type = new_Type (type->type, 0);
        return type;
    }

    if (var->decl->type == DeclVar)
        return var->decl->u.dv.type;
    if (var->decl->type == DeclFunc)
        type = var->decl->u.df.type;
    return NULL;
}

Type* coerce (Type *from, Type *to)
{
    if (match (from, to))
        return to;

    if ((match (from, INT_TYPE) && match (to, FLOAT_TYPE)) ||
        (match (from, FLOAT_TYPE) && match (to, INT_TYPE)))
        return FLOAT_TYPE;

    if ((match (from, CHAR_TYPE) && match (to, INT_TYPE)) ||
        (match (from, INT_TYPE) && match (to, CHAR_TYPE)))
        return INT_TYPE;

    return NULL;
}

Type *assignment_coerce (Type *from, Type *to)
{
    Type *new_type;

    if (match (from, to))
        return to;

    if (match (from, INT_TYPE) && match (to, FLOAT_TYPE))
        return FLOAT_TYPE;

    if ((match (from, CHAR_TYPE) && match (to, INT_TYPE)) ||
        (match (from, INT_TYPE) && match (to, CHAR_TYPE)))
        return CHAR_TYPE;

    if (match_array_assignment (from, to)) {
        return from;
    }

    return NULL;
}

Decl *link_call (Call *call, Decl *globals)
{
    Decl *decl = resolve_decl (call->id, globals);
    if (decl == NULL || decl->type == DeclVar)
        return NULL;
    call->decl = decl;
    return call->decl;
}

int link_calls (Exp *exp, Decl *globals)
{
    int rc;
    Decl *decl;
    Type *type, *subscript_type, *type2, *new_type;
    Exp *subscript_exp;

    switch (exp->type) {
        case ExpConstInt:
        case ExpConstLong:
        case ExpConstFloat:
        case ExpConstString:
        case ExpVar:
            return 0;
        case ExpCall:
            decl = resolve_decl (exp->u.ec.call->id, globals);
            if (!decl) {
                printdebug ("Undeclared function name");
                return -1;
            }
            exp->u.ec.call->decl = decl;
            exp = exp->u.ec.call->exp_list;
            while (exp) {
                rc = link_calls (exp, globals);
                if (rc == -1)
                    return rc;
                exp = exp->next;
            }
            break;
        case ExpNew:
            rc = link_calls (exp->u.en.exp, globals);
            if (rc == -1)
                return rc;
            break;
        case UnaExpArith:
            rc = link_calls (exp->u.eu.exp, globals);
            if (rc == -1)
                return rc;
            break;
        case BinExpArith:
            rc = link_calls (exp->u.eb.exp1, globals);
            if (rc == -1)
                return rc;
            rc = link_calls (exp->u.eb.exp2, globals);
            if (rc == -1)
                return rc;
        break;
    }
    return 0;
}

Type *get_exp_type (Exp *exp)
{
    Type *type, *subscript_type, *type2, *new_type;
    Exp *subscript_exp;

    switch (exp->type) {
        case ExpConstInt:
            exp->exp_type = INT_TYPE;
            return INT_TYPE;
        case ExpConstLong:
            exp->exp_type = INT_TYPE;
            return INT_TYPE;
        case ExpConstFloat:
            exp->exp_type = FLOAT_TYPE;
            return FLOAT_TYPE;
        case ExpConstString:
            exp->exp_type = CHAR_TYPE_AR;
            return CHAR_TYPE_AR;
        case ExpVar:
            printdebug ("Get Expression Var Type...\n");
            type = resolve_type (exp->u.ev.var);

            if (exp->u.ev.var->type == VarArray) {
                if (exp->u.ev.var->u.va.exp != NULL) {
                    type = new_Type (type->type, 0);
                }
            }

            exp->exp_type = type;
            if (type)
                return type;
            break;
        case ExpCall:
            printdebug ("Get call return type..\n");
            type = get_call_type (exp->u.ec.call);
            exp->exp_type = type;
            if (type)
                return type;
            break;
        case ExpNew:
            type = exp->u.en.type;
            subscript_exp = exp->u.en.exp;
            subscript_type = get_exp_type (subscript_exp);
            if (!match (INT_TYPE, subscript_type)) {
                printdebug ("Invalid index type\n");
                return NULL;
            }
            type->array = 1;
            exp->exp_type = type;
            if (type)
                return type;
            break;
        case UnaExpArith:
            type = get_exp_type (exp->u.eu.exp);
            exp->exp_type = type;
            if (match (FLOAT_TYPE, type) && exp->u.eu.op == UnaArith_Minus) {
                return type;
            }
            if (!match (INT_TYPE, type)) {
                printdebug ("Invalid unary expression\n");
                return NULL;
            }
            if (type)
                return type;
            break;
        case BinExpArith:
            type = get_exp_type (exp->u.eb.exp1);
            type2 = get_exp_type (exp->u.eb.exp2);
            new_type = coerce (type, type2);
            if (!match (type, new_type)) {
                exp->u.ecast.exp = exp->u.eb.exp1;
                exp->u.ecast.type = new_type;
                printf ("Change Exp to Exp with cast\n");
            }
            if (!new_type) {
                printdebug ("Invalid binary expression - uncompatible expressions\n");
                return NULL;
            }
            exp->exp_type = new_type;
            return new_type;
    }
    return NULL;
}

int match_signature (Params *params, Exp *exps)
{
    int rc;
    Params *par;
    Exp *exp;
    Type *type, *type2;

    while (params && exps) {
        type = get_exp_type (exps);

        type2 = params->type;
        rc = match (type, type2);

        if (rc != 1) {
            return rc;
        }

        params = params->next;
        exps = exps->next;
    }
    if ((!exps && params) || (exps && !params))
        return -1;

    return 1;
}

int match (Type *t1, Type *t2)
{
    /* verifica tipo base */
    int e = t1->type == t2->type;
    /* verifica flag de array */
    int a = t1->array == t2->array;
    return e && a;
}

int match_array_assignment (Type *t1, Type *t2)
{
    /* verifica tipo base */
    int e = t1->type == t2->type;
    /* verifica flag de array */
    int a = (t2->array && !t1->array);
    return e && a;
}

int link_and_validate_calls (Block *block, Decl *globals)
{
    if (!block)
        return 0;

    int rc = 0;
    Decl *call_decl = NULL;
    Cmd *cmd = block->cmd;
    Exp *return_exp = NULL;

    while (cmd) {
        switch (cmd->type) {
            case CmdIf:
                rc = link_calls (cmd->u.cif.cond, globals);
                break;
            case CmdWhile:
                rc = link_calls (cmd->u.cw.cond, globals);
                break;
            case CmdAss:
                rc = link_calls (cmd->u.ca.exp, globals);
            break;
            case CmdRet:
                return_exp = cmd->u.cr.exp;
                if (return_exp)
                    rc = link_calls (return_exp, globals);

            break;
            case CmdCall:
                call_decl = link_call (cmd->u.cc.call, globals);
                if (!call_decl) {
                    printdebug ("Unknow Function name\n");
                    return -1;
                }
                break;
            break;
            case CmdBlock:
                rc = link_and_validate_calls (cmd->u.cb.block, globals);
                break;
        }
        cmd = cmd->next;
    }
    return rc;
}

void set_decl_offset_get_max (Decl *decls, Block *block, int current)
{
    int i, local_max = current;
    Cmd *cmd = block->cmd;

    for (i = 1; decls; i++) {
        decls->_offset = (i * -4) + current;
        local_max = decls->_offset;
        decls = decls->next;
    }

    if (local_max < current_max) {
        current_max = local_max;
    }

    current = local_max;

    while (cmd) {
        if (cmd->type == CmdBlock) {
            set_decl_offset_get_max (cmd->u.cb.block->decl,
                                     cmd->u.cb.block, current);

        }
        cmd = cmd->next;
    }
}

int check_declaration_block (Decl *decl)
{
    int i, rc;

    if (decl->u.df._extern)
        return 0;

    Decl *decls = decl->u.df.block->decl;
    Type *return_type = decl->u.df.type;
    Cmd *cmd = decl->u.df.block->cmd;
    Exp *return_exp = NULL, *assign_exp = NULL, *assignee_exp = NULL,
        *exp_list = NULL;
    Params *function_params;
    Type *type, *type2, *new_type;
    Var *var;

    function_params = decl->u.df.params;
    for (i = 0; function_params; i++) {
        function_params->decl->_offset = (i * 4) + 8;
        function_params = function_params->next;
    }

    set_decl_offset_get_max (decls, decl->u.df.block, 0);
    decl->u.df.max_offset = -current_max;

    while (cmd) {
        switch (cmd->type) {
            case CmdIf:
                type = get_exp_type (cmd->u.cif.cond);
                if (!type || !match (type, INT_TYPE)) {
                    printdebug ("Invalid If Condition\n");
                    return -1;
                }
                break;
            case CmdWhile:
                type = get_exp_type (cmd->u.cw.cond);
                if (!type || !match (type, INT_TYPE)) {
                    printdebug ("Invalid While Condition\n");
                    return -1;
                }
                break;
            case CmdAss:
                printdebug ("Checking assignment types... \n");
                var = cmd->u.ca.var;
                type = get_exp_type (cmd->u.ca.exp);
                type2 = resolve_type (cmd->u.ca.var);

                if (!type || !type2) {
                    printdebug ("Invalid Expression\n");
                    return -1;
                }

                new_type = assignment_coerce (type2, type);
                if (!new_type) {
                    printdebug ("Assignment Type didn't match\n");
                    return -1;
                }

                if (cmd->u.ca.exp->type == ExpCall) {
                    exp_list = cmd->u.ca.exp->u.ec.call->exp_list;
                    function_params = get_func_params (cmd->u.ca.exp->u.ec.call);

                    rc = match_signature (function_params, exp_list);
                    if (rc != 1) {
                        printdebug ("Function Call signagure -> parameters dont match\n");
                        return -1;
                    }
                }
            break;
            case CmdRet:
                printdebug ("Checking return type.. \n");
                return_exp = cmd->u.cr.exp;
                if (return_exp)
                    type = get_exp_type (return_exp);
                else
                    type = VOID_TYPE;

                new_type = coerce (type, decl->u.df.type);
                if (!new_type) {
                    printdebug ("Return Type didn't match function declaration\n");
                    return -1;
                }
            break;
            case CmdCall:
                type = get_call_type (cmd->u.cc.call);
                if (!type) {
                    printdebug ("Unknown Function Name");
                    return -1;
                }

                exp_list = cmd->u.cc.call->exp_list;
                function_params = get_func_params (cmd->u.cc.call);

                rc = match_signature (function_params, exp_list);
                if (rc != 1) {
                    printdebug ("Function Call signature -> parameters dont match\n");
                    return -1;
                }
            break;
            case CmdBlock:

            break;
        }
        cmd = cmd->next;
    }
    return 0;
}

int link_missing_calls (Program *program)
{
    int rc;
    Decl *decl, *globals, *locals;

    decl = program->decl;
    globals = program->decl;

    while (decl) {
        if (decl->type == DeclFunc) {
            rc = link_and_validate_calls (decl->u.df.block, globals);
            if (rc == -1)
                return rc;
        }
        decl = decl->next;
    }
}

int check_program (Program *program)
{
    int rc, has_main = 0;
    Decl *decl;
    Params *params;

    VOID_TYPE = new_Type (TypeVoid, 0);
    INT_TYPE = new_Type (TypeInt, 0);
    FLOAT_TYPE = new_Type (TypeFloat, 0);
    CHAR_TYPE = new_Type (TypeChar, 0);
    INT_TYPE_AR = new_Type (TypeInt, 1);
    FLOAT_TYPE_AR = new_Type (TypeFloat, 1);
    CHAR_TYPE_AR = new_Type (TypeChar, 1);

    if (program == NULL) {
        return 0;
    }
    decl = program->decl;

    //TODO Verificar se tem main

    while (decl) {
        if (decl->type == DeclFunc && !decl->u.df._extern)
            if (strcmp (decl->u.df.id, "main") == 0)
                has_main = 1;
        decl = decl->next;
    }

    if (!has_main) {
        return -2;
    }

    decl = program->decl;

    while (decl) {
        if (decl->type == DeclFunc) {
            printdebug ("Checking Func %s\n", decl->u.df.id);
            current_max = 0;
            rc = check_declaration_block (decl);
            if (rc == -1)
                return rc;
        }
        decl = decl->next;
    }

}
