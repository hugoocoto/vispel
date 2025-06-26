/* VISPEL interpreter - Core lib
 *
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/vispel
 *
 * */

#include <stdio.h>
#include <string.h>

#include "env.h"
#include "interpreter.h"
#include "tokens.h"

extern char *strdup(const char *);

static void
load(const char *name, Value (*func)(Expr *), int arity)
{
        Value v;
        v.type = TYPE_CORE_CALL;
        v.call.arity = arity;       // number of params
        v.call.ifunc = func;        // C function
        v.call.name = strdup(name); // vispel function name
        v.call.params = NULL;
        v.call.closure = get_current_env();
        env_add(v.call.name, v);
}

Value
core_print(Expr *args)
{
        print_val(eval_expr(args));
        return NO_VALUE;
}

Value
core_input(Expr* _)
{
        char buf[1024];
        char *c;
        if (fgets(buf, sizeof buf - 1, stdin)) {
                if ((c = strchr(buf, '\n'))) {
                        *c = 0;
                }
                return (Value) { .type = TYPE_STR, .str = strdup(buf) };
        }
        return NO_VALUE;
}

void
load_core_lib()
{
        load("print", core_print, 1);
        load("input", core_input, 0);
}
