#define COMPILER_SOURCE 1

#include "cthulhu/ast/compile.h"
#include "cthulhu/ast/interop.h"
#include "cthulhu/util/file.h"

#include "cthulhu/report/report.h"
#include "cthulhu/util/util.h"

#include <string.h>

#include "driver/ctu/ast.h"

static scan_t scan_new(reports_t *reports, const char *language, const char *path)
{
    scan_t scan = {.language = language, .path = path, .reports = reports,};

    return scan;
}

scan_t scan_string(reports_t *reports, const char *language, const char *path, const char *text)
{
    text_t source = {.size = strlen(text), .text = text,};
    scan_t scan = scan_new(reports, language, path);

    scan.source = source;

    return scan;
}

scan_t scan_file(reports_t *reports, const char *language, file_t file)
{
    cerror_t error = 0;
    size_t size = file_size(file, &error);
    const char *text = file_map(file, &error);
    scan_t scan = scan_new(reports, language, file.path);
    text_t source = {.size = size, .text = text};

    if (text == NULL || error != 0)
    {
        report(reports, ERROR, node_invalid(), "failed to map file: %s", error_string(error));
    }

    scan.source = source;

    return scan;
}

scan_t scan_without_source(reports_t *reports, const char *language, const char *path)
{
    scan_t scan = scan_new(reports, language, path);
    text_t source = {.size = 0, .text = ""};
    scan.source = source;
    return scan;
}

void *compile_string(scan_t *scan, callbacks_t *callbacks)
{
    int err;
    void *scanner;
    void *state;

    if ((err = callbacks->init(scan, &scanner)))
    {
        ctu_assert(scan->reports, "failed to init parser for %s: %d", scan->path, err);
        return NULL;
    }

    if (!(state = callbacks->scan(scan_text(scan), scanner)))
    {
        report(scan->reports, ERROR, node_invalid(), "failed to scan %s", scan->path);
        return NULL;
    }

    if ((err = callbacks->parse(scan, scanner)))
    {
        report(scan->reports, ERROR, node_invalid(), "failed to parse %s: %d", scan->path, err);
        return NULL;
    }

    callbacks->destroyBuffer(state, scanner);
    callbacks->destroy(scanner);

    return scan_get(scan);
}

#define DUMP_AST(scanner) do { ast_t *ast = scan_get(scanner); logverbose("[ast] %p", ast->decls); } while (0)

void *compile_file(scan_t *scan, callbacks_t *callbacks)
{
    FILE *fd = scan_get(scan);

    int err = 0;
    void *state = NULL;

    logverbose("[compile-file] %p", scan_get(scan));

    if ((err = callbacks->init(scan, &state)))
    {
        return NULL;
    }

    logverbose("[compile-file] %p", scan_get(scan));

    callbacks->setIn(fd, state);

    logverbose("[compile-file] %p", scan_get(scan));

    if ((err = callbacks->parse(scan, state)))
    {
        return NULL;
    }

    DUMP_AST(scan);

    callbacks->destroy(state);

    DUMP_AST(scan);

    return scan_get(scan);
}
