/* VISPEL interpreter - Evalue AST branch
 *
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/vispel
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                longjmp(panik_jmp, 1);
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
        default:
                report("No yet implemented: eval_litexpr for %s\n",
                       TOKEN_REPR[e->litexpr.value->token]);
                longjmp(panik_jmp, 1);
        }
        return v;
}

void
panik_invalid_binop(Value L, vtoktype OP, Value R)
{
        report("Invalid binary operation %s between %s and %s\n",
               TOKEN_REPR[OP], VALTYPE_REPR[L.type], VALTYPE_REPR[R.type]);
        longjmp(panik_jmp, 1);
}

void
panik_invalid_unop(vtoktype OP, Value R)
{
        report("Invalid binary operation %s for %s\n",
               TOKEN_REPR[OP], VALTYPE_REPR[R.type]);
        longjmp(panik_jmp, 1);
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
                longjmp(panik_jmp, 1);
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
                longjmp(panik_jmp, 1);
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
                longjmp(panik_jmp, 1);
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

        case BITWISE_AND:
                if (rhs.type == TYPE_NUM && rhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num & lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case BITWISE_OR:
                if (rhs.type == TYPE_NUM && rhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num | lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case BITWISE_XOR:
                if (rhs.type == TYPE_NUM && rhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = rhs.num ^ lhs.num;
                        break;
                }
                panik_invalid_binop(lhs, e->binexpr.op->token, rhs);

        case EQUAL_EQUAL:
                v.type = TYPE_NUM;
                v.num = is_equal(rhs, lhs);
                break;

        case BANG_EQUAL:
                v.type = TYPE_NUM;
                v.num = !is_equal(rhs, lhs);
                break;

        case GREATER:
                v.type = TYPE_NUM;
                v.num = is_greater(rhs, lhs);
                break;

        case GREATER_EQUAL:
                v.type = TYPE_NUM;
                v.num = is_greater_equal(rhs, lhs);
                break;

        case LESS:
                v.type = TYPE_NUM;
                v.num = !is_greater_equal(rhs, lhs);
                break;

        case LESS_EQUAL:
                v.type = TYPE_NUM;
                v.num = !is_greater(rhs, lhs);
                break;

        default:
                report("Binexpr Operation no yet implemented: %s\n",
                       TOKEN_REPR[e->litexpr.value->token]);
                longjmp(panik_jmp, 1);
        }
        return v;
}

Value
eval_unexpr(Expr *e)
{
        Value v;
        Value lhs = eval_expr(e->binexpr.lhs);

        switch (e->unexpr.op->token) {
        case BANG:
                v.type = TYPE_NUM;
                v.num = !is_true(lhs);
                break;

        case MINUS:
                if (lhs.type == TYPE_NUM) {
                        v.type = TYPE_NUM;
                        v.num = !is_true(lhs);
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
                report("Binexpr Operation no yet implemented: %s\n",
                       TOKEN_REPR[e->litexpr.value->token]);
                longjmp(panik_jmp, 1);
        }
        return v;
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
        case CALLEXPR:
        case ASSIGNEXPR:
        case VAREXPR:
        default:
                report("Todo: eval expression %s\n", EXPR_REPR[e->type]);
                longjmp(panik_jmp, 1);
                break;
        }
        return v;
}

Value
eval_stmt(Stmt *s)
{
        switch (s->type) {
        case EXPRSTMT:
                return eval_expr(s->expr.body);
        case VARDECLSTMT:
        case BLOCKSTMT:
        default:
                report("Todo: eval_stmt for %s\n", STMT_REPR[s->type]);
                longjmp(panik_jmp, 1);
                break;
        }
        return (Value) { .type = TYPE_STR, .str = "no-value" };
}

void
eval()
{
        Stmt *s = head_stmt;
        while (s) {
                print_val(eval_stmt(s));
                s = s->next;
        }
}
