#pragma once

#include "inputstream.h"

#include <stdbool.h>
typedef struct BrainfuckInterpreter BrainfuckInterpreter;

BrainfuckInterpreter *interpreter_create(char *filename,
                                         InputStream *inputstream);

void interpreter_free(BrainfuckInterpreter *interpreter);

void interpreter_step(BrainfuckInterpreter *interpreter);

void interpreter_run(BrainfuckInterpreter *interpreter, bool ignore_breaks);

InputStream *interpreter_get_inputstream(BrainfuckInterpreter *interpreter);

void interpreter_print_code(BrainfuckInterpreter *interpreter);

void interpreter_print_context(BrainfuckInterpreter *interpreter);

void interpreter_print_output(BrainfuckInterpreter *interpreter);

void interpreter_set_optimized(BrainfuckInterpreter *interpreter, bool value);

void interpreter_exec_char(BrainfuckInterpreter *interpreter, char cmd);

void interpreter_read_code(BrainfuckInterpreter *interpreter);

void interpreter_set_eof_mode(BrainfuckInterpreter *interpreter, EOFMode mode);

bool interpreter_write_code(BrainfuckInterpreter *interpreter, char *filename);
