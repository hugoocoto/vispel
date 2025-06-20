/* VISPEL parser - Create AST from tokens
 *
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/vispel
 *
 * */

#include <fcntl.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tokens.h"

vtok *current_token = NULL;
Expr *head_expr = NULL;
jmp_buf panik_jmp;


void
print_ast_branch(Expr *e)
{
        static int indent = 0;
        const int indent_size = 2;
        printf("Expression: %s\n", EXPR_REPR[e->type]);
        ++indent;

        switch (e->type) {
        case ASSIGNEXPR:
                printf("[assignexpr] Todo\n");
                break;
        case BINEXPR:
                printf("%*s", indent * indent_size, "");
                printf("- [lhs] ");
                print_ast_branch(e->binexpr.lhs);
                printf("%*s", indent * indent_size, "");
                printf("- [op] %s\n", TOKEN_REPR[e->binexpr.op->token]);
                printf("%*s", indent * indent_size, "");
                printf("- [rhs] ");
                print_ast_branch(e->binexpr.rhs);
                break;
        case UNEXPR:
                printf("%*s", indent * indent_size, "");
                printf("- [op] %s\n", TOKEN_REPR[e->unexpr.op->token]);
                printf("%*s", indent * indent_size, "");
                printf("- [rhs] ");
                print_ast_branch(e->unexpr.rhs);
                break;
        case CALLEXPR:
                printf("[callexpr] Todo\n");
                break;
        case LITEXPR:
                printf("%*s", indent * indent_size, "");
                printf("- [lit] ");
                print_literal(e->litexpr.value);
                printf("\n");
                break;
        case VAREXPR:
                printf("[varexpr] Todo\n");
                break;
        }

        --indent;
}

void
print_ast()
{
        printf("-----------|AST|-----------\n");
        Expr *e = head_expr;
        while (e) {
                print_ast_branch(e);
                e = e->next;
        }
        printf("---------------------------\n");
}

/* Use only to create a expr of a concrete type */
Expr *
new_expr()
{
        Expr *e = malloc(sizeof(Expr));
        memset(e, 0, sizeof(Expr));
        return e;
}

Expr *
new_binexpr(Expr *lhs, vtok *op, Expr *rhs)
{
        Expr *e = new_expr();
        e->binexpr.lhs = lhs;
        e->binexpr.op = op;
        e->binexpr.rhs = rhs;
        e->type = BINEXPR;
        return e;
}

Expr *
new_unexpr(vtok *op, Expr *rhs)
{
        Expr *e = new_expr();
        e->unexpr.rhs = rhs;
        e->unexpr.op = op;
        e->type = UNEXPR;
        return e;
}

Expr *
new_litexpr(vtok *value)
{
        Expr *e = new_expr();
        e->litexpr.value = value;
        e->type = LITEXPR;
        return e;
}

vtok *
get_token()
{
        return current_token;
}

vtok *
consume_token()
{
        if (current_token) current_token = current_token->next;
        return current_token;
}

vtok *
get_consume_token()
{
        vtok *tok = get_token();
        consume_token();
        return tok;
}

vtok *
match(vtoktype token)
{
        vtok *tok = get_token();
        if (tok->token == token) {
                consume_token();
                return tok;
        }
        return NULL;
}

void
expect_token(vtoktype expected)
{
        vtok *tok = get_token();
        if (tok->token != expected) {
                report("Expected %s but got %s\n", TOKEN_REPR[expected], TOKEN_REPR[tok->token]);
                longjmp(panik_jmp, 1);
        }
}

void
expect_consume_token(vtoktype expected)
{
        expect_token(expected);
        consume_token();
}

void
link_expression(Expr *e)
{
        if (head_expr == NULL) {
                head_expr = e;
                return;
        }
        Expr *last = head_expr;
        while (last->next)
                last = last->next;
        last->next = e;
}

vtok *
is_literal()
{
        vtok *t;
        // clang-format off
        if ((t=match(NUMBER))
         || (t=match(STRING))
         || (t=match(CHAR))
         || (t=match(TRUE))
         || (t=match(FALSE)))
                return t;
        // clang-format on
        return NULL;
}

Expr *
get_literal()
{
        vtok *t;
        if ((t = is_literal())) {
                return new_litexpr(t);
        }
        /* can't be used expect() because LITERAL is an expression, not a token */
        report("Expected LITERAL but got %s\n", TOKEN_REPR[get_token()->token]);
        longjmp(panik_jmp, 1);
        return NULL;
}

Expr *get_expression();

Expr *
get_group()
{
        Expr *e;
        if (match(LEFT_PARENT)) {
                e = get_expression();
                expect_consume_token(RIGHT_PARENT);
                return e;
        }
        return get_literal();
}

Expr *
get_unary()
{
        vtok *op;
        if ((op = match(MINUS)) || (op = match(BANG))) {
                return new_unexpr(op, get_unary());
        } else
                return get_group();
}

Expr *
get_factor()
{
        Expr *e = get_unary();
        vtok *op;
        while ((op = match(SLASH)) || (op = match(STAR))) {
                e = new_binexpr(e, op, get_unary());
        }
        return e;
}

Expr *
get_term()
{
        Expr *e = get_factor();
        vtok *op;
        while ((op = match(MINUS)) || (op = match(PLUS))) {
                e = new_binexpr(e, op, get_factor());
        }
        return e;
}

Expr *
get_comparison()
{
        Expr *e = get_term();
        vtok *op;
        // clang-format off
        while ((op = match(GREATER))
            || (op = match(GREATER_EQUAL))
            || (op = match(LESS))
            || (op = match(LESS_EQUAL))) {
                e = new_binexpr(e, op, get_term());
        }
        // clang-format on
        return e;
}

Expr *
get_equality()
{
        Expr *e = get_comparison();
        vtok *op;
        while ((op = match(EQUAL_EQUAL)) || (op = match(BANG_EQUAL))) {
                e = new_binexpr(e, op, get_comparison());
        }
        return e;
}

Expr *
get_expression()
{
        return get_equality();
}

void
tok_parse()
{
        if (head_token == NULL) {
                report("tok_parse: invalid token list. Call lex_analize() first.\n");
                exit(1);
        }

        head_expr = NULL;
        current_token = head_token;

        for (;;) {
                /* Set point to reset after failure */
                if (setjmp(panik_jmp)) {
                        /* This is executed after failure. Go down to the
                         * next semicolon, as current expression failed. After
                         * the semicolon it should continue without problems. */
                        vtok *tok;
                        for (;;) {
                                tok = get_token();
                                if (tok->token == END_OF_FILE) break;
                                consume_token();
                                if (tok->token == SEMICOLON) break;
                        }
                        continue;
                }

                if (get_token()->token == END_OF_FILE) break;

                /* Get an expression and link it to the list of expressions */
                link_expression(get_expression());

                if (!match(SEMICOLON)) expect_token(SEMICOLON);
        }
}
