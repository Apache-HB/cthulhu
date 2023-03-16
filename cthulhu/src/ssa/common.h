#pragma once

#include "cthulhu/ssa/ssa.h"

const char *ssa_opcode_name(ssa_opcode_t op);
const char *ssa_operand_name(ssa_operand_kind_t op);
const char *ssa_kind_name(ssa_kind_t kind);

const ssa_type_t *ssa_get_step_type(ssa_step_t step);

ssa_kind_t ssa_get_value_kind(const ssa_value_t *value);

ssa_param_t *ssa_param_new(const char *name, const ssa_type_t *type);

ssa_type_t *ssa_type_new(ssa_kind_t kind, const char *name);
ssa_type_t *type_empty_new(const char *name);

ssa_value_t *ssa_value_new(const ssa_type_t *type, bool init);
ssa_value_t *value_digit_new(mpz_t digit, const ssa_type_t *type);
ssa_value_t *value_empty_new(const ssa_type_t *type);
ssa_value_t *value_ptr_new(const ssa_type_t *type, const mpz_t value);
