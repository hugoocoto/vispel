/* VISPEL interpreter - Core lib - IO stuff
 *
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/vispel
 *
 * */

#include <stdio.h>
#include <string.h>

#include "core.h"

Value
core_print(Expr *args)
{
        print_val(eval_expr(args));
        return NO_VALUE;
}

Value
core_input(Expr *_)
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

__attribute__((constructor)) void
load_core_io()
{
        preload("print", core_print, 1);
        preload("input", core_input, 0);
}
