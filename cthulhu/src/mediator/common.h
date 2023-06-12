#pragma once

#include "cthulhu/mediator/common.h"

#include "std/map.h"

typedef struct mediator_t
{
    const char *id;
    version_info_t version;
} mediator_t;

typedef struct lifetime_t 
{
    mediator_t *parent;

    map_t *extensions;

    map_t *modules;
} lifetime_t;

typedef struct context_t 
{
    lifetime_t *parent;

    void *ast;
    hlir_t *root;
} context_t;
