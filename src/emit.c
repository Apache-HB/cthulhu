#include "emit.h"

#include <stdio.h>

static void 
emit_stmt(node_t *stmt);

static void 
emit_type(node_t *type)
{
    switch (type->kind) {
    case NODE_BUILTIN_TYPE:
        if (!type->mut) {
            printf("const ");
        }
        printf("%s", type->name);
        break;
    case NODE_POINTER:
        emit_type(type->type);
        printf("*");
        break;
    case NODE_RECORD:
        printf("struct %s", type->decl.name);
        break;
    case NODE_TYPENAME:
        fprintf(stderr, "typename(%s) leaked past sema\n", type->name);
        break;
    default:
        printf("unknown type %d\n", type->kind);
        break;
    }
}

static void 
emit_stmt(node_t *stmt)
{
    size_t i = 0;
    node_t *node;
    switch (stmt->kind) {
    case NODE_COMPOUND:
        printf("{");
        for (; i < stmt->compound->length; i++) {
            node = stmt->compound->data + i;
            emit_stmt(node);
            if (node->kind != NODE_COMPOUND && node->kind != NODE_BRANCH)
                printf(";");
        }
        printf("}");
        break;
    case NODE_VAR:
        emit_type(stmt->decl.var.type);
        printf(" %s", stmt->decl.name);
        if (stmt->decl.var.init) {
            printf(" = ");
            emit_stmt(stmt->decl.var.init);
        }
        break;
    case NODE_BINARY:
        printf("(");
        emit_stmt(stmt->binary.lhs);
        switch (stmt->binary.op) {
        case BINARY_ADD: printf(" + "); break;
        case BINARY_SUB: printf(" - "); break;
        case BINARY_MUL: printf(" * "); break;
        case BINARY_DIV: printf(" / "); break;
        case BINARY_REM: printf(" %% "); break;
        default: printf("unknown binop\n"); break;
        }
        emit_stmt(stmt->binary.rhs);
        printf(")");
        break;
    case NODE_UNARY:
        switch (stmt->unary.op) {
        case UNARY_ABS: printf("abs("); emit_stmt(stmt->unary.expr); printf(")"); break;
        case UNARY_NEG: printf("("); emit_stmt(stmt->unary.expr); printf(" * -1)"); break;
        case UNARY_DEREF: printf("(*"); emit_stmt(stmt->unary.expr); printf(")"); break;
        case UNARY_REF: printf("(&"); emit_stmt(stmt->unary.expr); printf(")"); break;
        case UNARY_NOT: printf("(!"); emit_stmt(stmt->unary.expr); printf(")"); break;
        default: printf("unknown unop\n"); break;
        }
        break;
    case NODE_TERNARY:
        printf("(");
        emit_stmt(stmt->ternary.cond);
        printf("?");
        emit_stmt(stmt->ternary.yes);
        printf(":");
        emit_stmt(stmt->ternary.no);
        printf(")");
        break;
    case NODE_CALL:
        emit_stmt(stmt->call.body);
        printf("(");
        for (; i < stmt->call.args->length; i++) {
            if (i) {
                printf(", ");
            }
            emit_stmt(stmt->call.args->data + i);
        }
        printf(")");
        break;
    case NODE_RETURN:
        printf("return");
        if (stmt->expr) {
            printf(" ");
            emit_stmt(stmt->expr);
        }
        break;
    case NODE_DIGIT:
        printf("%s", stmt->digit);
        break;
    case NODE_NAME:
        printf("%s", stmt->name);
        break;
    case NODE_ASSIGN:
        emit_stmt(stmt->assign.old);
        printf(" = ");
        emit_stmt(stmt->assign.expr);
        break;
    case NODE_BOOL:
        printf("%d", stmt->boolean);
        break;
    case NODE_STRING:
        printf("%s", stmt->text);
        break;
    case NODE_BRANCH:
        if (stmt->branch.cond) {
            printf("if (");
            emit_stmt(stmt->branch.cond);
            printf(")");
        }
        emit_stmt(stmt->branch.body);
        if (stmt->branch.next) {
            printf("else");
            emit_stmt(stmt->branch.next);
        }
        break;
    case NODE_ACCESS:
        emit_stmt(stmt->access.expr);
        printf(".%s", stmt->access.field);
        break;
    default:
        printf("unknown node %d\n", stmt->kind);
    }
}

static void 
emit_param(node_t *decl)
{
    emit_type(decl->decl.param);
    printf(" %s", decl->decl.name);
}

static void 
emit_func(node_t *func)
{
    emit_type(func->decl.func.result);
    printf(" %s(", func->decl.name);
    size_t len = func->decl.func.params->length;

    if (len == 0) {
        printf("void");
    } else {
        for (size_t i = 0; i < len; i++) {
            if (i) {
                printf(", ");
            }
            emit_param(func->decl.func.params->data + i);
        }
    }
    printf(")");

    emit_stmt(func->decl.func.body);
}

static void
emit_record(node_t *node)
{
    printf("struct %s {", node->decl.name);
    for (size_t i = 0; i < node->decl.fields->length; i++) {
        node_t *field = (node->decl.fields->data + i);
        emit_type(field->decl.param);
        printf(" %s;", field->decl.name);
    }
    printf("};");
}

static void 
emit_decl(node_t *decl)
{
    switch (decl->kind) {
    case NODE_FUNC: 
        if (!decl->decl.exported) {
            printf("static ");
        }
        emit_func(decl);
        break;
    case NODE_VAR:
        if (!decl->decl.exported) {
            printf("static ");
        }
        emit_stmt(decl);
        printf(";");
        break;
    case NODE_RECORD:
        emit_record(decl);
        break;
    default:
        printf("unknown decl to emit\n");
        break;
    }
    printf("\n");
}

void 
emit(node_t *prog)
{
    for (size_t i = 0; i < prog->compound->length; i++) {
        emit_decl(prog->compound->data + i);
    }
}
