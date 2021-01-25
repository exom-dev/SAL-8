#ifndef SAL8_IO_H_GUARD
#define SAL8_IO_H_GUARD

#include <stdint.h>
#include <stdarg.h>

typedef void (*SAL8OutHandler)(const char*);
typedef uint16_t (*SAL8InHandler)();

typedef struct {
    SAL8InHandler  in;
    SAL8OutHandler out;
    SAL8OutHandler err;
} SAL8IO;

uint16_t SAL8_IO_STDIN();
void     SAL8_IO_STDOUT(const char* data);
void     SAL8_IO_STDERR(const char* data);

void sal8_io_init(SAL8IO* io);

uint16_t sal8_io_in(SAL8IO* io);
void     sal8_io_out(SAL8IO* io, const char* fmt, ...);
void     sal8_io_err(SAL8IO* io, const char* fmt, ...);

void sal8_io_redirect_in(SAL8IO* io, SAL8InHandler handler);
void sal8_io_redirect_out(SAL8IO* io, SAL8OutHandler handler);
void sal8_io_redirect_err(SAL8IO* io, SAL8OutHandler handler);

void sal8_io_clone(SAL8IO* dest, SAL8IO* src);

char* sal8_io_format(const char* fmt, va_list list);

#endif
