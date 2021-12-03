#pragma once

typedef struct InputStream InputStream;

InputStream *inputstream_create(char *(*gather_func)());

void inputstream_free(InputStream *inputstream);

char inputstream_read(InputStream *inputstream);
