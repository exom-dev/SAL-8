#ifndef SAL8_LOG_H_GUARD
#define SAL8_LOG_H_GUARD

#include "io.h"

#include <stdio.h>
#include <stdlib.h>

#define SAL8_IN(io) \
    sal8_io_in(&io)

#define SAL8_OUT(io, fmt, ...) \
    sal8_io_out(&io, fmt, ##__VA_ARGS__)

#define SAL8_ERR(io, fmt, ...) \
    sal8_io_err(&io, fmt, ##__VA_ARGS__)

#define SAL8_WARNING(io, fmt, ...) \
    SAL8_OUT(io, "[warning] " fmt "\n", ##__VA_ARGS__)

#define SAL8_ERROR(io, fmt, ...) \
    SAL8_ERR(io, "[error] " fmt "\n", ##__VA_ARGS__)

#ifdef DEBUG_TRACE_MEMORY_OPS
    #define SAL8_MEMORY(fmt, ...) \
        fprintf(stdout, "[memory] " fmt "\n", ##__VA_ARGS__)
#else
    #define SAL8_MEMORY(fmt, ...)
#endif

#endif
