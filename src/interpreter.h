#ifndef VINTERPRETER
#define VINTERPRETER

#include "tokens.h"
#include <setjmp.h>

#define NO_VALUE ((Value) { .type = TYPE_STR, .str = "no-value" })

struct ValueNode;

typedef enum Valtype {
        TYPE_NUM,
        TYPE_STR,
        TYPE_CALLABLE,
        TYPE_CORE_CALL,
} Valtype;

static const char *VALTYPE_REPR[] = {
        [TYPE_NUM] = "NUMBER",
        [TYPE_STR] = "STRING",
        [TYPE_CALLABLE] = "CALLABLE",
        [TYPE_CORE_CALL] = "CORE CALL",
};

struct Env;

typedef struct Value {
        union {
                int num;
                char *str;
                struct {
                        int arity;
                        vtok *params;
                        char *name;
                        union {
                                Stmt *body;
                                struct Value (*ifunc)(Expr *);
                        };
                        struct Env *closure;
                } call;
        };
        Valtype type;
} Value;

typedef struct ValueNode {
        Value v;
        struct ValueNode *next;
} ValueNode;

typedef struct node {
        char *key;
        Value value;
} node;

typedef struct Env {
        node *map;
        char *name;
        struct Env *upper;
} Env;

/* Get the result of eval a single expression */
Value eval_expr(Expr *e);

/* Eval all expressions from parsing and print result to stdout */
void eval();
void print_val(Value v);
void load_core_lib();

int resolve();


#endif
