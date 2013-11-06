#include "typesystem.h"
#include "dump.h"

static Type *VOID_TYPE;
static Type *INT_TYPE;
static Type *FLOAT_TYPE;
static Type *CHAR_TYPE;
static Type *INT_TYPE_AR;
static Type *FLOAT_TYPE_AR;
static Type *CHAR_TYPE_AR;

char *get_var_id (Var *var)
{
    if (var == NULL)
        return NULL;

    if (var->type == VarSingle)
        return var->u.vs.id;
    else
        return get_var_id (var->u.va.var);
}

Type *get_parameter_type (Params *param, char *id)
{
    while (param) {
        if (strcmp (param->id, id) == 0)
            return param->type;

        param = param->next;
    }
    return NULL;
}

Params *get_params (char *id, Decl *decl)
{
    while (decl) {
        if (decl->type == DeclFunc)
            if (strcmp (decl->u.df.id, id) == 0)
                return decl->u.df.params;
        decl = decl->next;
    }
}


Params* get_func_params (Call *call, Decl *globals, Decl *locals)
{
    Params *params;

    params = get_params (call->id, globals);
    if (params)
        return params;

    params = get_params (call->id, locals);
    if (params)
        return params;

    return NULL;
}

Type *get_decl_type (Decl *decl, char *id)
{
    while (decl) {
        if (decl->type == DeclFunc)
            if (strcmp (decl->u.df.id, id) == 0)
                return decl->u.df.type;
        if (decl->type == DeclVar)
            if (strcmp (decl->u.dv.id, id) == 0)
                return decl->u.dv.type;

        decl = decl->next;
    }
    return NULL;
}

Type *resolve_type (char *id, Decl *globals, Decl *locals, Params *params)
{
    Type *type;
    type = get_decl_type (locals, id);
    if (type)
        return type;
    type = get_decl_type (globals, id);
    if (type)
        return type;
    type = get_parameter_type (params, id);
    if (type)
        return type;
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


Type *get_exp_type (Exp *exp, Decl *globals, Decl *locals, Params *params)
{
    Type *type, *subscript_type, *type2, *new_type;
    Exp *subscript_exp;
    printf ("Get Type\n");

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
            printf ("Get expression Type...\n");
            type = resolve_type (get_var_id (exp->u.ev.var), globals, locals,
                                 params);

            if (exp->u.ev.var->u.va.exp == NULL)
                return type;

            type->array = 0;
            return type;
            break;
        case ExpCall:
            printf ("Get call return type..\n");
            type = get_decl_type (globals, exp->u.ec.call->id);
            if (type)
                return type;
            break;
        case ExpNew:
            type = exp->u.en.type;
            subscript_exp = exp->u.en.exp;
            subscript_type = get_exp_type (subscript_exp, globals, locals, params);
            if (!match (INT_TYPE, subscript_type)) {
                printf ("Invalid index type\n");
                return NULL;
            }
            type->array = 1;
            if (type)
                return type;
            break;
        case UnaExpArith:
            type = get_exp_type (exp->u.eu.exp, globals, locals, params);
            if (!match (INT_TYPE, type)) {
                printf ("Invalid unary expression\n");
                return NULL;
            }
            if (type)
                return type;
            break;
        case BinExpArith:
            type = get_exp_type (exp->u.eb.exp1, globals, locals, params);
            type2 = get_exp_type (exp->u.eb.exp2, globals, locals, params);
            new_type = coerce (type, type2);
            if (!new_type) {
                printf ("Invalid binary expression - uncompatible expressions\n");
                return NULL;
            }
            return new_type;
    }
    return NULL;
}

int match_signature (Params *params, Exp *exps, Decl *globals, Decl *locals,
                     Params *function_params)
{
    int rc;
    Params *par;
    Exp *exp;
    Type *type, *type2;

    while (params && exps) {
        type = get_exp_type (exps, globals, locals, params);
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

int check_declaration_block (Decl *decl, Decl *globals, Decl *locals,
                             Params *params)
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
                type = get_exp_type (cmd->u.cif.cond, globals, locals, params);
                if (!type || !match (type, INT_TYPE)) {
                    printf ("Invalid If Condition\n");
                    return -1;
                }
                break;
            case CmdWhile:
                type = get_exp_type (cmd->u.cw.cond, globals, locals, params);
                if (!type || !match (type, INT_TYPE)) {
                    printf ("Invalid While Condition\n");
                    return -1;
                }
                break;
            case CmdAss:
                printf ("Checking assingment types... \n");
                assign_exp = cmd->u.ca.exp;
                type = get_exp_type (assign_exp, globals, locals, params);
                type2 = get_exp_type (cmd->u.ca.exp, globals,
                                      locals, params);

                if (!type || !type2) {
                    printf ("Invalid Expression\n");
                    return -1;
                }

                type = assignment_coerce (type, type2);
                if (!type) {
                    printf ("Assignment Type didn't match\n");
                    return -1;
                }

                if (assign_exp->type == ExpCall) {
                    exp_list = cmd->u.ca.exp->u.ec.call->exp_list;
                    function_params = get_func_params (cmd->u.ca.exp->u.ec.call,
                                                       globals, locals);

                    print_Params (function_params, 0);
                    rc = match_signature (function_params, exp_list, globals,
                                          locals, params);
                    if (rc != 1) {
                        printf ("Function Call signagure -> parameters dont match\n");
                        return -1;
                    }
                }
            break;
            case CmdRet:
                printf ("Checking return type.. \n");
                return_exp = cmd->u.cr.exp;
                if (return_exp)
                    type = get_exp_type (return_exp, globals, locals, params);
                else
                    type = VOID_TYPE;

                rc = match(type, decl->u.df.type);
                if (rc != 1) {
                    printf ("Return Type didn't match function declaration\n");
                    return -1;
                }
            break;
            case CmdCall:
                exp_list = cmd->u.cc.call->exp_list;
                function_params = get_func_params (cmd->u.cc.call,
                                                   globals, locals);

                print_Params (function_params, 0);
                rc = match_signature (function_params, exp_list, globals,
                                      locals, params);
                if (rc != 1) {
                    printf ("Function Call signagure -> parameters dont match\n");
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

int check_program (Program *program)
{
    int rc;
    Decl *decl, *globals, *locals;
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
    globals = program->decl;

    while (decl) {
        if (decl->type == DeclFunc) {
            locals = decl->u.df.block->decl;
            params = decl->u.df.params;
            printf ("Checking Func %s\n", decl->u.df.id);
            rc = check_declaration_block (decl, globals, locals, params);
            if (rc == -1)
                return rc;
        }
        decl = decl->next;
    }

}
