#include <setjmp.h>

#include "env.h"
#include "interpreter.h"
#include "tokens.h"

#include "stb_ds.h"

jmp_buf resolve_error_jmp;

/* Hashmap Expr * -> int (num of jumps to env that contains definition) */
struct {
        Expr *key;
        int value;
} *sidetable = NULL;

/* Hashmap that store variable status */
struct {
        char *key;
        enum { UNDECLARED = 0,
               DECLARED = 1,
               DEFINED = 3,
        } value;
} *varstatus = NULL;

static void
resolve_error()
{
        longjmp(resolve_error_jmp, 1);
}

static void
change_status(char *name, int type)
{
        switch (type) {
        case DEFINED:
        case DECLARED:
        case UNDECLARED:
                break;
        default:
                report("Invalid type in change_status\n");
                resolve_error();
                break;
        }
        shput(varstatus, name, type);
}

static int
get_status(char *name)
{
        int i = shgeti(varstatus, name);
        if (i < 0) return UNDECLARED;
        return varstatus[i].value;
}

static void
sidetable_add(Expr *e)
{
        switch (e->type) {
        case VAREXPR:
                hmput(sidetable, e, env_get_offset(e->varexpr.name->str_literal));
                break;
        default:
                report("No yet implemented: sidetable_add for %s\n",
                       EXPR_REPR[e->type]);
                resolve_error();
        }
}

static int
sidetable_get(Expr *e)
{
        int i;
        switch (e->type) {
        case LITEXPR:
                if (e->litexpr.value->token != IDENTIFIER) {
                        report("Var is not an identifier\n");
                        resolve_error();
                }
                i = hmgeti(sidetable, e);
                if (i < 0) {
                        if (get_status(e->litexpr.value->str_literal) & DEFINED)
                                return 0; // core functions
                        report("[sidetable] Var `%s` not defined\n",
                               e->litexpr.value->str_literal);
                        resolve_error();
                }
                break;

        case ASSIGNEXPR:
                i = hmgeti(sidetable, e);
                if (i < 0) {
                        report("[sidetable] Var `%s` not defined\n",
                               e->assignexpr.name->str_literal);
                        resolve_error();
                }
                break;

        default:
                report("No yet implemented: sidetable_get for %s\n",
                       EXPR_REPR[e->type]);
                resolve_error();
        }
        return sidetable[i].value;
}

static void
define_core()
{
        Env *env = get_current_env();
        int len = hmlenu(env->map);
        int i;
        for (i = 0; i < len; i++) {
                change_status(env->map[i].key, DEFINED);
                printf("[STATUS] var %s : DEFINED\n", env->map[i].key);
        }
}

static void
check_defined(Expr *e)
{
        switch (e->type) {
        case CALLEXPR:
                check_defined(e->callexpr.name);
                Expr *a = e->callexpr.args;
                while (a) {
                        check_defined(a);
                        a = a->next;
                }
                break;

        case LITEXPR:
                if (e->litexpr.value->token != IDENTIFIER) return;
                if (!(get_status(e->litexpr.value->str_literal) & DEFINED)) {
                        report("Var `%s` is not defined\n",
                               e->litexpr.value->str_literal);
                        resolve_error();
                }
                break;

        default:
                report("No yet implemented: check_defined for %s\n",
                       EXPR_REPR[e->type]);
                resolve_error();
                break;
        }
}

static void
check_declared(Expr *e)
{
        switch (e->type) {
        default:
                report("No yet implemented: check_declared for %s\n",
                       EXPR_REPR[e->type]);
                resolve_error();
                break;
        }
}

static void
define(char *name)
{
        change_status(name, DEFINED);
}

static void
resolve_declaration(Stmt *s)
{
        switch (s->type) {
        case VARDECLSTMT:
                if (s->vardecl.value)
                        change_status(s->vardecl.name->str_literal, DEFINED);
                else
                        change_status(s->vardecl.name->str_literal, DECLARED);
                define(s->vardecl.name->str_literal);
                break;
        case FUNDECLSTMT:
                change_status(s->funcdecl.name->str_literal, DEFINED);
                define(s->funcdecl.name->str_literal);
                vtok *a = s->funcdecl.params;
                while (a) {
                        define(a->str_literal);
                        a = a->next;
                }
                printf("[STATUS] var %s : DEFINED\n", s->vardecl.name->str_literal);
                break;
        default:
                report("No yet implemented: resolve_declaration for %s\n",
                       STMT_REPR[s->type]);
                resolve_error();
                break;
        }
}

static void resolve_expr_arr(Expr *e);

static void
resolve_expr(Expr *e)
{
        switch (e->type) {
        case BINEXPR:
                resolve_expr(e->binexpr.lhs);
                resolve_expr(e->binexpr.rhs);
                break;
        case UNEXPR:
                resolve_expr(e->unexpr.rhs);
                break;
        case ANDEXPR:
                resolve_expr(e->andexpr.lhs);
                resolve_expr(e->andexpr.rhs);
                break;
        case OREXPR:
                resolve_expr(e->orexpr.lhs);
                resolve_expr(e->orexpr.rhs);
                break;
        case LITEXPR:
                break;
        case ASSIGNEXPR:
                define(e->assignexpr.name->str_literal);
                resolve_expr(e->assignexpr.value);
                break;
        case CALLEXPR:
                check_defined(e);
                resolve_expr_arr(e->callexpr.args);
                resolve_expr(e->callexpr.name);
                break;
        case VAREXPR:
        default:
                report("No yet implemented: resolve_expr for %s\n",
                       EXPR_REPR[e->type]);
                resolve_error();
        }
}

static void
resolve_expr_arr(Expr *e)
{
        while (e) {
                resolve_expr(e);
                e = e->next;
        }
}

static void resolve_stmt_arr(Stmt *s);

static void
resolve_stmt(Stmt *s)
{
        switch (s->type) {
        case BLOCKSTMT:
                resolve_stmt_arr(s->block.body);
                break;
        case EXPRSTMT:
                resolve_expr(s->expr.body);
                break;
        case ASSERTSTMT:
                resolve_expr(s->assert.body);
                break;
        case RETSTMT:
                resolve_expr(s->retstmt.value);
                break;
        case IFSTMT:
                resolve_expr(s->ifstmt.cond);
                resolve_stmt(s->ifstmt.body);
                resolve_stmt(s->ifstmt.elsebody);
                break;
        case WHILESTMT:
                resolve_expr(s->whilestmt.cond);
                resolve_stmt(s->whilestmt.body);
                break;
        case VARDECLSTMT:
                /* Expr before declaration to check for "var a = a;" */
                resolve_expr(s->vardecl.value);
                resolve_declaration(s);
                break;
        case FUNDECLSTMT:
                /* declaration before body to allow recursion */
                resolve_declaration(s);
                resolve_stmt_arr(s->funcdecl.body);
                break;

        default:
                report("No yet implemented: resolve_stmt for %s\n", s->type);
                resolve_error();
                break;
        }
}

static void
resolve_stmt_arr(Stmt *s)
{
        while (s) {
                resolve_stmt(s);
                s = s->next;
        }
}

void
resolve()
{
        if (setjmp(resolve_error_jmp)) {
                return;
        }
        define_core();
        resolve_stmt_arr(head_stmt);
}

Value
env_get_expr(Expr *e, char *name)
{
        return env_get(name);
        return env_get_o(sidetable_get(e), name);
}

Value
env_set_expr(Expr *e, char *name, Value value)
{
        return env_set(name, value);
        return env_set_o(sidetable_get(e), name, value);
}
