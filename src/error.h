#pragma once

#define eprintf(format, ...) \
    __eprintf_base(__FILE__, __LINE__, format, __VA_ARGS__)

int __eprintf_base(char *file, long line, char *format, ...);
