#include "base/util.h"
#include "cthulhu/broker/broker.h"

#include "oberon/driver.h"

#include "interop/compile.h"

#include "core/macros.h"

#include "driver/driver.h"

#include "oberon/sema/sema.h"
#include "obr_bison.h" // IWYU pragma: keep
#include "obr_flex.h" // IWYU pragma: keep

CTU_CALLBACKS(kCallbacks, obr);

static void obr_preparse(language_runtime_t *runtime, void *context)
{
    obr_scan_t *ctx = context;
    ctx->logger = runtime->logger;
}

static void obr_postparse(language_runtime_t *runtime, scan_t *scan, void *tree)
{
    vector_t *modules = tree;

    size_t len = vector_len(modules);
    for (size_t i = 0; i < len; i++)
    {
        obr_t *mod = vector_get(modules, i);

        size_t decl_count = vector_len(mod->decls);
        size_t sizes[eObrTagTotal] = {
            [eObrTagValues] = decl_count,
            [eObrTagTypes] = decl_count,
            [eObrTagProcs] = decl_count,
            [eObrTagModules] = 32,
        };

        lang_add_unit(runtime, text_view_from(mod->name), mod->node, mod, sizes, eObrTagTotal);
    }
}

static void obr_destroy(language_runtime_t *handle) { CT_UNUSED(handle); }

#define NEW_EVENT(id, ...) const diagnostic_t kEvent_##id = __VA_ARGS__;
#include "oberon/events.def"

static const diagnostic_t *const kDiagnosticTable[] = {
#define NEW_EVENT(id, ...) &kEvent_##id,
#include "oberon/events.def"
};

static const char *const kLangNames[] = { "m", "mod", "obr", "oberon", NULL };

static const size_t kDeclSizes[eObrTagTotal] = {
    [eObrTagValues] = 32,
    [eObrTagTypes] = 32,
    [eObrTagProcs] = 32,
    [eObrTagModules] = 32,
};

CT_DRIVER_API const language_t kOberonModule = {
    .info = {
        .id = "obr",
        .name = "Oberon-2",
        .version = {
            .license = "LGPLv3",
            .desc = "Oberon-2 language frontend",
            .author = "Elliot Haisley",
            .version = CT_NEW_VERSION(1, 0, 0)
        },

        .diagnostics = {
            .diagnostics = kDiagnosticTable,
            .count = sizeof(kDiagnosticTable) / sizeof(const diagnostic_t *)
        },
    },

    .builtin = {
        .name = CT_TEXT_VIEW("obr\0lang"),
        .decls = kDeclSizes,
        .length = eObrTagTotal,
    },

    .exts = kLangNames,

    .context_size = sizeof(obr_scan_t),

    .fn_create = obr_create,
    .fn_destroy = obr_destroy,

    .fn_preparse = obr_preparse,
    .fn_postparse = obr_postparse,

    .scanner = &kCallbacks,

    .fn_passes = {
        [ePassForwardDecls] = obr_forward_decls,
        [ePassImportModules] = obr_process_imports,
        [ePassCompileDecls] = obr_compile_module
    }
};

CT_LANG_EXPORT(kOberonModule)
