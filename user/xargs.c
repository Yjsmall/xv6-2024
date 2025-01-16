#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h" // MAXARG

#define is_blank(chr) (chr == ' ' || chr == '\t')

int main(int argc, char *argv[]) {
    char  buf[512];
    char *p = buf;
    char *cmd_argv[MAXARG];
    int   arg_index;
    int   blanks = 0;
    int   offset = 0;

    if (argc <= 1) {
        fprintf(2, "usage: xargs <command> [argv...]\n");
        exit(1);
    }

    // Initialize v with command arguments
    for (arg_index = 1; arg_index < argc; arg_index++) {
        cmd_argv[arg_index - 1] = argv[arg_index];
    }
    --arg_index; // Adjust arg_index to start from the first argument

    // Read from stdin and process input
    while (read(0, &buf[offset], 1) > 0) {
        if (is_blank(buf[offset])) {
            blanks++;
            offset++;
            continue;
        }

        if (blanks) {          // If there were blanks before
            buf[offset++] = 0; // Null-terminate the current argument

            cmd_argv[arg_index++] = p;
            p                     = buf + offset; // Move to the next position

            blanks = 0; // Reset blanks counter
        }

        if (buf[offset] != '\n') {
            offset++; // Move to the next character
        } else {
            buf[offset]           = 0; // Null-terminate the current argument
            cmd_argv[arg_index++] = p;
            p                     = buf + offset; // Move to the next position

            // Execute the command with the current arguments
            if (!fork()) {
                exit(exec(cmd_argv[0], cmd_argv));
            }
            wait(0);

            arg_index = argc - 1; // Reset arg_index to start from the first argument again
            offset    = 0;        // Reset offset to start from the beginning of the buffer
            p         = buf;      // Reset p to start from the beginning of the buffer
        }
    }

    exit(0);
}
