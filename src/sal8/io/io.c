#include "io.h"
#include "../mem/mem.h"

#include <stdio.h>

uint16_t SAL8_IO_STDIN() {
    uint8_t in;

    if(fscanf(stdin, "%hhu", &in) < 1)
        return UINT16_MAX;
    return in;
}

void SAL8_IO_STDOUT(const char* data) {
    fprintf(stdout, "%s", data);
}

void SAL8_IO_STDERR(const char* data) {
    fprintf(stderr, "%s", data);
}

void sal8_io_init(SAL8IO* io) {
    io->in = &SAL8_IO_STDIN;
    io->out = &SAL8_IO_STDOUT;
    io->err = &SAL8_IO_STDERR;
}

uint16_t sal8_io_in(SAL8IO* io) {
    return io->in();
}

void sal8_io_out(SAL8IO* io, const char* fmt, ...) {
    char* data;
    va_list args;

    va_start(args, fmt);
    data = sal8_io_format(fmt, args);
    va_end(args);

    io->out(data);
    SAL8_MEM_FREE(data);
}

void sal8_io_err(SAL8IO* io, const char* fmt, ...) {
    char* data;
    va_list args;

    va_start(args, fmt);
    data = sal8_io_format(fmt, args);
    va_end(args);

    io->err(data);
    SAL8_MEM_FREE(data);
}

void sal8_io_redirect_in(SAL8IO* io, SAL8InHandler handler) {
    io->in = handler;
}

void sal8_io_redirect_out(SAL8IO* io, SAL8OutHandler handler) {
    io->out = handler;
}

void sal8_io_redirect_err(SAL8IO* io, SAL8OutHandler handler) {
    io->err = handler;
}

void sal8_io_clone(SAL8IO* dest, SAL8IO* src) {
    dest->in = src->in;
    dest->out = src->out;
    dest->err = src->err;
}

char* sal8_io_format(const char* fmt, va_list args) {
    char* data;

    va_list argscpy;
    va_copy(argscpy, args);

    data = SAL8_MEM_ALLOC(1 + vsnprintf(NULL, 0, fmt, argscpy));

    vsprintf(data, fmt, args);
    va_end(argscpy);

    return data;
}