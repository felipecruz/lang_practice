#ifndef _ASTNODES_H_
#define _ASTNODES_H_
typedef enum {
    DeclVar,
    DeclFunc
} DeclType;

typedef enum {
    TypeInt,
    TypeFloat,
    TypeChar,
    TypeVoid
} TypeType;

typedef enum {
    CmdIf,
    CmdWhile,
    CmdAss,
    CmdRet,
    CmdCall,
    CmdBlock
} CmdType;

typedef enum {
    ExpConstInt,
    ExpConstLong,
    ExpConstFloat,
    ExpConstString,
    ExpVar,
    ExpCall,
    ExpNew,
    UnaExpArith,
    BinExpArith,
} ExpType;

typedef enum {
    VarSingle,
    VarArray
} VarType;

typedef enum {
    UnaArith_Minus,
    UnaArith_Log_Neg
} UnaArithOps;

typedef enum {
    Arith_Plus,
    Arith_Sub,
    Arith_Mul,
    Arith_Div,
    Arith_Dbl_EQ,
    Arith_Lte,
    Arith_Gte,
    Arith_Lt,
    Arith_Ge,
    Arith_Log_And,
    Arith_Log_Or
} BinArithOps;

typedef struct Program {
    struct Decl *decl;
} Program;

typedef struct Type {
    TypeType type;
    int array;
} Type;

typedef struct Params {
    struct Type *type;
    char *id;
    struct Params *next;
} Params;

typedef struct Decl {
    DeclType type;
    struct Decl *next;
    int _offset;
    union {
        struct {
            struct Type *type;
            char *id;
            struct Decl *next;
        } dv;
        struct {
            char *id;
            struct Type *type;
            struct Params *params;
            struct Block *block;
        } df;
    } u;
} Decl;

typedef struct Block {
    struct Decl *decl;
    struct Cmd *cmd;
} Block;

typedef struct Exp {
    ExpType type;
    struct Exp *next;
    Type *exp_type;
    union {
        struct {
            /* const int */;
            int val;
        } eci;
        struct {
            /* const long hexa */
            long val;
        } ech;
        struct {
            /* const float */
            float val;
        } ecf;
        struct {
            /* const string */
            char *val;
        } ecs;
        struct {
            /* variable */
            struct Var *var;
        } ev;
        struct {
            /* call */
            struct Call *call;
        } ec;
        struct {
            /* new */
            struct Type *type;
            struct Exp *exp;
        } en;
        struct {
            /* Unari arith */
            UnaArithOps op;
            struct Exp *exp;
        } eu;
        struct {
            /* Binary arith */
            BinArithOps op;
            struct Exp *exp1, *exp2;
        } eb;
        struct {
            struct Exp *exp;
            struct Type *type;
        } ecast;
    }u;
} Exp;

typedef struct Cmd {
    CmdType type;
    struct Cmd *next;
    union {
        struct {
            /* if then else */
            struct Exp *cond;
            struct Cmd *then, *_else;
        } cif;
        struct {
            /* while */
            struct Exp *cond;
            struct Cmd *body;
        } cw;
        struct {
            /* assignment */
            struct Var *var;
            struct Exp *exp;
        } ca;
        struct {
            /* return */
            struct Exp *exp;
        } cr;
        struct {
            /* call */
            struct Call *call;
        } cc;
        struct {
            /* block */
            struct Block *block;
        } cb;
    } u;
} Cmd;

typedef struct Var {
    VarType type;
    struct Decl *decl;
    union {
        struct {
            char *id;
        } vs;
        struct {
            struct Exp *prefix_exp, *exp;
        } va;
    } u;
} Var;

typedef struct Call {
    char *id;
    Exp *exp_list;
    Decl *decl;
} Call;

typedef struct ScopeElement {
    struct ScopeElement *next;
    struct Decl *decl;
    int level;
} ScopeElement;

typedef struct Stack {
    struct ScopeElement *head;
} Stack;

void add_declaration(Stack*, Decl*, int);
Decl* has_name_same_level (Stack *stack, char *id, int level);
Decl* has_name (Stack *stack, char *id);
void _traverse_declarations(Stack *stack);
void remove_top_elements (Stack *stack, int level);
char *get_var_id (Var *var);

Program *new_Program ();
Program *add_Decl (Program *program, Decl *decl);

Type *new_Type (TypeType typetype, int array);

Decl* new_Decl_Var (Type *type, char* id, Decl *next);
Decl* new_Decl_Func (Type *type, char *id, Params *params, Block *block);

Params *new_Param (Type *type, char *id, Params *param);

Var *new_Var (char *id, Exp *exp, Decl *decl);
Var *new_Var_Array (Exp *prefix_exp, Exp *exp, Decl *decl);

Block *new_Block (Decl* decl, Cmd *cmd);

Cmd *new_If_Cmd (Exp *exp, Cmd *if_command, Cmd *else_cmd);
Cmd *new_While_Cmd (Exp *exp, Cmd *command);
Cmd *new_Return_Cmd (Exp *exp);
Cmd *new_Assign_Cmd (Var *var, Exp *exp);
Cmd *new_Call_Cmd (Call *call);
Cmd *new_Block_Cmd (Block *block);

Call *new_Call (char *id, Exp *exp_list);

Exp *new_Exp_Int (int val, Exp *next);
Exp *new_Exp_Hexa (long val, Exp *next);
Exp *new_Exp_Float (float val, Exp *next);
Exp *new_Exp_String (char *val, Exp *next);
Exp *new_Exp_Var (Var *var, Exp *next);
Exp *new_Exp_Call (Call *call, Exp *next);
Exp *new_Exp_New (Type *type, Exp *exp, Exp *next);
Exp *new_Exp_Unary (UnaArithOps op, Exp *exp, Exp *next);
Exp *new_Exp_Binary (BinArithOps op, Exp *expl, Exp *expr, Exp *next);
#endif
