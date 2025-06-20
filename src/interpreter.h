#ifndef VINTERPRETER
#define VINTERPRETER

#include "tokens.h"

typedef struct Value {
        union {
                int num;
                char *str;
        };
        Valtype type;
} Value;

/* Get the result of eval a single expression */
Value eval_expr(Expr *e);

/* Eval all expressions from parsing and print result to stdout */
void eval();


#endif
