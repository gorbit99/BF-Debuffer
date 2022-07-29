#include "interpreter.h"

#include "inputstream.h"
#include "outputbuffer.h"

#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const long TAPE_SIZE = 1000000;

struct BrainfuckInterpreter {
    char *code;
    char *pc;
    uint8_t *tape;
    uint8_t *tapePtr;
    InputStream *inputstream;
    OutputBuffer *outputbuffer;
    bool optimized;
    char *fileName;
    EOFMode eof_mode;
};

BrainfuckInterpreter *interpreter_create(char *filename,
                                         InputStream *inputstream) {
    uint8_t *tape = (uint8_t *)calloc(TAPE_SIZE, sizeof(uint8_t));
    BrainfuckInterpreter *interpreter =
            (BrainfuckInterpreter *)malloc(sizeof(BrainfuckInterpreter));
    interpreter->tape = tape;
    interpreter->tapePtr = tape;

    interpreter->inputstream = inputstream;
    interpreter->outputbuffer = outputbuffer_create();

    interpreter->optimized = false;
    interpreter->eof_mode = EOF_Negative;

    interpreter->fileName =
            (char *)malloc(sizeof(char) * (strlen(filename) + 1));
    strcpy(interpreter->fileName, filename);

    interpreter->code = NULL;

    interpreter_read_code(interpreter);

    return interpreter;
}

void interpreter_free(BrainfuckInterpreter *interpreter) {
    outputbuffer_free(interpreter->outputbuffer);
    inputstream_free(interpreter->inputstream);
    free(interpreter->code);
    free(interpreter->tape);
    free(interpreter->fileName);
    free(interpreter);
}

void interpreter_step(BrainfuckInterpreter *interpreter) {
    char cmd = *interpreter->pc;

    if (cmd == '\0') {
        return;
    }

    do {
        interpreter_exec_char(interpreter, cmd);

        interpreter->pc++;
    } while (interpreter->optimized && *interpreter->pc == cmd
             && strchr("+-<>", cmd));

    while (!strchr("+-<>[].,#", *interpreter->pc) && *interpreter->pc != '\0') {
        interpreter->pc++;
    }
}

void interpreter_run(BrainfuckInterpreter *interpreter, bool ignore_breaks) {
    interpreter_step(interpreter);
    while (*interpreter->pc != '\0'
           && (ignore_breaks || *interpreter->pc != '#')) {
        interpreter_step(interpreter);
    }
}

InputStream *interpreter_get_inputstream(BrainfuckInterpreter *interpreter) {
    return interpreter->inputstream;
}

void interpreter_print_code(BrainfuckInterpreter *interpreter) {
    size_t lineNo = 1;
    size_t code_pos = interpreter->pc - interpreter->code;
    for (size_t i = 0; i < code_pos; ++i) {
        if (interpreter->code[i] == '\n') {
            lineNo++;
        }
    }

    const size_t context_lines = 5;

    size_t pos = code_pos;
    for (size_t lines = context_lines + 1; lines > 0 && pos > 0; pos--) {
        if (interpreter->code[pos] == '\n') {
            lines--;
            lineNo--;
        }
    }
    if (interpreter->code[pos] == '\n') {
        pos++;
    }

    printf("%4zu ", lineNo);
    while (pos != code_pos) {
        if (interpreter->code[pos] == '\n') {
            lineNo++;
            printf("\n%4zu ", lineNo);
        } else {
            printf("%c", interpreter->code[pos]);
        }
        pos++;
    }

    if (interpreter->code[code_pos] != '\0') {
        printf("\x1b[38;2;255;255;255m");
        printf("\x1b[48;2;0;0;255m");
        printf("%c", interpreter->code[code_pos]);
        printf("\x1b[m");
    }

    pos++;
    for (size_t lines = 0;
         lines < context_lines + 1 && interpreter->code[pos] != '\0';
         pos++) {
        if (interpreter->code[pos] == '\n') {
            lines++;
            lineNo++;
            if (lines != context_lines + 1) {
                printf("\n%4zu ", lineNo);
            }
        } else {
            printf("%c", interpreter->code[pos]);
        }
    }
}

void interpreter_print_context(BrainfuckInterpreter *interpreter) {
    size_t pos = interpreter->tapePtr - interpreter->tape;

    const size_t context_size = 15;

    size_t min = pos < context_size ? 0 : pos - context_size;
    size_t max = TAPE_SIZE - pos + 1 < context_size ? (size_t)TAPE_SIZE - 1
                                                    : pos + context_size;

    if (min == 0) {
        max = min + 2 * context_size;
    }
    if (max == (size_t)TAPE_SIZE - 1) {
        min = (size_t)TAPE_SIZE - 1 - 2 * context_size;
    }

    for (size_t i = 0; i < (pos - min) * 4; i++) {
        printf(" ");
    }
    printf("  ▽\n");

    printf("┌───");
    for (size_t i = min + 1; i <= max; i++) {
        printf("┬───");
    }
    printf("┐\n");

    for (size_t i = min; i <= max; i++) {
        printf("│%3d", interpreter->tape[i]);
    }
    printf("│\n");

    for (size_t i = min; i <= max; i++) {
        if (isgraph(interpreter->tape[i])) {
            printf("│%3c", interpreter->tape[i]);
        } else {
            printf("│   ");
        }
    }
    printf("│\n");

    printf("└───");
    for (size_t i = min + 1; i <= max; i++) {
        printf("┴───");
    }
    printf("┘\n");
}

void interpreter_print_output(BrainfuckInterpreter *interpreter) {
    outputbuffer_print(interpreter->outputbuffer);
}

void interpreter_set_optimized(BrainfuckInterpreter *interpreter, bool value) {
    interpreter->optimized = value;
}

void interpreter_exec_char(BrainfuckInterpreter *interpreter, char cmd) {
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
        outputbuffer_add(interpreter->outputbuffer, *interpreter->tapePtr);
        break;
    case ',': {
        char val = inputstream_read(interpreter->inputstream,
                                    interpreter->eof_mode);
        if (val != '\0' || interpreter->eof_mode != EOF_Unchanged) {
            *interpreter->tapePtr = val;
        }
        break;
    }
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
}

static size_t interpreter_handle_imports(char **code) {
    char *found;
    size_t code_length = strlen(*code);
    while ((found = strchr(*code, '@')) != NULL) {
        size_t offset = found - *code;
        size_t filename_length = strcspn(found, "\n");
        char *filename = (char *)malloc(sizeof(char) * filename_length);
        strncpy(filename, found + 1, filename_length);
        filename[filename_length - 1] = '\0';

        FILE *file = fopen(filename, "r");
        fseek(file, 0, SEEK_END);
        size_t file_length = ftell(file);
        rewind(file);
        char *data = (char *)malloc(sizeof(char) * (file_length + 1));
        fread(data, sizeof(char), file_length, file);
        if (data[file_length - 2] == '\n') {
            file_length--;
        }
        data[file_length - 1] = '\0';

        size_t new_size = interpreter_handle_imports(&data);
        *code = (char *)realloc(
                *code,
                sizeof(char) * (code_length - filename_length + new_size + 1));

        memmove(*code + offset + new_size,
                *code + offset + filename_length,
                code_length - offset - filename_length);
        memcpy(*code + offset, data, new_size);
        code_length = code_length - filename_length + new_size;
        (*code)[code_length] = '\0';
        free(data);
        free(filename);
        fclose(file);
    }
    return code_length;
}

void interpreter_read_code(BrainfuckInterpreter *interpreter) {
    free(interpreter->code);
    FILE *file = fopen(interpreter->fileName, "rt");

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *code = (char *)malloc(sizeof(char) * (file_size + 1));
    fread(code, sizeof(char), file_size, file);
    code[file_size] = '\0';

    fclose(file);

    interpreter_handle_imports(&code);

    interpreter->code = code;
    interpreter->pc = code;

    while (!strchr("+-<>[].,#", *interpreter->pc) && *interpreter->pc != '\0') {
        interpreter->pc++;
    }

    memset(interpreter->tape, 0, TAPE_SIZE * sizeof(char));
    interpreter->tapePtr = interpreter->tape;

    outputbuffer_clear(interpreter->outputbuffer);
}

void interpreter_set_eof_mode(BrainfuckInterpreter *interpreter, EOFMode mode) {
    interpreter->eof_mode = mode;
}

bool interpreter_write_code(BrainfuckInterpreter *interpreter, char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return false;
    }

    fwrite(interpreter->code, sizeof(char), strlen(interpreter->code), file);

    return true;
}
