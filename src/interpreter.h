#pragma once

#include "inputstream.h"
typedef struct BrainfuckInterpreter BrainfuckInterpreter;

BrainfuckInterpreter *interpreter_create(char *filename,
                                         InputStream *inputstream);

void interpreter_free(BrainfuckInterpreter *interpreter);

void interpreter_step(BrainfuckInterpreter *interpreter);

void interpreter_run(BrainfuckInterpreter *interpreter);
