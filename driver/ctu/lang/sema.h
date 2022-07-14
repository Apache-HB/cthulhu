#pragma once

#include "cthulhu/interface/interface.h"

typedef enum
{
    eTagValues,  // hlir_t*
    eTagProcs,   // hlir_t*
    eTagTypes,   // hlir_t*
    eTagModules, // sema_t*
    eTagAttribs, // hlir_t*
    eTagSuffix,  // suffix_t*

    eTagTotal
} tag_t;

hlir_t *get_digit_type(sign_t sign, digit_t digit);

void ctu_init_compiler(runtime_t *runtime);
void ctu_forward_decls(runtime_t *runtime, compile_t *compile);
void ctu_process_imports(runtime_t *runtime, compile_t *compile);
void ctu_compile_module(runtime_t *runtime, compile_t *compile);
