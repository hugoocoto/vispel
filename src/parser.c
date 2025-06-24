/* VISPEL parser - Create AST from tokens
 *
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/vispel
 *
 * */

#include <assert.h>
#include <fcntl.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "env.h"
#include "tokens.h"

vtok *current_token = NULL;
Stmt *head_stmt = NULL;
jmp_buf panik_jmp;

void
print_ast_expr_branch(Expr *e)
{
        static int indent = 0;
        const int indent_size = 2;
        printf("Expression: %s\n", EXPR_REPR[e->type]);
        ++indent;

        switch (e->type) {
        case ASSIGNEXPR:
                printf("%*s", indent * indent_size, "");
                printf("- [name] %s\n", e->assignexpr.name->str_literal);
                printf("%*s", indent * indent_size, "");
                printf("- [value] %s\n", TOKEN_REPR[e->assignexpr.value->type]);
                printf("%*s", indent * indent_size, "");
                print_ast_expr_branch(e->assignexpr.value);
                break;
        case BINEXPR:
                printf("%*s", indent * indent_size, "");
                printf("- [lhs] ");
                print_ast_expr_branch(e->binexpr.lhs);
                printf("%*s", indent * indent_size, "");
                printf("- [op] %s\n", TOKEN_REPR[e->binexpr.op->token]);
                printf("%*s", indent * indent_size, "");
                printf("- [rhs] ");
                print_ast_expr_branch(e->binexpr.rhs);
                break;
        case UNEXPR:
                printf("%*s", indent * indent_size, "");
                printf("- [op] %s\n", TOKEN_REPR[e->unexpr.op->token]);
                printf("%*s", indent * indent_size, "");
                printf("- [rhs] ");
                print_ast_expr_branch(e->unexpr.rhs);
                break;
        case LITEXPR:
                printf("%*s", indent * indent_size, "");
                printf("- [lit] ");
                print_literal(e->litexpr.value);
                printf("\n");
                break;
        case CALLEXPR:
        case VAREXPR:
        case ANDEXPR:
        case OREXPR:
        default:
                report("print_ast_expr_branch not yet implemeted for %s\n",
                       EXPR_REPR[e->type]);
                break;
        }

        --indent;
}

void
print_ast_branch(Stmt *s)
{
        switch (s->type) {
        case EXPRSTMT:
                print_ast_expr_branch(s->expr.body);
                break;
        case VARDECLSTMT:
                printf("var %s = ", s->vardecl.name->str_literal);
                print_ast_expr_branch(s->vardecl.value);
                break;
        case ASSERTSTMT:
                printf("assert ");
                print_ast_expr_branch(s->assert.body);
                break;
        case IFSTMT:
                printf("if ");
                print_ast_expr_branch(s->ifstmt.cond);
                printf("if true: ");
                print_ast_branch(s->ifstmt.body);
                if (s->ifstmt.elsebody) {
                        printf("if false: ");
                        print_ast_branch(s->ifstmt.elsebody);
                }
                break;
        case BLOCKSTMT:
                printf("block ");
                s = s->block.body;
                while (s) {
                        print_ast_branch(s);
                        s = s->next;
                }
                break;
        case WHILESTMT:
                printf("while ");
                print_ast_expr_branch(s->ifstmt.cond);
                printf("body: ");
                print_ast_branch(s->ifstmt.body);
                break;
        default:
                report("No yet implemented: print_ast_branch for %s\n",
                       STMT_REPR[s->type]);
                break;
        }
}

void
print_ast()
{
        printf("-----------|AST|-----------\n");
        Stmt *e = head_stmt;
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
new_orexpr(Expr *lhs, Expr *rhs)
{
        Expr *e = new_expr();
        e->orexpr.lhs = lhs;
        e->orexpr.rhs = rhs;
        e->type = OREXPR;
        return e;
}

Expr *
new_andexpr(Expr *lhs, Expr *rhs)
{
        Expr *e = new_expr();
        e->andexpr.lhs = lhs;
        e->andexpr.rhs = rhs;
        e->type = ANDEXPR;
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
new_assignexpr(vtok *name, Expr *value)
{
        Expr *e = new_expr();
        e->assignexpr.name = name;
        e->assignexpr.value = value;
        e->type = ASSIGNEXPR;
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
        /* I add EOF here so I can evaluate a single expression witout
         * provide the semicolon.
         * This is not a feature, but a humman-bug fix */
        if (expected == SEMICOLON && get_token()->token == END_OF_FILE) return;
        expect_token(expected);
        consume_token();
}

void
link_stmt(Stmt *s)
{
        if (head_stmt == NULL) {
                head_stmt = s;
                return;
        }
        Stmt *last = head_stmt;
        while (last->next)
                last = last->next;
        last->next = s;
}

vtok *
is_literal()
{
        vtok *t;
        if ((t = match(NUMBER)) ||
            (t = match(STRING)) ||
            (t = match(CHAR)) ||
            (t = match(IDENTIFIER)) ||
            (t = match(TRUE)) ||
            (t = match(FALSE)))
                return t;
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
        while ((op = match(GREATER)) ||
               (op = match(GREATER_EQUAL)) ||
               (op = match(LESS)) ||
               (op = match(LESS_EQUAL))) {
                e = new_binexpr(e, op, get_term());
        }
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
get_or()
{
        Expr *e = get_equality();
        if (match(OR)) {
                e = new_orexpr(e, get_or());
        }
        return e;
}

Expr *
get_and()
{
        Expr *e = get_or();
        if (match(AND)) {
                e = new_andexpr(e, get_and());
        }
        return e;
}

Expr *
get_assignment()
{
        vtok *id;
        if ((id = match(IDENTIFIER))) {
                if (match(EQUAL))
                        return new_assignexpr(id, get_expression());
                current_token = id;
        }
        return get_and();
}

Expr *
get_expression()
{
        return get_assignment();
}

Stmt *
new_stmt()
{
        return calloc(1, sizeof(Stmt));
}

Stmt *
new_blockstmt()
{
        Stmt *s = new_stmt();
        s->type = BLOCKSTMT;
        s->block.body = NULL;
        return s;
}

Stmt *
new_exprstmt(Expr *e)
{
        Stmt *s = new_stmt();
        s->type = EXPRSTMT;
        s->expr.body = e;
        return s;
}

Stmt *
new_whilestmt(Expr *e, Stmt *body)
{
        Stmt *s = new_stmt();
        s->type = WHILESTMT;
        s->whilestmt.cond = e;
        s->whilestmt.body = body;
        return s;
}

Stmt *
new_ifstmt(Expr *e, Stmt *body, Stmt *elsebody)
{
        Stmt *s = new_stmt();
        s->type = IFSTMT;
        s->ifstmt.cond = e;
        s->ifstmt.body = body;
        s->ifstmt.elsebody = elsebody;
        return s;
}

Stmt *
new_assertstmt(Expr *e)
{
        Stmt *s = new_stmt();
        s->type = ASSERTSTMT;
        s->assert.body = e;
        return s;
}

Stmt *
new_vardecl(vtok *id, Expr *value)
{
        Stmt *s = new_stmt();
        s->type = VARDECLSTMT;
        s->vardecl.name = id;
        s->vardecl.value = value;
        return s;
}

void
blockstmt_addstmt(Stmt *block, Stmt *s)
{
        if (block->block.body == NULL) {
                block->block.body = s;
                return;
        }
        Stmt *current = block->block.body;
        while (current->next)
                current = current->next;
        current->next = s;
}

Stmt *get_declaration();

Stmt *
get_program()
{
        Stmt *c = NULL;
        Stmt *ret;
        Stmt *s;
        while (get_token()->token != END_OF_FILE) {
                s = get_declaration();
                if (c) {
                        c->next = s;
                } else
                        ret = s;
                c = s;
        }
        return ret;
}

Stmt *get_vardecl();
Stmt *get_stmt();

Stmt *
get_declaration()
{
        return get_vardecl() ?: get_stmt();
}

Expr *
littok_novalue()
{
        Expr *e = new_expr();
        e->litexpr.value = calloc(1, sizeof(vtok));
        e->litexpr.value->str_literal = "no-value";
        e->litexpr.value->token = STRING;
        e->type = LITEXPR;
        return e;
}

Stmt *
get_vardecl()
{
        vtok *t = current_token;
        vtok *id;
        Expr *value = NULL;
        if (match(VAR) && (id = match(IDENTIFIER))) {
                if (match(EQUAL)) {
                        value = get_expression();
                } else
                        value = littok_novalue();
                expect_consume_token(SEMICOLON);
                return new_vardecl(id, value);
        }
        current_token = t;
        return NULL;
}

Stmt *get_block();
Stmt *get_exprstmt();
Stmt *get_assert();
Stmt *get_ifstmt();
Stmt *get_whilestmt();

Stmt *
get_stmt()
{
        if (match(ASSERT)) return get_assert();
        if (match(LEFT_BRACE)) return get_block();
        if (match(IF)) return get_ifstmt();
        if (match(WHILE)) return get_whilestmt();
        return get_exprstmt();
}

Stmt *
get_whilestmt()
{
        expect_consume_token(LEFT_PARENT);
        Expr *e = get_expression();
        expect_consume_token(RIGHT_PARENT);
        return new_whilestmt(e, get_declaration());
}

Stmt *
get_ifstmt()
{
        expect_consume_token(LEFT_PARENT);
        Expr *e = get_expression();
        expect_consume_token(RIGHT_PARENT);
        Stmt *body = get_declaration();
        Stmt *elsebody = NULL;
        if (match(ELSE)) {
                elsebody = get_declaration();
        }
        return new_ifstmt(e, body, elsebody);
}

Stmt *
get_exprstmt()
{
        Stmt *s = new_exprstmt(get_expression());
        expect_consume_token(SEMICOLON);
        return s;
}

Stmt *
get_assert()
{
        Stmt *s = new_assertstmt(get_expression());
        expect_consume_token(SEMICOLON);
        return s;
}

Stmt *
get_block()
{
        Stmt *s = new_blockstmt();
        while (!match(RIGHT_BRACE))
                blockstmt_addstmt(s, get_declaration());
        return s;
}

void
tok_parse()
{
        if (head_token == NULL) {
                report("tok_parse: invalid token list. Call lex_analize() first.\n");
                exit(1);
        }

        head_stmt = NULL;
        current_token = head_token;

        /* Set point to reset after failure */
        if (setjmp(panik_jmp)) {
                /* This is executed after failure. Go down to the
                 * next semicolon, as current expression failed. After
                 * the semicolon it should continue without problems. */
                vtok *tok;
                for (;;) {
                        tok = get_token();
                        if (tok->token == END_OF_FILE) return;
                        consume_token();
                        if (tok->token == SEMICOLON) break;
                }
        }
        link_stmt(get_program());
}
