#include "core/macros.h"
#include "ctu/driver.h"

#include "cthulhu/runtime/driver.h"

#include "interop/compile.h"

#include "std/vector.h"
#include "std/str.h"

#include "arena/arena.h"

#include "driver/driver.h"

#include "ctu_bison.h" // IWYU pragma: keep
#include "ctu_flex.h" // IWYU pragma: keep

CTU_CALLBACKS(kCallbacks, ctu);

static vector_t *mod_basename(const char *fp, arena_t *arena)
{
    return vector_init(str_basename(fp, arena), arena);
}

static vector_t *find_mod_path(ctu_t *ast, const char *fp, arena_t *arena)
{
    if (ast == NULL) { return mod_basename(fp, arena); }

    return vector_len(ast->modspec) > 0
        ? ast->modspec
        : mod_basename(fp, arena);
}

static void *ctu_preparse(driver_t *driver, scan_t *scan)
{
    CTU_UNUSED(scan);

    lifetime_t *lifetime = handle_get_lifetime(driver);
    arena_t *arena = lifetime_get_arena(lifetime);

    ctu_scan_t info = {
        .reports = lifetime_get_logger(lifetime),
        .attribs = vector_new(4, arena)
    };

    return arena_memdup(&info, sizeof(ctu_scan_t), arena);
}

static void ctu_postparse(driver_t *driver, scan_t *scan, void *tree)
{
    ctu_t *ast = tree;
    CTASSERT(ast->kind == eCtuModule);
    arena_t *arena = scan_get_arena(scan);

    vector_t *path = find_mod_path(ast, scan_path(scan), arena);

    lifetime_t *lifetime = handle_get_lifetime(driver);
    context_t *ctx = context_new(driver, vector_tail(path), ast, NULL);

    add_context(lifetime, path, ctx);
}

static const diagnostic_t * const kDiagnosticTable[] = {
#define NEW_EVENT(name, ...) &kEvent_##name,
#include "ctu/events.inc"
};

static const char *const kLangNames[] = { "ct", "ctu", "cthulhu", NULL };

CT_DRIVER_API const language_t kCtuModule = {
    .id = "ctu",
    .name = "Cthulhu",
    .version = {
        .license = "LGPLv3",
        .desc = "Cthulhu language driver",
        .author = "Elliot Haisley",
        .version = NEW_VERSION(0, 4, 0)
    },

    .exts = kLangNames,

    .diagnostics = {
        .diagnostics = kDiagnosticTable,
        .count = sizeof(kDiagnosticTable) / sizeof(diagnostic_t*),
    },

    .fn_create = ctu_init,

    .fn_preparse = ctu_preparse,
    .fn_postparse = ctu_postparse,
    .parse_callbacks = &kCallbacks,

    .fn_compile_passes = {
        [eStageForwardSymbols] = ctu_forward_decls,
        [eStageCompileImports] = ctu_process_imports,
        [eStageCompileSymbols] = ctu_compile_module
    }
};

CTU_DRIVER_ENTRY(kCtuModule)
