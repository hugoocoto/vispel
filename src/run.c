/* VISPEL interpreter - Evalue AST branch
 *
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/vispel
 *
 * */

#include "tokens.h"

typedef struct Value {
} Value;

Value
eval_expr(Expr *e)
{
        switch (e->type) {
        case ASSIGNEXPR:
        case BINEXPR:
        case UNEXPR:
        case CALLEXPR:
        case LITEXPR:
        case VAREXPR:
                break;
        }
}
