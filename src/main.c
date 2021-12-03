#include "./inputstream.h"
#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_from_stdin() {
    char *input = (char *)malloc(sizeof(char) * 500);
    input[0] = '\0';
    if (scanf("%[^\n]%*c", input) > 0) {
        strcat(input, "\n");
    } else if (scanf("%*c") != EOF) {
        strcat(input, "\n");
    }
    return input;
}

int main(int argc, char **argv) {
    InputStream *inputstream = inputstream_create(read_from_stdin);

    char *filename;
    if (argc >= 2) {
        filename = argv[1];
    } else {
        filename = "test.bf";
    }

    BrainfuckInterpreter *interpreter =
            interpreter_create(filename, inputstream);
    interpreter_run(interpreter);
    interpreter_free(interpreter);
    inputstream_free(inputstream);

    return 0;
}
