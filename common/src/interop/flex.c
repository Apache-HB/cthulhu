#include "base/macros.h"
#include "base/panic.h"

#include <string.h>
#include <limits.h>

#define COMPILER_SOURCE 1

#include "interop/flex.h"

void flex_action(where_t *where, const char *text)
{
    where->firstLine = where->lastLine;
    where->firstColumn = where->lastColumn;

    for (int64_t i = 0; text[i]; i++)
    {
        if (text[i] == '\n')
        {
            where->lastLine += 1;
            where->lastColumn = 0;
        }
        else
        {
            where->lastColumn += 1;
        }
    }
}

int flex_input(scan_t *scan, char *out, int size)
{
    CTASSERT(size <= INT_MAX);
    return scan_read(scan, out, size);
}

void flex_init(where_t *where)
{
    where->firstLine = 0;
    where->firstColumn = 0;
    where->lastLine = 0;
    where->lastColumn = 0;
}
