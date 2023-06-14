#include "common.h"

#include "ap-bison.h"

#include "base/panic.h"
#include "base/util.h"
#include "base/memory.h"

#include "std/map.h"
#include "std/vector.h"
#include "std/str.h"

#include "report/report.h"

#include <string.h>

// internals

static void apply_callbacks(scan_t *scan, where_t where, const ap_param_t *param, const void *value, vector_t *all)
{
    ap_t *self = scan_get(scan);
    size_t len = vector_len(all);

    for (size_t i = 0; i < len; i++)
    {
        ap_callback_t *cb = vector_get(all, i);
        ap_event_result_t result = cb->callback(self, param, value, cb->data);

        if (result == eEventHandled)
        {
            return;
        }
    }

    const char *msg = param == NULL 
        ? format("unhandled positional argument '%s'", value)
        : format("unhandled event '%s'", param->names[0]);
    ap_on_error(scan, where, msg);
}

static void add_value(ap_t *self, const ap_param_t *param, void *value)
{
    map_set_ptr(self->paramValues, param, value);
}

// flex + bison callbacks

void ap_on_string(scan_t *scan, where_t where, const ap_param_t *param, const char *value)
{
    ap_t *self = scan_get(scan);
    add_value(self, param, ctu_strdup(value));
    apply_callbacks(scan, where, param, value, map_get_ptr(self->eventLookup, param));
}

void ap_on_bool(scan_t *scan, where_t where, const ap_param_t *param, bool value)
{
    ap_t *self = scan_get(scan);
    add_value(self, param, BOX(value));
    apply_callbacks(scan, where, param, &value, map_get_ptr(self->eventLookup, param));
}

void ap_on_int(scan_t *scan, where_t where, const ap_param_t *param, mpz_t value)
{
    ap_t *self = scan_get(scan);
    
    void *it = ctu_malloc(sizeof(mpz_t));
    memcpy(it, value, sizeof(mpz_t));
    add_value(self, param, it);

    apply_callbacks(scan, where, param, value, map_get_ptr(self->eventLookup, param));
}

void ap_on_posarg(scan_t *scan, where_t where, const char *value)
{
    ap_t *self = scan_get(scan);
    apply_callbacks(scan, where, NULL, value, self->posArgCallbacks);
}

void ap_on_error(scan_t *scan, where_t where, const char *message)
{
    ap_t *self = scan_get(scan);
    node_t *node = node_new(scan, where);
    size_t len = vector_len(self->errorCallbacks);

    for (size_t i = 0; i < len; i++)
    {
        ap_err_callback_t *cb = vector_get(self->errorCallbacks, i);
        cb->callback(self, node, message, cb->data);
    }
}

int ap_get_opt(ap_t *self, const char *name, ap_param_t **param, char **error)
{
    CTASSERT(self != NULL);
    CTASSERT(name != NULL);
    CTASSERT(param != NULL);

    ap_param_t *result = map_get(self->nameLookup, name);
    if (result == NULL)
    {
        *error = format("unknown option '%s'", name);
        return AP_ERROR;
    }

    *param = result;

    switch (result->type)
    {
    case eParamBool: return AP_BOOL;
    case eParamInt: return AP_INT;
    case eParamString: return AP_STRING;
    default: return AP_ERROR;
    }
}

void aperror(where_t *where, void *state, scan_t *scan, const char *msg)
{
    ap_on_error(scan, *where, ctu_strdup(msg));
}
