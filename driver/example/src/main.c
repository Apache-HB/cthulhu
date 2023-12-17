#include "cthulhu/mediator/driver.h"

#include "cthulhu/tree/tree.h"

#include "std/vector.h"

#include "report/report.h"

#include "scan/node.h"

static vector_t *example_lang_path(void)
{
    vector_t *path = vector_new(2);
    vector_push(&path, "example");
    vector_push(&path, "lang");
    return path;
}

static tree_t *example_lang_module(lifetime_t *lifetime)
{
    reports_t *reports = lifetime_get_reports(lifetime);
    cookie_t *cookie = lifetime_get_cookie(lifetime);

    const node_t *node = node_builtin();
    size_t sizes[eSemaTotal] = {
        [eSemaValues] = 1,
        [eSemaTypes] = 1,
        [eSemaProcs] = 1,
        [eSemaModules] = 1
    };

    return tree_module_root(reports, cookie, node, "runtime", eSemaTotal, sizes);
}

static void ex_create(driver_t *handle)
{
    lifetime_t *lifetime = handle_get_lifetime(handle);

    vector_t *path = example_lang_path();
    tree_t *mod = example_lang_module(lifetime);
    context_t *ctx = compiled_new(handle, mod);

    add_context(lifetime, path, ctx);

    logverbose("ex-create(0x%p)", (void*)handle);
}

static void ex_destroy(driver_t *handle)
{
    logverbose("ex-destroy(0x%p)", (void*)handle);
}

static void ex_parse(driver_t *handle, scan_t *scan)
{
    logverbose("ex-parse(0x%p, %s)", (void*)handle, scan_path(scan));
}

static void ex_forward_symbols(context_t *context)
{
    logverbose("ex-forward(0x%p)", (void*)context);
}

static void ex_compile_imports(context_t *context)
{
    logverbose("ex-compile-imports(0x%p)", (void*)context);
}

static void ex_compile_types(context_t *context)
{
    logverbose("ex-compile-types(0x%p)", (void*)context);
}

static void ex_compile_symbols(context_t *context)
{
    logverbose("ex-compile-symbols(0x%p)", (void*)context);
}

static const char *kLangNames[] = { "e", "example", NULL };

const language_t kExampleModule = {
    .id = "example",
    .name = "Example",
    .version = {
        .license = "GPLv3",
        .desc = "Example language driver",
        .author = "Elliot Haisley",
        .version = NEW_VERSION(1, 0, 1)
    },

    .exts = kLangNames,

    .fnCreate = ex_create,
    .fnDestroy = ex_destroy,

    .fnParse = ex_parse,
    .fnCompilePass = {
        [eStageForwardSymbols] = ex_forward_symbols,
        [eStageCompileImports] = ex_compile_imports,
        [eStageCompileTypes] = ex_compile_types,
        [eStageCompileSymbols] = ex_compile_symbols
    }
};
