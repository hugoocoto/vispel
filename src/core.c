/* VISPEL interpreter - Core lib
 *
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/vispel
 *
 * */

#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "interpreter.h"
#include "tokens.h"

extern char *strdup(const char *);

Value
core_print(Expr *args)
{
        print_val(eval_expr(args));
        return NO_VALUE;
}

void
load_print()
{
        Value v;
        v.type = TYPE_CORE_CALL;
        v.call.arity = 1;
        v.call.ifunc = core_print;
        v.call.name = strdup("print");
        v.call.params = NULL;
        env_add("print", v);
}

void
load_core_lib()
{
        load_print();
}
