#include "base/panic.h"

#include "stacktrace/stacktrace.h"

#include <stdio.h>
#include <stdlib.h>

#if ADDRSAN_ENABLED
#    include <signal.h>
#endif

#define STACK_FRAMES 128

static void default_panic_handler(panic_t panic, const char *fmt, va_list args)
{
    fprintf(stderr, COLOUR_CYAN "[panic]" COLOUR_RESET "[%s:%zu] => " COLOUR_RED "%s" COLOUR_RESET ": ", panic.file,
            panic.line, panic.function);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    stacktrace_print(stderr);
    __debugbreak();
}

panic_handler_t globalPanicHandler = default_panic_handler;

USE_DECL
void ctpanic(panic_t panic, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    globalPanicHandler(panic, msg, args);
    va_end(args);

#if ADDRSAN_ENABLED
    raise(SIGSEGV);
#endif

    abort();
}
