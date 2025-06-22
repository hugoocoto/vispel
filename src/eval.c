/* VISPEL interpreter - Evalue AST branch
 *
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/vispel
 *
 * */

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "interpreter.h"
#include "tokens.h"

jmp_buf eval_runtime_error;

/* should return to repr. Now it does nothing */
#define EXIT(X) longjmp(eval_runtime_error, 1)

void
print_val(Value v)
{
        switch (v.type) {
        case TYPE_NUM:
                printf("%d\n", v.num);
                break;
        case TYPE_STR:
                /* -- TEMP -- */
                if (strcmp("no-value", v.str) == 0) return;
                /* ---------- */
                printf("%s\n", v.str);
                break;
        default:
                report("No yet implemented: print_val for %s\n",
                       VALTYPE_REPR[v.type]);
                EXIT(1);
        }
}

Value
eval_litexpr(Expr *e)
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
        case IDENTIFIER:
                v = env_get(e->litexpr.value->str_literal);
                break;
        default:
                report("No yet implemented: eval_litexpr for %s\n",
                       TOKEN_REPR[e->litexpr.value->token]);
                EXIT(1);
        }
        return v;
}

void
panik_invalid_binop(Value L, vtoktype OP, Value R)
{
        report("Invalid binary operation %s between %s and %s\n",
               TOKEN_REPR[OP], VALTYPE_REPR[L.type], VALTYPE_REPR[R.type]);
        EXIT(1);
}

void
panik_invalid_unop(vtoktype OP, Value R)
{
        report("Invalid binary operation %s for %s\n",
               TOKEN_REPR[OP], VALTYPE_REPR[R.type]);
        EXIT(1);
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
                EXIT(1);
        }
}

int
is_equal(Value v1, Value v2)
{
        if (v1.type != v2.type) return 0;

        switch (v1.type) {
        case TYPE_NUM:
                return v1.num == v2.num;
        case TYPE_STR:
                return strcmp(v1.str, v2.str) == 0;
        default:
                report("No yet implemented: is_equal for %s and %s\n",
                       VALTYPE_REPR[v1.type], VALTYPE_REPR[v1.type]);
                EXIT(1);
        }
}

int
is_greater(Value v1, Value v2)
{
        if (v1.type != v2.type) return 0;

        switch (v1.type) {
        case TYPE_NUM:
                return v1.num > v2.num;
        case TYPE_STR:
                return strcmp(v1.str, v2.str) > 0;
        default:
                report("No yet implemented: is_greater for %s and %s\n",
                       VALTYPE_REPR[v1.type], VALTYPE_REPR[v1.type]);
                EXIT(1);
        }
}

int
is_greater_equal(Value v1, Value v2)
{
        return is_equal(v1, v2) || is_greater(v1, v2);
}

Value
eval_binexpr(Expr *e)
{
        Value v;
        Value lhs = eval_expr(e->binexpr.lhs);
        Value rhs = eval_expr(e->binexpr.rhs);

        switch (e->binexpr.op->token) {
        case MINUS:
                if (rhs.type == TYPE_NUM && lhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num - lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case PLUS:
                if (rhs.type == TYPE_NUM && lhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num + lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case SLASH:
                if (rhs.type == TYPE_NUM && lhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num / lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case STAR:
                if (rhs.type == TYPE_NUM && lhs.type == TYPE_NUM) {
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

        case BITWISE_AND:
                if (rhs.type == TYPE_NUM && lhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num & lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case BITWISE_OR:
                if (rhs.type == TYPE_NUM && lhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num | lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case BITWISE_XOR:
                if (rhs.type == TYPE_NUM && lhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num ^ lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case EQUAL_EQUAL:
                v.type = TYPE_NUM;
                v.num = is_equal(lhs, rhs);
                break;

        case BANG_EQUAL:
                v.type = TYPE_NUM;
                v.num = !is_equal(lhs, rhs);
                break;

        case GREATER:
                v.type = TYPE_NUM;
                v.num = is_greater(lhs, rhs);
                break;

        case GREATER_EQUAL:
                v.type = TYPE_NUM;
                v.num = is_greater_equal(lhs, rhs);
                break;

        case LESS:
                v.type = TYPE_NUM;
                v.num = !is_greater_equal(lhs, rhs);
                break;

        case LESS_EQUAL:
                v.type = TYPE_NUM;
                v.num = !is_greater(lhs, rhs);
                break;

        default:
                report("Binexpr Operation no yet implemented: %s\n",
                       TOKEN_REPR[e->litexpr.value->token]);
                EXIT(1);
        }
        return v;
}

Value
eval_unexpr(Expr *e)
{
        Value v;
        Value lhs = eval_expr(e->unexpr.rhs);

        switch (e->unexpr.op->token) {
        case BANG:
                v.type = TYPE_NUM;
                v.num = !is_true(lhs);
                break;

        case MINUS:
                if (lhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = -lhs.num;
                        break;
                }
                panik_invalid_unop(e->unexpr.op->token, lhs);

        case BITWISE_NOT:
                if (lhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = ~lhs.num;
                        break;
                }
                panik_invalid_unop(e->unexpr.op->token, lhs);

        default:
                report("unexpr operation no yet implemented: %s\n",
                       TOKEN_REPR[e->litexpr.value->token]);
                EXIT(1);
        }
        return v;
}

Value eval_expr(Expr *e);

Value
eval_assignexpr(Expr *s)
{
        return env_set(s->assignexpr.name->str_literal,
                       eval_expr(s->assignexpr.value));
}

Value
eval_expr(Expr *e)
{
        Value v;
        switch (e->type) {
        case LITEXPR:
                v = eval_litexpr(e);
                break;
        case BINEXPR:
                v = eval_binexpr(e);
                break;
        case UNEXPR:
                v = eval_unexpr(e);
                break;
        case ASSIGNEXPR:
                v = eval_assignexpr(e);
                break;
        case CALLEXPR:
        case VAREXPR:
        default:
                report("No yet implemented: eval_expr for %s\n", EXPR_REPR[e->type]);
                EXIT(1);
                break;
        }
        return v;
}

Value
eval_stmt(Stmt *s)
{
        Value v = (Value) { .type = TYPE_STR, .str = "no-value" };
        switch (s->type) {
        case EXPRSTMT:
                v = eval_expr(s->expr.body);
                break;
        case VARDECLSTMT:
                v = env_add(s->vardecl.name->str_literal,
                            eval_expr(s->vardecl.value));
                if (s->vardecl.value) return v;
                break;
        case ASSERTSTMT:
                if (!is_true(eval_expr(s->assert.body))) {
                        report("Assert failed\n");
                        EXIT(1);
                }
                break;
        case BLOCKSTMT:
        default:
                report("Todo: eval_stmt for %s\n", STMT_REPR[s->type]);
                EXIT(1);
                break;
        }
        return v;
}

void
eval()
{
        Stmt *s = head_stmt;
        if (setjmp(eval_runtime_error)) {
                s = s->next;
        }
        while (s) {
                print_val(eval_stmt(s));
                s = s->next;
        }
}
