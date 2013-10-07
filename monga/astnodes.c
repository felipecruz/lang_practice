#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "astnodes.h"

Program *new_Program (Decl *decl)
{
    Program *program = (Program*) malloc (sizeof (Program));
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
    NameList *name_list = malloc (sizeof (NameList));
    name_list->next = next;
    name_list->id = strdup (id);
    return name_list;
}

Decl* new_Decl_Var (Type *type, NameList *name_list)
{
    Decl *decl = (Decl*) malloc (sizeof (Decl));
    decl->type = DeclVar;

    assert (type->type != TypeVoid);

    decl->u.dv.type = type;
    decl->u.dv.names = name_list;

    return decl;
}

Decl* new_Decl_Func (Type *type, char *id, Params *params, Block *block)
{
    Decl *decl = (Decl*) malloc (sizeof (Decl));
    decl->type = DeclFunc;

    decl->u.df.type = type;
    decl->u.df.params = params;
    decl->u.df.block = block;

    return decl;
}
