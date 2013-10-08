#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "astnodes.h"

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
        type->type = TypeInt;

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
