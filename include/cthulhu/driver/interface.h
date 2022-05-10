#pragma once

#include "cthulhu/hlir/hlir.h"
#include "cthulhu/hlir/sema.h"
#include "cthulhu/util/report.h"
#include "cthulhu/util/version-def.h"
#include "cthulhu/util/file.h"

///
/// code required by interfaces
///

typedef struct
{
    reports_t *reports;
    map_t *modules;
} runtime_t;

typedef struct
{
    file_t file;
    scan_t *scanner;

    void *ast;
    const char *moduleName;
    hlir_t *hlir;
    sema_t *sema;
} compile_t;

typedef void (*init_compiler_t)(runtime_t *);

// parse and register a module
typedef void (*parse_file_t)(runtime_t *, compile_t *);

// forward declare all exports
typedef void (*forward_decls_t)(runtime_t *, compile_t *);

// process all imports
typedef void (*import_modules_t)(runtime_t *, compile_t *);

// compile the file
typedef void (*compile_module_t)(runtime_t *, compile_t *);

typedef struct
{
    const char *name;
    version_t version;

    init_compiler_t fnInitCompiler;
    parse_file_t fnParseFile;
    forward_decls_t fnForwardDecls;
    import_modules_t fnResolveImports;
    compile_module_t fnCompileModule;
} driver_t;

typedef struct
{
    driver_t driver;
    
    int status;

    reports_t *reports;

    runtime_t runtime;
    vector_t *compiles;

    vector_t *sources;
    vector_t *plugins;
} cthulhu_t;

cthulhu_t *cthulhu_new(driver_t driver, vector_t *sources, vector_t *plugins);

int cthulhu_init(cthulhu_t *cthulhu);
int cthulhu_parse(cthulhu_t *cthulhu);
int cthulhu_forward(cthulhu_t *cthulhu);
int cthulhu_resolve(cthulhu_t *cthulhu);
int cthulhu_compile(cthulhu_t *cthulhu);

/**
 * @brief initialize the common runtime, always the first function an interface
 * should call
 */
void common_init(void);

// interface implemented by the language driver

CTHULHU_EXTERN driver_t get_driver(void);
