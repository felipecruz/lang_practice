#include "typesystem.h"
#include "dump.h"

static Type *VOID_TYPE;
static Type *INT_TYPE;
static Type *FLOAT_TYPE;
static Type *CHAR_TYPE;
static Type *INT_TYPE_AR;
static Type *FLOAT_TYPE_AR;
static Type *CHAR_TYPE_AR;

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
    if (var->decl->type == DeclVar)
        return var->decl->u.dv.type;
    return NULL;
}

Type* coerce (Type *from, Type *to)
{
    if (match (from, to))
        return to;

    if ((match (from, INT_TYPE) && match (to, FLOAT_TYPE)) ||
        (match (from, FLOAT_TYPE) && match (to, INT_TYPE)))
        return FLOAT_TYPE;

    return NULL;
}

Type *assignment_coerce (Type *from, Type *to)
{
    Type *new_type;

    if (match (from, to))
        return to;

    if (match (from, INT_TYPE) && match (to, FLOAT_TYPE))
        return FLOAT_TYPE;

    if ((match (from, INT_TYPE) && match (to, CHAR_TYPE)))
        return CHAR_TYPE;

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

void link_calls (Exp *exp, Decl *globals)
{
    Decl *decl;
    Type *type, *subscript_type, *type2, *new_type;
    Exp *subscript_exp;

    switch (exp->type) {
        case ExpConstInt:
        case ExpConstLong:
        case ExpConstFloat:
        case ExpConstString:
        case ExpVar:
            return;
        case ExpCall:
            decl = resolve_decl (exp->u.ec.call->id, globals);
            if (!decl)
                printdebug ("Undeclared function name");
            exp->u.ec.call->decl = decl;
            exp = exp->u.ec.call->exp_list;
            while (exp) {
                link_calls (exp, globals);
                exp = exp->next;
            }
            break;
        case ExpNew:
            link_calls (exp->u.en.exp, globals);
            break;
        case UnaExpArith:
            link_calls (exp->u.eu.exp, globals);
            break;
        case BinExpArith:
            link_calls (exp->u.eb.exp1, globals);
            link_calls (exp->u.eb.exp2, globals);
        break;
    }
}

Type *get_exp_type (Exp *exp)
{
    Type *type, *subscript_type, *type2, *new_type;
    Exp *subscript_exp;

    switch (exp->type) {
        case ExpConstInt:
            return INT_TYPE;
        case ExpConstLong:
            return INT_TYPE;
        case ExpConstFloat:
            return FLOAT_TYPE;
        case ExpConstString:
            return CHAR_TYPE_AR;
        case ExpVar:
            printdebug ("Get Expression Var Type...\n");
            type = resolve_type (exp->u.ev.var);

            if (exp->u.ev.var->u.va.exp == NULL)
                return type;

            type->array = 1;
            return type;
            break;
        case ExpCall:
            printdebug ("Get call return type..\n");
            type = get_call_type (exp->u.ec.call);
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
            if (type)
                return type;
            break;
        case UnaExpArith:
            type = get_exp_type (exp->u.eu.exp);
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
            if (!new_type) {
                printdebug ("Invalid binary expression - uncompatible expressions\n");
                return NULL;
            }
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

int link_and_validate_calls (Decl *decl, Decl *globals)
{
    Decl *call_decl = NULL;
    Cmd *cmd = decl->u.df.block->cmd;
    Exp *return_exp = NULL;

    while (cmd) {
        switch (cmd->type) {
            case CmdIf:
                link_calls (cmd->u.cif.cond, globals);
                break;
            case CmdWhile:
                link_calls (cmd->u.cw.cond, globals);
                break;
            case CmdAss:
                link_calls (cmd->u.ca.exp, globals);
            break;
            case CmdRet:
                return_exp = cmd->u.cr.exp;
                if (return_exp)
                    link_calls (return_exp, globals);
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
                link_and_validate_calls (cmd->u.cb.block->decl, globals);
                break;
        }
        cmd = cmd->next;
    }
    return 0;
}

int check_declaration_block (Decl *decl)
{
    int rc;

    Type *return_type = decl->u.df.type;
    Cmd *cmd = decl->u.df.block->cmd;
    Exp *return_exp = NULL, *assign_exp = NULL, *assignee_exp = NULL,
        *exp_list = NULL;
    Params *function_params;
    Type *type, *type2;

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
                printdebug ("Checking assingment types... \n");
                assign_exp = cmd->u.ca.exp;
                type = get_exp_type (assign_exp);
                type2 = resolve_type (cmd->u.ca.var);

                if (!type || !type2) {
                    printdebug ("Invalid Expression\n");
                    return -1;
                }

                type = assignment_coerce (type, type2);
                if (!type) {
                    printdebug ("Assignment Type didn't match\n");
                    return -1;
                }

                if (assign_exp->type == ExpCall) {
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

                rc = match(type, decl->u.df.type);
                if (rc != 1) {
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
            rc = link_and_validate_calls (decl, globals);
            if (rc == -1)
                return rc;
        }
        decl = decl->next;
    }
}

int check_program (Program *program)
{
    int rc;
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

    while (decl) {
        if (decl->type == DeclFunc) {
            printdebug ("Checking Func %s\n", decl->u.df.id);
            rc = check_declaration_block (decl);
            if (rc == -1)
                return rc;
        }
        decl = decl->next;
    }

}
