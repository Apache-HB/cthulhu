// SPDX-License-Identifier: GPL-3.0-only

#include "cfamily-target/target.h"

#include "core/macros.h"
#include "driver/driver.h"

static void cfamily_create(target_runtime_t *runtime)
{
    // empty
    CT_UNUSED(runtime);
}

static void cfamily_destroy(target_runtime_t *runtime)
{
    // empty
    CT_UNUSED(runtime);
}

CT_DRIVER_API const target_t kTargetC = {
    .info = {
        .id = "target/cfamily",
        .name = "C",
        .version = {
            .license = "LGPLv3",
            .author = "Elliot Haisley",
            .desc = "C89 output target",
            .version = CT_NEW_VERSION(0, 0, 1)
        }
    },

    .fn_create = cfamily_create,
    .fn_destroy = cfamily_destroy,

    .fn_tree = cfamily_tree,
    .fn_ssa = cfamily_ssa
};

CT_TARGET_EXPORT(kTargetC)
