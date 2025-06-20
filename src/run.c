/* VISPEL interpreter - Evalue AST branch
 *
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/vispel
 *
 * */
#include <stdio.h>
#include <stdlib.h>

#include "interpreter.h"
#include "tokens.h"

void
print_val(Value v)
{
        switch (v.type) {
        case TYPE_NUM:
                printf("%d\n", v.num);
                break;
        case TYPE_STR:
                printf("%s\n", v.str);
                break;
        default:
                report("No yet implemented: print_val for %s\n",
                       VALTYPE_REPR[v.type]);
                exit(1);
        }
}

Value
get_lit_value(Expr *e)
{
        Value v;
        switch (e->litexpr.value->token) {
        case STRING:
        case CHAR:
                v.type = TYPE_STR;
                v.str = e->litexpr.value->str_literal;
                break;
        case NUMBER:
                v.type = TYPE_NUM;
                v.num = e->litexpr.value->num_literal;
                break;
        case TRUE:
                v.type = TYPE_NUM;
                v.num = 1;
                break;
        case FALSE:
                v.type = TYPE_NUM;
                v.num = 0;
                break;
        default:
                report("No yet implemented: get_lit_value for %s\n",
                       TOKEN_REPR[e->litexpr.value->token]);
                exit(1);
        }
        return v;
}

void
panik_invalid_binop(Value L, vtoktype OP, Value R)
{
        report("Invalid binary operation %s between %s and %s\n",
               TOKEN_REPR[OP], VALTYPE_REPR[L.type], VALTYPE_REPR[R.type]);
        exit(1);
}

int
is_true(Value v)
{
        switch (v.type) {
        case TYPE_NUM:
                return v.num;
        case TYPE_STR:
                return v.str && *v.str;
        default:
                report("No yet implemented: is_true for %s\n",
                       VALTYPE_REPR[v.type]);
                break;
        }
}

Value
get_bin_value(Expr *e)
{
        Value v;
        Value lhs = eval_expr(e->binexpr.lhs);
        Value rhs = eval_expr(e->binexpr.rhs);

        switch (e->binexpr.op->token) {
        case MINUS:
                if (rhs.type == TYPE_NUM && rhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num - lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case PLUS:
                if (rhs.type == TYPE_NUM && rhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num + lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case SLASH:
                if (rhs.type == TYPE_NUM && rhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num / lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case STAR:
                if (rhs.type == TYPE_NUM && rhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num * lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case AND:
                v.type = TYPE_NUM;
                v.num = is_true(rhs) && is_true(lhs);
                break;

        case OR:
                v.type = TYPE_NUM;
                v.num = is_true(rhs) && is_true(lhs);
                break;

        case BANG_EQUAL:
        case EQUAL_EQUAL:
        case EQUAL:
        case GREATER:
        case GREATER_EQUAL:
        case LESS:
        case LESS_EQUAL:
        case BITWISE_AND:
        case BITWISE_OR:
        case BITWISE_XOR:
        case BITWISE_NOT:
        case SHIFT_LEFT:
        case SHIFT_RIGHT:
        default:
                report("Binexpr Operation no yet implemented: %s\n",
                       TOKEN_REPR[e->litexpr.value->token]);
                exit(1);
        }
        return v;
}

Value
eval_expr(Expr *e)
{
        Value v;
        switch (e->type) {
        case LITEXPR:
                v = get_lit_value(e);
                break;
        case BINEXPR:
                v = get_bin_value(e);
                break;
        case UNEXPR:
        case CALLEXPR:
        case ASSIGNEXPR:
        case VAREXPR:
        default:
                report("Todo: eval expression %s\n", EXPR_REPR[e->type]);
                exit(1);
                break;
        }
        return v;
}

void
eval()
{
        Expr *e = head_expr;
        while (e) {
                print_val(eval_expr(e));
                e = e->next;
        }
}
