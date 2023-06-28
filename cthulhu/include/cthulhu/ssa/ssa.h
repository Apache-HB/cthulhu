#pragma once

#include "cthulhu/hlir/ops.h"

#include <gmp.h>
#include <stdbool.h>

typedef struct reports_t reports_t;
typedef struct vector_t vector_t;
typedef struct typevec_t typevec_t;
typedef struct map_t map_t;

typedef struct h2_t h2_t;

typedef struct ssa_symbol_t ssa_symbol_t;

typedef struct ssa_block_t ssa_block_t;
typedef struct ssa_step_t ssa_step_t;
typedef struct ssa_operand_t ssa_operand_t;

typedef struct ssa_type_t ssa_type_t;
typedef struct ssa_value_t ssa_value_t;

typedef enum ssa_kind_t {
    eTypeEmpty,
    eTypeUnit,
    eTypeBool,
    eTypeDigit,
    eTypeString,
    eTypeClosure,
    eTypeQualify,

    eTypeTotal
} ssa_kind_t;

typedef enum ssa_opkind_t {
    eOperandEmpty,
    eOperandImm,
    eOperandBlock,
    eOperandGlobal,
    eOperandLocal,
    eOperandReg,
    eOperandFunction,

    eOperandTotal
} ssa_opkind_t;

typedef enum ssa_opcode_t {
    eOpStore,
    eOpLoad,
    eOpAddress,

    eOpImm,
    eOpUnary,
    eOpBinary,
    eOpCompare,
    
    eOpCast,
    eOpCall,

    eOpIndex, // get the address of an element in an array
    eOpMember, // get the address of a field in a struct

    /* control flow */
    eOpReturn,
    eOpBranch,
    eOpJump,

    eOpTotal
} ssa_opcode_t;

typedef struct ssa_type_t {
    ssa_kind_t kind;
    const char *name;
} ssa_type_t;

typedef struct ssa_param_t {
    const char *name;
    const ssa_type_t *type;
} ssa_param_t;

typedef struct ssa_local_t {
    const char *name;
    const ssa_type_t *type;
} ssa_local_t;

typedef struct ssa_field_t {
    const char *name;
    const ssa_type_t *type;
} ssa_field_t;

typedef struct ssa_value_t {
    ssa_kind_t kind;
    const ssa_type_t *type;

    union {
        mpz_t digitValue;
        bool boolValue;

        struct {
            const char *stringValue;
            size_t stringLength;
        };
    };
} ssa_value_t;

typedef struct ssa_operand_t {
    ssa_opkind_t kind;

    union {
        const ssa_block_t *bb;
        const ssa_step_t *vreg;
        
        size_t local;
        size_t param;

        const ssa_symbol_t *global;
        const ssa_symbol_t *function;

        const ssa_value_t *value;
    };
} ssa_operand_t;

typedef struct ssa_store_t {
    ssa_operand_t dst;
    ssa_operand_t src;
} ssa_store_t;

typedef struct ssa_load_t {
    ssa_operand_t src;
} ssa_load_t;

typedef struct ssa_addr_t {
    ssa_operand_t symbol;
} ssa_addr_t;

typedef struct ssa_imm_t {
    const ssa_value_t *value;
} ssa_imm_t;

typedef struct ssa_unary_t {
    ssa_operand_t operand;
    unary_t unary;
} ssa_unary_t;

typedef struct ssa_binary_t {
    ssa_operand_t lhs;
    ssa_operand_t rhs;
    binary_t binary;
} ssa_binary_t;

typedef struct ssa_compare_t {
    ssa_operand_t lhs;
    ssa_operand_t rhs;
    compare_t compare;
} ssa_compare_t;

typedef struct ssa_cast_t {
    ssa_operand_t operand;
    const ssa_type_t *type;
} ssa_cast_t;

typedef struct ssa_call_t {
    ssa_operand_t function;
    typevec_t *args;
} ssa_call_t;

typedef struct ssa_index_t {
    ssa_operand_t array;
    ssa_operand_t index;
} ssa_index_t;

typedef struct ssa_member_t {
    ssa_operand_t object;
    const ssa_field_t *field;
} ssa_member_t;

typedef struct ssa_return_t {
    ssa_operand_t value;
} ssa_return_t;

typedef struct ssa_branch_t {
    ssa_operand_t cond;
    ssa_operand_t then;
    ssa_operand_t other;
} ssa_branch_t;

typedef struct ssa_jump_t {
    ssa_operand_t target;
} ssa_jump_t;

typedef struct ssa_step_t {
    ssa_opcode_t opcode;

    union {
        ssa_store_t store;
        ssa_load_t load;
        ssa_addr_t addr;

        ssa_imm_t imm;
        ssa_unary_t unary;
        ssa_binary_t binary;
        ssa_compare_t compare;

        ssa_cast_t cast;
        ssa_call_t call;

        ssa_index_t index;
        ssa_member_t member;

        ssa_return_t ret;
        ssa_branch_t branch;
        ssa_jump_t jump;
    };
} ssa_step_t;

typedef struct ssa_block_t {
    const char *name;
    vector_t *steps;
} ssa_block_t;

typedef struct ssa_symbol_t {
    const char *name;
    const ssa_type_t *type;
    const ssa_value_t *value;
    const ssa_block_t *entry;
} ssa_symbol_t;

typedef struct ssa_module_t {
    const char *name;
    vector_t *globals;
} ssa_module_t;

map_t *ssa_compile(const h2_t *root);
