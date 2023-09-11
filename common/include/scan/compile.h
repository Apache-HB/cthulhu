#pragma once

#include "scan/scan.h"

#include "base/analyze.h"
#include "base/panic.h"

#include <limits.h>

typedef struct io_t io_t;
typedef struct scan_t scan_t;

/**
 * scanner function callbacks for flex and bison
 */
typedef struct callbacks_t {
    int (*init)(scan_t *extra, void *scanner);                   ///< yylex_init_extra
    int (*parse)(void *scanner, scan_t *extra);                  ///< yyparse
    void *(*scan)(const char *text, size_t size, void *scanner); ///< yy_scan_string
    void (*destroyBuffer)(void *buffer, void *scanner);          ///< yy_delete_buffer
    void (*destroy)(void *scanner);                              ///< yylex_destroy
} callbacks_t;

#define CT_CALLBACKS(id, prefix)                                                                                       \
    static int prefix##_##id##_##init(scan_t *extra, void *scanner)                                                    \
    {                                                                                                                  \
        return prefix##lex_init_extra(extra, scanner);                                                                 \
    }                                                                                                                  \
    static int prefix##_##id##_parse(void *scanner, scan_t *extra)                                                     \
    {                                                                                                                  \
        return prefix##parse(scanner, extra);                                                                          \
    }                                                                                                                  \
    static void *prefix##_##id##_scan(const char *text, size_t size, void *scanner)                                    \
    {                                                                                                                  \
        CTASSERTF(size <= INT_MAX, #prefix "-scan (size = %zu > %d, name = %s)", size, INT_MAX, scan_path(scanner));                                                                                     \
        return prefix##_scan_bytes(text, (int)size, scanner);                                                          \
    }                                                                                                                  \
    static void prefix##_##id##_destroyBuffer(void *buffer, void *scanner)                                                    \
    {                                                                                                                  \
        prefix##_delete_buffer(buffer, scanner);                                                                       \
    }                                                                                                                  \
    static void prefix##_##id##_destroy(void *scanner)                                                                 \
    {                                                                                                                  \
        prefix##lex_destroy(scanner);                                                                                  \
    }                                                                                                                  \
    static callbacks_t id = {                                                                                          \
        .init = prefix##_##id##_##init,                                                                                \
        .parse = prefix##_##id##_parse,                                                                                \
        .scan = prefix##_##id##_scan,                                                                                  \
        .destroyBuffer = prefix##_##id##_destroyBuffer,                                                                       \
        .destroy = prefix##_##id##_destroy,                                                                            \
    }

/**
 * @brief compile a string into a language specific ast
 *
 * @param extra the sanner being used
 * @param callbacks the flex/bison callbacks
 * @return void* a pointer to the compiled ast
 */
void *compile_scanner(scan_t *extra, callbacks_t *callbacks);
