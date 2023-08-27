#pragma once

#include "cthulhu/tree/tree.h"

void *tree_get_extra(tree_t *sema, const void *key);
void tree_set_extra(tree_t *sema, const void *key, void *data);
