#include "cthulhu/util/report-ext.h"

#include "cthulhu/util/str.h"

message_t *report_shadow(reports_t *reports, const char *name, const node_t *shadowed, const node_t *shadowing)
{
    message_t *id = report(reports, ERROR, shadowing, "redefinition of `%s`", name);
    report_append(id, shadowed, "previous definition");
    return id;
}

message_t *report_unknown_character(reports_t *reports, const node_t *node, const char *str)
{
    where_t where = node->where;

    column_t width = where.lastColumn - where.firstColumn;
    char *normal = str_normalizen(str, MAX(width, 1));
    message_t *id = report(reports, ERROR, node, "unknown character `%s`", normal);

    return id;
}
