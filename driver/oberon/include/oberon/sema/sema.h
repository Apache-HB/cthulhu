#pragma once

#include "oberon/ast.h"

#include "cthulhu/tree/query.h"

typedef struct lifetime_t lifetime_t;

typedef enum obr_tag_t {
    eObrTagValues = eSema2Values,
    eObrTagTypes = eSema2Types,
    eObrTagProcs = eSema2Procs,
    eObrTagModules = eSema2Modules,

    eObrTagTotal
} obr_tag_t;

/// getters

tree_t *obr_get_type(tree_t *sema, const char *name);
tree_t *obr_get_module(tree_t *sema, const char *name);
tree_t *obr_get_symbol(tree_t *sema, obr_tag_t tag, const char *name);

/// add decls

void obr_add_decl(tree_t *sema, obr_tag_t tag, const char *name, tree_t *decl);

/// builtin types

tree_t *obr_get_digit_type(digit_t digit, sign_t sign);
tree_t *obr_get_bool_type(void);
tree_t *obr_get_void_type(void);

/// extra

obr_t *obr_current_decl(tree_t *sema);
void obr_set_current_decl(tree_t *sema, obr_t *decl);

const char *obr_current_name(tree_t *sema);

/// runtime module

tree_t *obr_rt_mod(lifetime_t *lifetime);
vector_t *obr_rt_path(void);
