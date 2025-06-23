#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "env.h"
#include "interpreter.h"
#include "tokens.h"

#define PROMPT "[vispel] >> "

void
prompt()
{
        printf("%s", PROMPT);
        fflush(stdout);
}

int
main(int argc, char **argv)
{
        char buf[1024 * 1024];
        ssize_t n;
        int fd;
        int interactive = 0;

        if (argc == 2)
                fd = open(argv[1], O_RDONLY);
        else {
                fd = STDIN_FILENO;
                interactive = 1;
        }

        if (fd < 0) {
                report("Can not open to read file `%s`\n", argv[1]);
                return -1;
        }

        env_create();
        if (interactive) prompt();
        while ((n = read(fd, buf, sizeof buf - 2)) > 0) {
                buf[n] = 0;
                buf[n + 1] = EOF;
                lex_analize(buf);
                // print_tokens();
                tok_parse();
                print_ast();
                eval();
                if (interactive) prompt();
        }
        env_destroy();
        if (n < 0) {
                report("Can not read\n");
                return -1;
        }

        return 0;
}
