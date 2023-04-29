#pragma once

#include <stdbool.h>

#include "base/version-def.h"

/**
 * argparse2 control flow
 * 
 * parser instance
 *   - contains a list of groups
 *   - each group contains a list of params
 *   - params may not share names
 *
 * when parsing:
 *  - when a param is encountered, its related events are called
 *  - posargs are sent to a global event
 *  - each event is part of its chain, simillar to windows driver filters
 *
 * parameters can be added while the parser is running
 *   - this is how modules are dynamically loaded
 * 
 */

typedef struct reports_t reports_t;
typedef struct node_t node_t;

typedef struct ap_t ap_t;
typedef struct ap_param_t ap_param_t;
typedef struct ap_group_t ap_group_t;

typedef enum ap_event_result_t
{
    eEventHandled,
    eEventContinue,
    
    eEventTotal
} ap_event_result_t;

/**
 * @brief callback for a parameter event
 *
 * @param ap the parser instance
 * @param param the parameter that triggered the event, may be NULL for positional args
 * @param value the value of the parameter, may be NULL for positional args
 *              is mpz_t for int, const char * for string, and bool* for bool
 * @param data the data passed to @see ap_event
 */
typedef ap_event_result_t (*ap_event_t)(
    ap_t *ap, 
    const ap_param_t *param, 
    const void *value, 
    void *data
);

typedef ap_event_result_t (*ap_error_t)(
    ap_t *ap, 
    const node_t *node,
    const char *message, 
    void *data
);

ap_t *ap_new(const char *desc, version_t version);

ap_group_t *ap_group_new(
    ap_t *self, 
    const char *name, 
    const char *desc
);

ap_param_t *ap_add_bool(ap_group_t *self, const char *desc, const char **names);
ap_param_t *ap_add_int(ap_group_t *self, const char *desc, const char **names);
ap_param_t *ap_add_string(ap_group_t *self, const char *desc, const char **names);

/**
 * @brief add a callback event to a parameter
 * 
 * @param self the parser instance
 * @param param the parameter to add the event to, or NULL to use for positional args
 * @param callback 
 * @param data 
 */
void ap_event(ap_t *self, ap_param_t *param, ap_event_t callback, void *data);

void ap_error(ap_t *self, ap_error_t callback, void *data);

// TODO: remove reports here, requires a refactor of the reports module

/**
 * @brief parse the command line
 * 
 * @param self the parser instance
 * @param reports reporting channel
 * @param argc from main
 * @param argv from main
 * @return int exit code
 */
int ap_parse(ap_t *self, reports_t *reports, int argc, const char **argv);
