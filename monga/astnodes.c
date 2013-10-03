#include <stdlib.h>
#include "astnodes.h"

Decl* new_Decl_Var (Type *type, NameList *name_list){
    Decl *decl = (Decl*) malloc (sizeof (Decl));

    return decl;
}
