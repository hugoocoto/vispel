#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

void static report(char *format, ...)
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
} vtoktype;

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

typedef struct vtok {
        vtoktype token;
        const char *lexeme;
        union {
                int num_literal;
                void *mem_literal;
                char *str_literal;
        };
        int line;
        intptr_t offset;
        /* Linked list stuff */
        struct vtok *next;
        struct vtok *prev;
} vtok;

extern vtok *head_token;

/* Get source code as a string and return a linked list of tokens */
void lex_analize(char *source);

/* Print the list of tokens */
void print_tokens();
void print_token(vtok *t);
void print_literal(vtok *tok);
