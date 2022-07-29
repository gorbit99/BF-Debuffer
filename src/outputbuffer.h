#pragma once

typedef struct OutputBuffer OutputBuffer;

OutputBuffer *outputbuffer_create();

void outputbuffer_free(OutputBuffer *outputbuffer);

void outputbuffer_print(OutputBuffer *outputbuffer);

void outputbuffer_add(OutputBuffer *outputbuffer, char c);

void outputbuffer_print_context(OutputBuffer *outputbuffer);

void outputbuffer_clear(OutputBuffer *outputbuffer);
