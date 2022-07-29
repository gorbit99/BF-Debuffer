#include "outputbuffer.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct OutputBuffer {
    char *output;
    size_t size;
    size_t capacity;
};

OutputBuffer *outputbuffer_create() {
    OutputBuffer *outputbuffer = (OutputBuffer *)malloc(sizeof(OutputBuffer));

    const size_t start_size = 5;

    outputbuffer->output = (char *)malloc(sizeof(char) * (start_size + 1));
    outputbuffer->size = 0;
    outputbuffer->capacity = start_size;
    return outputbuffer;
}

void outputbuffer_free(OutputBuffer *outputbuffer) {
    free(outputbuffer->output);
    free(outputbuffer);
}

void outputbuffer_print(OutputBuffer *outputbuffer) {
    fwrite(outputbuffer->output, sizeof(char), outputbuffer->size, stdout);
}

void outputbuffer_add(OutputBuffer *outputbuffer, char c) {
    if (outputbuffer->size == outputbuffer->capacity) {
        outputbuffer->capacity *= 2;
        outputbuffer->output =
                (char *)realloc(outputbuffer->output,
                                sizeof(char) * (outputbuffer->capacity));
    }
    outputbuffer->output[outputbuffer->size] = c;
    outputbuffer->size++;
}

void outputbuffer_clear(OutputBuffer *outputbuffer) {
    outputbuffer->size = 0;
}
