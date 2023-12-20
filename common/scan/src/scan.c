#include "base/log.h"
#include "base/panic.h"

#include "core/text.h"
#include "io/io.h"
#include "memory/arena.h"

#include <string.h>

typedef struct scan_t
{
    io_t *io;           ///< file itself
    arena_t *alloc;     ///< allocator to use everything involving this file

    const char *language; ///< the language this file contains
    const char *path;     ///< the path to this file
    void *tree;           ///< tree generated by the parser
    void *context;        ///< user context

    const char *mapped;
    size_t size;
} scan_t;

static scan_t gBuiltinScan = {
    .io = NULL,
    .alloc = NULL,

    .language = "builtin",
    .path = "builtin",
    .tree = NULL,

    .mapped = "",
    .size = 0
};

USE_DECL
scan_t *scan_builtin(void)
{
    return &gBuiltinScan;
}

USE_DECL
bool scan_is_builtin(const scan_t *scan)
{
    return scan == scan_builtin();
}

USE_DECL
const char *scan_language(const scan_t *scan)
{
    CTASSERT(scan != NULL);

    return scan->language;
}

USE_DECL
const char *scan_path(const scan_t *scan)
{
    CTASSERT(scan != NULL);

    return scan->path;
}

USE_DECL
void *scan_get(scan_t *scan)
{
    CTASSERT(scan != NULL);

    ctu_log("scan get %p", scan->tree);
    return scan->tree;
}

USE_DECL
void scan_set(scan_t *scan, void *value)
{
    CTASSERT(scan != NULL);

    ctu_log("scan set %p", value);
    scan->tree = value;
}

USE_DECL
void scan_set_context(scan_t *scan, void *value)
{
    CTASSERT(scan != NULL);

    scan->context = value;
}

USE_DECL
void *scan_get_context(const scan_t *scan)
{
    CTASSERT(scan != NULL);

    return scan->context;
}

USE_DECL
const char *scan_text(const scan_t *scan)
{
    CTASSERT(scan != NULL);

    return scan->mapped;
}

USE_DECL
text_view_t scan_source(const scan_t *scan)
{
    CTASSERT(scan != NULL);

    text_view_t text = {
        .text = scan->mapped,
        .size = scan->size
    };

    return text;
}

USE_DECL
size_t scan_size(const scan_t *scan)
{
    CTASSERT(scan != NULL);

    return scan->size;
}

USE_DECL
size_t scan_read(scan_t *scan, void *dst, size_t size)
{
    CTASSERT(scan != NULL);
    CTASSERT(dst != NULL);

    return io_read(scan->io, dst, size);
}

USE_DECL
arena_t *scan_alloc(const scan_t *scan)
{
    CTASSERT(scan != NULL);

    return scan->alloc;
}

USE_DECL
io_t *scan_src(scan_t *scan)
{
    CTASSERT(scan != NULL);

    return scan->io;
}

USE_DECL
scan_t *scan_io(const char *language, io_t *io, arena_t *alloc)
{
    CTASSERT(language != NULL);
    CTASSERT(io != NULL);
    CTASSERTF(io_error(io) == 0, "io-error(%s) = %zu", io_name(io), io_error(io));
    CTASSERT(alloc != NULL);

    scan_t *self = ARENA_MALLOC(alloc, sizeof(scan_t), io_name(io), io);

    self->language = language;
    self->io = io;
    self->path = io_name(io);
    self->alloc = alloc;

    self->tree = NULL;
    self->context = NULL;

    self->mapped = io_map(io);
    self->size = io_size(io);

    return self;
}
