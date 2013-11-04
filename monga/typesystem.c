#include "typesystem.h"
#include "dump.h"

#define IS_INT(e) (e->type == ExpConstInt ||  \
                   (e->type == ExpVar && e->u.ev.var->type == VarSingle && \
                    e->u.ev.var->vs))


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

Type *get_exp_type (Exp *exp, Decl *globals, Decl *locals, Params *params)
{ 
    Type *type;
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
            return type;
            break;
        case ExpCall:
            printf ("Get call return type..\n");
            type = get_decl_type (globals, exp->u.ec.call->id);
            if (type)
                return type;
            break;
        /*
        ExpNew,
        UnaExpArith,
        BinExpArith,*/
    }
    return NULL;
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
    Exp *return_exp = NULL, *assign_exp = NULL, *assignee_exp = NULL;
    Type *type, *type2;

    while (cmd) {
        switch (cmd->type) {
            case CmdIf:

            break;
            case CmdWhile:

            break;
            case CmdAss:
                printf ("Checking assingment types... \n");
                assign_exp = cmd->u.ca.exp;
                type = get_exp_type (assign_exp, globals, locals, params);
                type2 = resolve_type (get_var_id (cmd->u.ca.var), globals,
                                      locals, params);
                rc = match(type, type2);
                if (rc != 1) {
                    printf ("Assignment Type didn't match\n");
                    return -1;
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