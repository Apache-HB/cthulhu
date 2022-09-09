#include "cthulhu/hlir/ops.h"
#include "cthulhu/ssa/ssa.h"

#include "std/vector.h"
#include "std/str.h"
#include "std/set.h"

#include "base/macros.h"

#include <stdio.h>

typedef struct {
    reports_t *reports;
    set_t *blocks;
} emit_t;

static const char *emit_operand(emit_t *emit, set_t *edges, operand_t op)
{
    switch (op.kind)
    {
    case eOperandEmpty: return "";
    case eOperandReg: return format("%%%s", op.reg->id);
    case eOperandBlock: 
        set_add_ptr(edges, op.bb);
        return format(".%s", op.bb->id);
    case eOperandImm: return format("$%s", mpz_get_str(NULL, 10, op.imm.digit));
    default: return "unknown";
    }
}

static void emit_step(emit_t *emit, size_t i, set_t *edges, step_t *step)
{
    const char *id = step->id == NULL ? "" : step->id;
    switch (step->opcode)
    {
    case eOpValue:
        printf("  %%%s = %s\n", id, emit_operand(emit, edges, step->value));
        break;

    case eOpBinary:
        printf("  %%%s = binary %s %s %s\n", id, binary_name(step->binary), emit_operand(emit, edges, step->lhs), emit_operand(emit, edges, step->rhs));
        break;

    case eOpReturn:
        printf("  ret %s\n", emit_operand(emit, edges, step->value));
        break;

    default:
        printf("  <error>\n");
        break;
    }
}

static void emit_block(emit_t *emit, const block_t *block)
{
    if (set_contains_ptr(emit->blocks, block)) 
    {
        return;
    }
    set_add_ptr(emit->blocks, block);

    size_t len = vector_len(block->steps);
    set_t *edges = set_new(len);

    printf(".%s:\n", block->id);
    for (size_t i = 0; i < len; i++) 
    {
        step_t *step = vector_get(block->steps, i);
        emit_step(emit, i, edges, step);
    }

    set_iter_t iter = set_iter(edges);
    while (set_has_next(&iter))
    {
        const block_t *edge = set_next(&iter);
        emit_block(emit, edge);
    }
}

static void emit_flow(emit_t *emit, const flow_t *flow)
{
    printf("%s:\n", flow->name);
    emit_block(emit, flow->entry);
}

void eval_module(reports_t *reports, module_t *mod)
{
    size_t len = vector_len(mod->flows);
    emit_t emit = {
        .reports = reports,
        .blocks = set_new(0x100)
    };

    for (size_t i = 0; i < len; i++)
    {
        emit_flow(&emit, vector_get(mod->flows, i));
    }
}
