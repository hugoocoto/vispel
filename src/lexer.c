/* VISPEL lexer
 *
 * Author: Hugo Coto Florez
 * Repo: https://github.com/hugocotoflorez/vispel
 */

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
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
        PRINT,
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
        [PRINT] = "PRINT",
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
};

typedef struct vlex {
        vtoken token;
        char *lexeme;
        union {
                int num_literal;
                void *mem_literal;
        };
        int line;
        /* Linked list stuff */
        struct vlex *next;
} vlex;

char *current = NULL;
int line = 1;

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
        /* TODO */
        report("Token: %s", TOKEN_REPR[token]);
}

bool
match(char expected)
{
        if (*current == expected) return false;
        ++current;
        return true;
}

char inline get_consume_lex()
{
        return *current++;
}

char *
get_string()
{
        /* To test */
        char *ret = current;
        char tmp;

        while (*current++ != '"')
                ;

        tmp = *current;
        *current = 0;
        ret = strdup(ret);
        *current = tmp;
        --current;
        return ret;
}

int
get_number()
{
        int n = current[-1];
        while (*current >= '0' && *current <= '9') {
                n *= 10;
                n += *current;
                ++current;
        }
        --current;
        return n;
}

vlex
lex_analize(char *source)
{
        current = source;
        for (;;) {
                switch (get_consume_lex()) {
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
                        add_token(LEFT_PARENT);
                        break;
                case '[':
                        add_token(LEFT_BRACKET);
                        break;
                case ']':
                        add_token(LEFT_BRACKET);
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
                case '/':
                        add_token(SLASH);
                        break;
                case '*':
                        add_token(STAR);
                        break;

                case '&':
                        if (match('&'))
                                add_token(BITWISE_AND);
                        else
                                add_token(AND);
                        break;
                case '|':
                        if (match('|'))
                                add_token(BITWISE_OR);
                        else
                                add_token(OR);
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
                        if (match('='))
                                add_token(GREATER_EQUAL);
                        else
                                add_token(GREATER);
                        break;
                case '<':
                        if (match('<'))
                                add_token(SHIFT_RIGHT);
                        if (match('='))
                                add_token(LESS_EQUAL);
                        else
                                add_token(LESS);
                        break;

                case '"':
                        add_token(STRING, get_string());
                        break;

                case '0' ... '9':
                        add_token(NUMBER, get_number());
                        break;

                case '\n':
                        ++line;
                        break;

                default:
                        report("[line %d] Invalid lexeme found: `%c`\n", line, current[-1]);
                }


                // IDENTIFIER,
                // ELSE,
                // FALSE,
                // FUNCTION,
                // FOR,
                // IF,
                // NIL,
                // PRINT,
                // RETURN,
                // TRUE,
                // WHILE,
                // END_OF_FILE,
        }
}

int
main(int argc, char **argv)
{
        if (argc != 1) return -1;

        char buf[1024 * 1024];
        int fd = open(argv[1], O_RDONLY);
        if (fd < 0)  return -1;

        ssize_t n = read(fd, buf, sizeof buf - 1);
        if (n <= 0) return -1;
        buf[n] = 0;
        lex_analize(buf);
        return 0;
}
