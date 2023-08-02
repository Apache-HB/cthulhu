#include "common.h"

#include "std/vector.h"
#include "std/map.h"

#include "base/memory.h"
#include "base/panic.h"

#include "report/report.h"

static h2_t *h2_module_new(const node_t *node, const char *name, h2_t *parent, reports_t *reports, size_t decls, size_t *sizes)
{
    CTASSERTF(decls >= eSema2Total, "module cannot be constructed with less than %zu tags (%zu given)", eSema2Total, decls);
    CTASSERT(reports != NULL);

    h2_t *self = h2_decl(eHlir2DeclModule, node, NULL, name);
    self->parent = parent;
    self->reports = reports;
    self->tags = vector_of(decls);

    for (size_t i = 0; i < decls; i++)
    {
        map_t *map = map_optimal(sizes[i]);
        vector_set(self->tags, i, map);
    }

    return self;
}

h2_t *h2_module_root(reports_t *reports, const node_t *node, const char *name, size_t decls, size_t *sizes)
{
    return h2_module_new(node, name, NULL, reports, decls, sizes);
}

h2_t *h2_module(h2_t *parent, const node_t *node, const char *name, size_t decls, size_t *sizes)
{
    CTASSERT(parent != NULL);

    return h2_module_new(node, name, parent, parent->reports, decls, sizes);
}

void *h2_module_get(h2_t *self, size_t tag, const char *name)
{
    CTASSERT(name != NULL);

    map_t *map = h2_module_tag(self, tag);
    h2_t *old = map_get(map, name);
    if (old != NULL)
    {
        return old;
    }

    if (self->parent != NULL)
    {
        return h2_module_get(self->parent, tag, name);
    }

    return NULL;
}

void *h2_module_set(h2_t *self, size_t tag, const char *name, void *value)
{
    void *old = h2_module_get(self, tag, name);
    if (old != NULL)
    {
        return old;
    }

    map_t *map = h2_module_tag(self, tag);
    map_set(map, name, value);

    return NULL;
}

map_t *h2_module_tag(const h2_t *self, size_t tag)
{
    CTASSERT(self != NULL);

    return vector_get(self->tags, tag);
}

void h2_module_update(h2_t *self, void *data)
{
    CTASSERT(self != NULL);

    self->data = data;
}

void *h2_module_data(h2_t *self)
{
    CTASSERT(self != NULL);

    return self->data;
}

h2_cookie_t *h2_module_cookie(h2_t *self)
{
    CTASSERT(self != NULL);

    h2_cookie_t *cookie = ctu_malloc(sizeof(h2_cookie_t));
    cookie->parent = self;
    cookie->stack = vector_new(16);
    return cookie;
}
