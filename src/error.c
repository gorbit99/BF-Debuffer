#include "error.h"

#include <stdarg.h>
#include <stdio.h>

int __eprintf_base(char *file, long line, char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "Error at %s:%ld!\n", file, line);
    int ret = vfprintf(stderr, format, args);
    va_end(args);

    return ret;
}
