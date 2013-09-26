typedef enum {
    CmdIf,
    CmdWhile,
    CmdAss,
    CmdRet,
    CmdCall,
    CmdBlock
} CmdType;

typedef enum {
    ExpConsInt,
    ExpConsFloat,
    ExpConsString,
    ExpVar,
    ExpCall,
    ExpNew,
    UnaExpArith,
    BinExpArith,
    LogNegExp
} ExpType;

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

typedef struct Exp {
    ExpType type;
    union {
        struct {
            /* const int */;
            int val;
        } eci;
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
    }u;
} Exp;

typedef struct Cmd {
    CmdType type;
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
            /* function call */
            char *name;
            struct Exp *exp;
        } cc;
        struct {
            /* block */
            struct Block *block;
        } cb;
    } u;
} Cmd;
