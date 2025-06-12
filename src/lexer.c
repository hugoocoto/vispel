/* VISPEL lexer
 *
 * Author: Hugo Coto Florez
 * Repo: https://github.com/hugocotoflorez/vispel
 */

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum {
        LEFT_PARENT,
        RIGHT_PARENT,
        LEFT_BRACE,
        RIGHT_BRACE,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        IDENTIFIER,
        STRING,
        NUMBER,
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUNCTION,
        FOR,
        IF,
        NIL,
        OR,
        EXTERN,
        RETURN,
        TRUE,
        WHILE,
        END_OF_FILE,
        BITWISE_AND,
        BITWISE_OR,
        BITWISE_XOR,
        BITWISE_NOT,
        PLUS_PLUS,
        LESS_LESS,
        SHIFT_LEFT,
        SHIFT_RIGHT,
        FUNC_INPUT,
        FUNC_OUTPUT,
        CHAR,
        UNKNOWN,
} vtoken;

static const char *TOKEN_REPR[] = {
        [LEFT_PARENT] = "LEFT_PARENT",
        [RIGHT_PARENT] = "RIGHT_PARENT",
        [LEFT_BRACE] = "LEFT_BRACE",
        [RIGHT_BRACE] = "RIGHT_BRACE",
        [LEFT_BRACKET] = "LEFT_BRACKET",
        [RIGHT_BRACKET] = "RIGHT_BRACKET",
        [COMMA] = "COMMA",
        [DOT] = "DOT",
        [MINUS] = "MINUS",
        [PLUS] = "PLUS",
        [SEMICOLON] = "SEMICOLON",
        [SLASH] = "SLASH",
        [STAR] = "STAR",
        [BANG] = "BANG",
        [BANG_EQUAL] = "BANG_EQUAL",
        [EQUAL] = "EQUAL",
        [EQUAL_EQUAL] = "EQUAL_EQUAL",
        [GREATER] = "GREATER",
        [GREATER_EQUAL] = "GREATER_EQUAL",
        [LESS] = "LESS",
        [LESS_EQUAL] = "LESS_EQUAL",
        [IDENTIFIER] = "IDENTIFIER",
        [STRING] = "STRING",
        [NUMBER] = "NUMBER",
        [AND] = "AND",
        [CLASS] = "CLASS",
        [ELSE] = "ELSE",
        [FALSE] = "FALSE",
        [FUNCTION] = "FUNCTION",
        [FOR] = "FOR",
        [IF] = "IF",
        [NIL] = "NIL",
        [OR] = "OR",
        [EXTERN] = "EXTERN",
        [RETURN] = "RETURN",
        [TRUE] = "TRUE",
        [WHILE] = "WHILE",
        [END_OF_FILE] = "END_OF_FILE",
        [BITWISE_AND] = "BITWISE_AND",
        [BITWISE_OR] = "BITWISE_OR",
        [BITWISE_XOR] = "BITWISE_XOR",
        [BITWISE_NOT] = "BITWISE_NOT",
        [PLUS_PLUS] = "PLUS_PLUS",
        [LESS_LESS] = "LESS_LESS",
        [SHIFT_LEFT] = "SHIFT_LEFT",
        [SHIFT_RIGHT] = "SHIFT_RIGHT",
        [FUNC_INPUT] = "FUNC_INPUT",
        [FUNC_OUTPUT] = "FUNC_OUTPUT",
        [CHAR] = "CHAR",
        [UNKNOWN] = "UNKNOWN",
};

typedef struct vlex {
        vtoken token;
        const char *lexeme;
        union {
                int num_literal;
                void *mem_literal;
                char *str_literal;
        };
        int line;
        intptr_t offset;
        /* Linked list stuff */
        struct vlex *next;
} vlex;

char *current_ptr = NULL;
int line = 1;
char *start_line;
char *start_offset;

void
report(char *format, ...)
{
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        FILE *f = fopen("log.txt", "a");
        if (f) {
                va_start(args, format);
                vfprintf(f, format, args);
                va_end(args);
                fclose(f);
        }
}

void
add_token(vtoken token, ...)
{
        va_list v;
        vlex *tok = calloc(1, sizeof *tok);
        tok->line = line;
        tok->token = token;
        tok->lexeme = TOKEN_REPR[token];
        tok->offset = start_offset - start_line + 1;
        report("[%2d:%2d] Token: %s", tok->line, tok->offset, TOKEN_REPR[token]);

        va_start(v, token);
        switch (token) {
        case STRING:
                tok->str_literal = va_arg(v, char *);
                report(" \"%s\"\n", tok->str_literal);
                break;
        case CHAR:
                tok->str_literal = va_arg(v, char *);
                report(" '%s'\n", tok->str_literal);
                break;
        case IDENTIFIER:
                tok->str_literal = va_arg(v, char *);
                report(" `%s`\n", tok->str_literal);
                break;
        case NUMBER:
                tok->num_literal = va_arg(v, int);
                report(" `%d`\n", tok->num_literal);
                break;

        default:
                report("\n");
                break;
        }
        va_end(v);
}

bool
match_word(const char *restrict word)
{
        int len = strlen(word);
        if (memcmp(word, current_ptr - 1, len)) return false;
        current_ptr += len - 1;
        return true;
}

bool
match(char expected)
{
        if (*current_ptr != expected) return false;
        ++current_ptr;
        return true;
}

#define get_consume_lex() (*current_ptr++)

char *
get_string()
{
        /* To test */
        char *ret = current_ptr;
        char tmp;

        do {
                tmp = get_consume_lex();
        } while (tmp != '"');

        current_ptr[-1] = 0;
        ret = strdup(ret);
        current_ptr[-1] = tmp;
        return ret;
}

char *
get_char()
{
        /* To test */
        char *ret = current_ptr;
        char tmp;

        do {
                tmp = get_consume_lex();
        } while (tmp != '\'');

        current_ptr[-1] = 0;
        ret = strdup(ret);
        current_ptr[-1] = tmp;
        return ret;
}

char *
get_identifier()
{
        /* To test */
        char *ret = current_ptr - 1;
        char tmp;

        do {
                tmp = get_consume_lex();
        } while (isalnum(tmp) || tmp == '_');

        current_ptr[-1] = 0;
        ret = strdup(ret);
        current_ptr[-1] = tmp;
        --current_ptr;
        return ret;
}

int
get_number()
{
        int n = current_ptr[-1];
        while (*current_ptr >= '0' && *current_ptr <= '9') {
                n *= 10;
                n += *current_ptr;
                ++current_ptr;
        }
        return n;
}

void
get_comment()
{
        for (;;) {
                if (*current_ptr == '\n' || *current_ptr == EOF || *current_ptr == 0) break;
                ++current_ptr;
        }
}

vlex
lex_analize(char *source)
{
        char current;
        current_ptr = source;
        start_line = current_ptr;
        for (;;) {
                start_offset = current_ptr;
                switch (current = get_consume_lex()) {
                case '(':
                        add_token(LEFT_PARENT);
                        break;
                case ')':
                        add_token(RIGHT_PARENT);
                        break;
                case '{':
                        add_token(LEFT_BRACE);
                        break;
                case '}':
                        add_token(RIGHT_BRACE);
                        break;
                case '[':
                        add_token(LEFT_BRACKET);
                        break;
                case ']':
                        add_token(RIGHT_BRACKET);
                        break;
                case ',':
                        add_token(COMMA);
                        break;
                case '.':
                        add_token(DOT);
                        break;
                case '^':
                        add_token(BITWISE_XOR);
                        break;
                case '~':
                        add_token(BITWISE_NOT);
                        break;
                case ';':
                        add_token(SEMICOLON);
                        break;
                case '*':
                        add_token(STAR);
                        break;

                case '/':
                        if (match('/'))
                                get_comment();
                        else
                                add_token(SLASH);
                        break;
                case '&':
                        if (match('&'))
                                add_token(AND);
                        else
                                add_token(BITWISE_AND);
                        break;
                case '+':
                        if (match('+'))
                                add_token(PLUS_PLUS);
                        else
                                add_token(PLUS);
                        break;
                case '-':
                        if (match('-'))
                                add_token(LESS_LESS);
                        else
                                add_token(LESS);
                        break;
                case '=':
                        if (match('='))
                                add_token(EQUAL_EQUAL);
                        else
                                add_token(EQUAL);
                        break;
                case '!':
                        if (match('='))
                                add_token(BANG_EQUAL);
                        else
                                add_token(BANG);
                        break;

                case '>':
                        if (match('>'))
                                add_token(SHIFT_LEFT);
                        else if (match('='))
                                add_token(GREATER_EQUAL);
                        else
                                add_token(GREATER);
                        break;
                case '<':
                        if (match('<'))
                                add_token(SHIFT_RIGHT);
                        else if (match('='))
                                add_token(LESS_EQUAL);
                        else
                                add_token(LESS);
                        break;

                case '|':
                        if (match('|'))
                                add_token(OR);
                        else if (match('>'))
                                add_token(FUNC_INPUT);
                        else if (match('<'))
                                add_token(FUNC_OUTPUT);
                        else
                                add_token(BITWISE_OR);
                        break;

                case '"':
                        add_token(STRING, get_string());
                        break;
                case '\'':
                        add_token(CHAR, get_char());
                        break;

                case '0' ... '9':
                        add_token(NUMBER, get_number());
                        break;

                case '\n':
                        start_line = current_ptr;
                        ++line;
                        break;
                case 0:
                case EOF:
                        add_token(END_OF_FILE);
                        break;

                default:
                        // clang-format off
                        if (isspace(current)) break;
                        if (!isalpha(current) && current != '_') {
                                report("[line %d] Invalid lexeme found: `%c`\n", line, current);
                                add_token(UNKNOWN);
                                break;
                        }

                        else if (match_word("else"))
                                add_token(ELSE);
                        else if (match_word("false"))
                                add_token(FALSE);
                        else if (match_word("function"))
                                add_token(FUNCTION);
                        else if (match_word("for"))
                                add_token(FOR);
                        else if (match_word("if"))
                                add_token(IF);
                        else if (match_word("nil"))
                                add_token(NIL);
                        else if (match_word("extern"))
                                add_token(EXTERN);
                        else if (match_word("return"))
                                add_token(RETURN);
                        else if (match_word("true"))
                                add_token(TRUE);
                        else if (match_word("while"))
                                add_token(WHILE);
                        else
                                add_token(IDENTIFIER, get_identifier());
                        // clang-format on

                        break;
                }

                if (current == EOF || current == 0) break;
        }
}

int
main(int argc, char **argv)
{
        if (argc != 2) return -1;

        char buf[1024 * 1024];
        int fd = open(argv[1], O_RDONLY);
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
        return 0;
}
