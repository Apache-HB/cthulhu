#include "cthulhu/mediator/driver.h"

#include "std/vector.h"

#include "report/report.h"

static vector_t *example_lang_path(void)
{
    vector_t *path = vector_new(2);
    vector_push(&path, "example");
    vector_push(&path, "lang");

    return path;
}

static void ex_create(handle_t *handle)
{
    vector_t *path = example_lang_path();
    context_t *ctx = context_new(handle, "example", NULL, NULL, NULL);

    add_context(handle_get_lifetime(handle), path, ctx);

    logverbose("ex-create(0x%p)", handle);
}

static void ex_destroy(handle_t *handle)
{
    logverbose("ex-destroy(0x%p)", handle);
}

static void ex_parse(handle_t *handle, scan_t *scan)
{
    logverbose("ex-parse(0x%p, %s)", handle, scan_path(scan));
}

static void ex_forward_symbols(context_t *context)
{
    logverbose("ex-forward(0x%p)", context);
}

static void ex_compile_imports(context_t *context)
{
    logverbose("ex-compile-imports(0x%p)", context);
}

static void ex_compile_types(context_t *context)
{
    logverbose("ex-compile-types(0x%p)", context);
}

static void ex_compile_symbols(context_t *context)
{
    logverbose("ex-compile-symbols(0x%p)", context);
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
