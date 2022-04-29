#pragma once

#include "ast.h"
#include "cthulhu/driver/driver.h"
#include "cthulhu/hlir/hlir.h"
#include "cthulhu/util/report.h"

void pl0_init();

void pl0_forward_decls(runtime_t *runtime, compile_t *compile);
void pl0_process_imports(runtime_t *runtime, compile_t *compile);
void pl0_compile_module(runtime_t *runtime, compile_t *compile);
