#pragma once

typedef enum EOFMode {
    EOF_Zero,
    EOF_Negative,
    EOF_Newline,
    EOF_Unchanged,
} EOFMode;

typedef struct InputStream InputStream;

InputStream *inputstream_create(char *(*gather_func)(void *data), void *data);

void inputstream_free(InputStream *inputstream);

char inputstream_read(InputStream *inputstream, EOFMode mode);

void *inputstream_get_data(InputStream *inputstream);
