#include "scan/compile.h"

#include "report/report.h"

void *compile_scanner(scan_t *extra, callbacks_t *callbacks)
{
    int err = 0;
    void *scanner = NULL;
    void *state = NULL;

    reports_t *reports = scan_reports(extra);
    const char *path = scan_path(extra);

    err = callbacks->init(extra, &scanner);
    if (err != 0)
    {
        ctu_assert(reports, "failed to init parser for %s: %d", path, err);
        return NULL;
    }

    state = callbacks->scan(scan_text(extra), scan_size(extra), scanner);
    if (state == NULL)
    {
        report(reports, eFatal, node_invalid(), "failed to scan %s", path);
        return NULL;
    }

    err = callbacks->parse(scanner, extra);
    if (err != 0)
    {
        report(reports, eFatal, node_invalid(), "failed to parse %s: %d", path, err);
        return NULL;
    }

    callbacks->destroyBuffer(state, scanner);
    callbacks->destroy(scanner);

    return scan_get(extra);
}
