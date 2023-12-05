#include "io/impl.h"

#include "core/macros.h"
#include "memory/memory.h"
#include "base/panic.h"

#include <string.h>

typedef struct buffer_t {
    char *data;   ///< stored data
    size_t used;  ///< used data
    size_t total; ///< total size of data

    size_t offset; ///< current offset in data
} buffer_t;

static buffer_t *mem_data(io_t *self)
{
    return io_data(self);
}

static size_t mem_read(io_t *self, void *dst, size_t size)
{
    buffer_t *mem = mem_data(self);
    size_t len = MIN(size, mem->used - mem->offset);
    memcpy(dst, mem->data + mem->offset, len);
    mem->offset += len;
    return len;
}

static size_t mem_write(io_t *self, const void *src, size_t size)
{
    buffer_t *mem = mem_data(self);
    mem->used = MAX(mem->used, mem->offset + size);
    if (mem->offset + size > mem->total)
    {
        mem->data = ctu_realloc(mem->data, mem->offset + size);
        mem->total = mem->offset + size;
    }

    memcpy(mem->data + mem->offset, src, size);
    mem->offset += size;

    return size;
}

static size_t mem_size(io_t *self)
{
    buffer_t *mem = mem_data(self);
    return mem->used;
}

static size_t mem_seek(io_t *self, size_t offset)
{
    buffer_t *mem = mem_data(self);
    mem->offset = MIN(offset, mem->used);
    return mem->offset;
}

static const void *mem_map(io_t *self)
{
    buffer_t *mem = mem_data(self);

    void *it = ctu_malloc(mem->used);
    memcpy(it, mem->data, mem->used);

    return it;
}

static void mem_close(io_t *self)
{
    buffer_t *mem = mem_data(self);
    ctu_free(mem->data);
}

static const io_callbacks_t kBufferCallbacks = {
    .fn_read = mem_read,
    .fn_write = mem_write,

    .fn_get_size = mem_size,
    .fn_seek = mem_seek,

    .fn_map = mem_map,
    .fn_close = mem_close
};

USE_DECL
io_t *io_memory(const char *name, const void *data, size_t size, os_access_t flags)
{
    CTASSERT(data != NULL);

    buffer_t buffer = {.data = ctu_malloc(size), .total = size, .used = size, .offset = 0};

    memcpy(buffer.data, data, size);

    return io_new(&kBufferCallbacks, flags, name, &buffer, sizeof(buffer_t));
}

USE_DECL
io_t *io_blob(const char *name, size_t size, os_access_t flags)
{
    buffer_t buffer = {
        .data = ctu_malloc(size),
        .total = size,
        .used = 0,
        .offset = 0
    };

    return io_new(&kBufferCallbacks, flags, name, &buffer, sizeof(buffer_t));
}
