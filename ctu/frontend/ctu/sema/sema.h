#pragma once

#include "ast.h"
#include "ctu/lir/sema.h"

lir_t *ctu_sema(reports_t *reports, ctu_t *ctu);
sema_t *ctu_start(reports_t *reports, ctu_t *ctu);
lir_t *ctu_finish(sema_t *sema);

vector_t *ctu_analyze(reports_t *reports, ctu_t *ctu);
