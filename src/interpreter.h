#ifndef VINTERPRETER
#define VINTERPRETER

#include <setjmp.h>
#include "tokens.h"

typedef struct Value {
        union {
                int num;
                char *str;
        };
        Valtype type;
} Value;

extern jmp_buf panik_jmp;

/* Get the result of eval a single expression */
Value eval_expr(Expr *e);

/* Eval all expressions from parsing and print result to stdout */
void eval();


#endif
