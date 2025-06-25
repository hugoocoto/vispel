#ifndef VINTERPRETER
#define VINTERPRETER

#include "tokens.h"
#include <setjmp.h>

struct ValueNode;

typedef enum Valtype {
        TYPE_NUM,
        TYPE_STR,
        TYPE_CALLABLE,
} Valtype;

static const char *VALTYPE_REPR[] = {
        [TYPE_NUM] = "NUMBER",
        [TYPE_STR] = "STRING",
        [TYPE_CALLABLE] = "CALLABLE",
};

typedef struct Value {
        union {
                int num;
                char *str;
                struct {
                        int arity;
                        struct ValueNode *params;
                        char *name;
                        Stmt *body;
                } call;
        };
        Valtype type;
} Value;

typedef struct ValueNode {
        Value v;
        struct ValueNode *next;
} ValueNode;

/* Get the result of eval a single expression */
Value eval_expr(Expr *e);

/* Eval all expressions from parsing and print result to stdout */
void eval();

#endif
