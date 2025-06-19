#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tokens.h"

// clang-format off
typedef struct Expr {
        union {
                struct Assignexpr { struct Expr *value; vtok *name; } Assignexpr;
                struct Binexpr    { struct Expr *rhs; struct Expr *lhs; vtok *op; } binexpr;
                struct Unexpr     { struct Expr *rhs; vtok *op; } unexpr;
                struct Callexpr   { struct Expr **args; int count; vtok *name; } callexpr;
                struct Varexpr    { struct Expr *value; vtok *name; } varexpr;
                struct Litexpr    { vtok *value; } litexpr;
        };
        enum {
                ASSIGNEXPR,
                BINEXPR,
                UNEXPR,
                CALLEXPR,
                LITEXPR,
                VAREXPR,
        } type;
        /* Linked list stuff */
        struct Expr *next;
        struct Expr *prev;
} Expr;
// clang-format on

typedef struct Unop {
        Expr lhs;
        vtok op;
} Unop;

extern vtok *head_token;
vtok *current_token = NULL;
Expr *head_expr = NULL;

static const char *EXPR_REPR[] = {
        [ASSIGNEXPR] = "Assign",
        [BINEXPR] = "Binary",
        [UNEXPR] = "Unary",
        [CALLEXPR] = "Call",
        [LITEXPR] = "Literal",
        [VAREXPR] = "Variable",
};

void
print_parsed_token(Expr *e)
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
                print_parsed_token(e->binexpr.lhs);
                printf("%*s", indent * indent_size, "");
                printf("- [op] %s\n", TOKEN_REPR[e->binexpr.op->token]);
                printf("%*s", indent * indent_size, "");
                printf("- [rhs] ");
                print_parsed_token(e->binexpr.rhs);
                break;
        case UNEXPR:
                printf("%*s", indent * indent_size, "");
                printf("- [op] %s\n", TOKEN_REPR[e->unexpr.op->token]);
                printf("%*s", indent * indent_size, "");
                printf("- [rhs] ");
                print_parsed_token(e->unexpr.rhs);
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
print_parsed_tokens()
{
        printf("------------------------------\n");
        Expr *e = head_expr;
        while (e) {
                print_parsed_token(e);
                e = e->next;
        }
        printf("------------------------------\n");
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

Expr *
get_literal()
{
        vtok *t;
        Expr *e;
        // clang-format off
        if ((t=match(NUMBER))
         || (t=match(STRING))
         || (t=match(CHAR))
         || (t=match(TRUE))
         || (t=match(FALSE)))
        // clang-format on
        {
                e = new_litexpr(t);
                return e;
        }

        printf("Expected literal but got");
        print_token(get_consume_token());
        return NULL;
}

Expr *
get_unary()
{
        vtok *op;
        Expr *e;
        if ((op = match(MINUS)) || (op = match(BANG))) {
                e = new_unexpr(op, get_unary());
        } else
                e = get_literal();
        return e;
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
        current_token = head_token;

        for (;;) {
                link_expression(get_expression());
                if (get_token()->token == END_OF_FILE) break;
                print_token(get_token());
        }
}


int
main(int argc, char **argv)
{
        char buf[1024 * 1024];
        int fd;
        if (argc == 2)
                fd = open(argv[1], O_RDONLY);
        else
                fd = open("../hello.vspl", O_RDONLY);

        if (fd < 0) {
                report("Can not open to read file `%s`\n", argv[1]);
                return -1;
        }
        ssize_t n = read(fd, buf, sizeof buf - 1);
        if (n <= 0) {
                report("Can not read from `%s`\n", argv[1]);
                return -1;
        }
        buf[n] = EOF;

        lex_analize(buf);
        print_tokens();
        tok_parse();
        print_parsed_tokens();

        return 0;
}
