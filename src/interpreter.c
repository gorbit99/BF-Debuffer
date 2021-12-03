#include "interpreter.h"

#include "inputstream.h"

#include <bits/stdint-uintn.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const long TAPE_SIZE = 30000;

struct BrainfuckInterpreter {
    char *code;
    char *pc;
    uint8_t *tape;
    uint8_t *tapePtr;
    InputStream *inputstream;
};

BrainfuckInterpreter *interpreter_create(char *filename,
                                         InputStream *inputstream) {
    FILE *file = fopen(filename, "rt");

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *code = (char *)malloc(sizeof(char) * (file_size + 1));
    fread(code, sizeof(char), file_size, file);
    code[file_size] = '\0';

    uint8_t *tape = (uint8_t *)calloc(TAPE_SIZE, sizeof(uint8_t));
    BrainfuckInterpreter *interpreter =
            (BrainfuckInterpreter *)malloc(sizeof(BrainfuckInterpreter));
    interpreter->code = code;
    interpreter->pc = code;
    interpreter->tape = tape;
    interpreter->tapePtr = tape;
    interpreter->inputstream = inputstream;

    fclose(file);
    return interpreter;
}

void interpreter_free(BrainfuckInterpreter *interpreter) {
    free(interpreter->code);
    free(interpreter->tape);
    free(interpreter);
}

void interpreter_step(BrainfuckInterpreter *interpreter) {
    char cmd = *interpreter->pc;

    if (cmd == '\0') {
        return;
    }

    switch (cmd) {
    case '+':
        (*interpreter->tapePtr)++;
        break;
    case '-':
        (*interpreter->tapePtr)--;
        break;
    case '>':
        interpreter->tapePtr++;
        if (interpreter->tapePtr - interpreter->tape >= TAPE_SIZE) {
            interpreter->tapePtr = interpreter->tape;
        }
        break;
    case '<':
        interpreter->tapePtr--;
        if (interpreter->tapePtr < interpreter->tape) {
            interpreter->tapePtr = interpreter->tape + TAPE_SIZE - 1;
        }
        break;
    case '.':
        putchar(*(interpreter->tapePtr));
        break;
    case ',':
        *interpreter->tapePtr = inputstream_read(interpreter->inputstream);
        break;
    case '[':
        if (*interpreter->tapePtr == 0) {
            size_t parens = 1;
            while (parens > 0) {
                interpreter->pc++;
                if (*interpreter->pc == '[') {
                    parens++;
                } else if (*interpreter->pc == ']') {
                    parens--;
                }
            }
        }
        break;
    case ']': {
        size_t parens = 1;
        while (parens > 0) {
            interpreter->pc--;
            if (*interpreter->pc == '[') {
                parens--;
            } else if (*interpreter->pc == ']') {
                parens++;
            }
        }
        interpreter->pc--;
        break;
    }
    }

    interpreter->pc++;
}

void interpreter_run(BrainfuckInterpreter *interpreter) {
    interpreter_step(interpreter);
    while (*interpreter->pc != '\0' && *interpreter->pc != '#') {
        interpreter_step(interpreter);
    }
}
