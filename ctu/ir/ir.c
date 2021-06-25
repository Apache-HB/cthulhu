#include "ir.h"

#include "ctu/util/report.h"

#include "ctu/sema/sema.h"

#include <stdlib.h>
#include <string.h>

/**
 * builder functions
 */

static operand_t new_operand(int kind) {
    operand_t op = { kind, { } };
    return op;
}

static operand_t new_imm(int64_t imm) {
    operand_t op = new_operand(IMM);
    op.imm = imm;
    return op;
}

static operand_t new_vreg(vreg_t reg) {
    operand_t op = new_operand(VREG);
    op.vreg = reg;
    return op;
}

static operand_t new_func(size_t idx) {
    operand_t op = new_operand(FUNC);
    op.func = idx;
    return op;
}

static operand_t new_arg(size_t idx) {
    operand_t op = new_operand(ARG);
    op.arg = idx;
    return op;
}

static operand_t new_block(size_t addr) {
    operand_t op = new_operand(BLOCK);
    op.label = addr;
    return op;
}

static step_t new_typed_step(opcode_t op, type_t *type) {
    step_t step = { op, type, { } };
    return step;
}

static step_t new_step(opcode_t op, node_t *node) {
    return new_typed_step(op, get_type(node));
}

/**
 * book keeping
 */

static size_t add_step_raw(flow_t *flow, step_t step) {
    if (flow->len + 1 > flow->size) {
        flow->size += 64;
        flow->steps = realloc(flow->steps, flow->size * sizeof(step_t));
    }

    flow->steps[flow->len] = step;
    return flow->len++;
}

static step_t *add_step(flow_t *flow, step_t step) {
    size_t idx = add_step_raw(flow, step);
    return step_at(flow, idx);
}

static operand_t add_vreg(flow_t *flow, step_t step) {
    return new_vreg(add_step_raw(flow, step));
}

static operand_t add_block(flow_t *flow) {
    /* a block cant have a type so null is fine */
    step_t block = new_typed_step(OP_BLOCK, NULL);
    return new_block(add_step_raw(flow, block));
}

/**
 * codegen logic
 */

static operand_t emit_opcode(flow_t *flow, node_t *node);

static operand_t emit_digit(node_t *node) {
    return new_imm(node->digit);
}

static operand_t emit_unary(flow_t *flow, node_t *node) {
    operand_t expr = emit_opcode(flow, node->expr);

    step_t step = new_step(OP_UNARY, node);
    step.unary = node->unary;
    step.expr = expr;

    return add_vreg(flow, step);
}

static operand_t emit_binary(flow_t *flow, node_t *node) {
    operand_t lhs = emit_opcode(flow, node->lhs),
              rhs = emit_opcode(flow, node->rhs);

    step_t step = new_step(OP_BINARY, node);
    step.binary = node->binary;
    step.lhs = lhs;
    step.rhs = rhs;

    return add_vreg(flow, step);
}

static operand_t emit_stmts(flow_t *flow, node_t *node) {
    operand_t block = add_block(flow);
    
    for (size_t i = 0; i < node->stmts->len; i++) {
        emit_opcode(flow, ast_at(node->stmts, i));
    }

    return block;
}

static step_t step_return(type_t *type, operand_t value) {
    step_t step = new_typed_step(OP_RETURN, type);
    step.value = value;
    return step;
}

static operand_t emit_return(flow_t *flow, node_t *node) {
    operand_t value = node->expr == NULL 
        ? new_operand(NONE)
        : emit_opcode(flow, node->expr);

    step_t step = step_return(get_type(node), value);

    return add_vreg(flow, step);
}

static operand_t emit_call(flow_t *flow, node_t *node) {
    operand_t expr = emit_opcode(flow, node->expr);

    size_t len = ast_len(node->args);
    operand_t *args = malloc(sizeof(operand_t) * len);

    for (size_t i = 0; i < len; i++) {
        args[i] = emit_opcode(flow, ast_at(node->args, i));
    }

    step_t step = new_step(OP_CALL, node);
    step.value = expr;
    step.args = args;
    step.len = len;

    return add_vreg(flow, step);
}

static operand_t emit_symbol(flow_t *flow, node_t *node) {
    /**
     * first try all functions
     */
    for (size_t i = 0; i < flow->mod->len; i++) {
        const char *name = flow->mod->flows[i].name;

        if (strcmp(name, node->ident) == 0) {
            return new_func(i);
        }
    }

    /**
     * now try arguments
     */

    for (size_t i = 0; i < flow->nargs; i++) {
        arg_t arg = flow->args[i];
        if (strcmp(arg.name, arg.name) == 0) {
            return new_arg(i);
        }
    }

    /* oh no */
    return new_operand(NONE);
}

static operand_t emit_branch(flow_t *flow, node_t *node) {
    operand_t cond = emit_opcode(flow, node->cond);
    
    step_t *branch = add_step(flow, new_typed_step(OP_BRANCH, NULL));

    operand_t body = emit_opcode(flow, node->branch);

    operand_t tail = add_block(flow);

    branch->cond = cond;
    branch->block = body;
    branch->other = tail;

    return cond;
}

static operand_t emit_convert(flow_t *flow, node_t *node) {
    operand_t body = emit_opcode(flow, node->expr);

    step_t step = new_step(OP_CONVERT, node);
    step.value = body;

    return add_vreg(flow, step);
}

static operand_t emit_opcode(flow_t *flow, node_t *node) {
    switch (node->kind) {
    case AST_STMTS: return emit_stmts(flow, node);
    case AST_DIGIT: return emit_digit(node);
    case AST_UNARY: return emit_unary(flow, node);
    case AST_BINARY: return emit_binary(flow, node);
    case AST_RETURN: return emit_return(flow, node);
    case AST_CALL: return emit_call(flow, node);
    case AST_SYMBOL: return emit_symbol(flow, node);
    case AST_BRANCH: return emit_branch(flow, node);
    case AST_CAST: return emit_convert(flow, node);
    default:
        reportf(LEVEL_INTERNAL, node, "unknown node kind %d", node->kind);
        return new_operand(NONE);
    }
}



/**
 * external api
 */

static flow_t compile_flow(module_t *mod, node_t *node) {
    ASSERT(node->kind == AST_DECL_FUNC)("compile_flow requires a function");

    nodes_t *params = node->params;
    size_t len = ast_len(params);

    flow_t flow = { 
        /* name */
        get_decl_name(node), 
        
        /* arguments */
        malloc(sizeof(arg_t) * len), len,

        /* body */
        malloc(sizeof(step_t) * 64), 0, 64, 
        
        /* return type */
        get_type(node->result),

        /* parent */
        mod 
    };

    for (size_t i = 0; i < len; i++) {
        node_t *param = ast_at(params, i);

        arg_t arg = { get_decl_name(param), get_type(param) };

        flow.args[i] = arg;
    }

    emit_opcode(&flow, node->body);

    if (is_void(get_type(node->result))) {
        step_t ret = step_return(VOID_TYPE, new_operand(NONE));
        add_step(&flow, ret);
    }

    return flow;
}

module_t compile_module(nodes_t *nodes) {
    size_t len = ast_len(nodes);
    module_t mod = { malloc(sizeof(flow_t) * len), len };
    
    for (size_t i = 0; i < len; i++) {
        mod.flows[i].name = get_decl_name(ast_at(nodes, i));
    }

    for (size_t i = 0; i < len; i++) {
        mod.flows[i] = compile_flow(&mod, ast_at(nodes, i));
    }

    return mod;
}

step_t *step_at(flow_t *flow, size_t idx) {
    return flow->steps + idx;
}
