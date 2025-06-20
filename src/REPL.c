#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

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

        if (interactive) prompt();
        while ((n = read(fd, buf, sizeof buf - 1)) > 0) {
                buf[n] = EOF;
                lex_analize(buf);
                // print_tokens();
                tok_parse();
                print_ast();
                eval();
                if (interactive) prompt();
        }
        if (n < 0) {
                report("Can not read\n");
                return -1;
        }

        return 0;
}
