#include "ast.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static node_t*
new_node(node_kind_t kind) 
{
    node_t *node = malloc(sizeof(node_t));
    node->kind = kind;
    return node;
}

static nodes_t*
new_nodes(size_t size)
{
    nodes_t *nodes = malloc(sizeof(nodes_t));
    nodes->length = 0;
    nodes->size = size;
    nodes->data = malloc(sizeof(node_t) * size);
    return nodes;
}

nodes_t*
empty_node_list()
{
    nodes_t *nodes = new_nodes(4);
    return nodes;
}

nodes_t*
new_node_list(node_t *init)
{
    nodes_t *nodes = new_nodes(4);
    nodes = node_append(nodes, init);
    return nodes;
}

nodes_t*
node_append(nodes_t *self, node_t *node)
{
    if (self->length + 1 >= self->size) {
        self->size += 8;
        self->data = realloc(self->data, sizeof(node_t) * self->size);
    }
    memcpy(self->data + self->length, node, sizeof(node_t));
    self->length += 1;
    return self;
}

node_t*
new_digit(char* digit) 
{
    node_t *node = new_node(NODE_DIGIT);
    node->digit = digit;
    return node;
}

node_t*
new_unary(unary_op_t op, node_t *expr) 
{
    node_t *node = new_node(NODE_UNARY);
    node->unary.op = op;
    node->unary.expr = expr;
    return node;
}

node_t*
new_binary(binary_op_t op, node_t *lhs, node_t *rhs) 
{
    node_t *node = new_node(NODE_BINARY);
    node->binary.op = op;
    node->binary.lhs = lhs;
    node->binary.rhs = rhs;
    return node;
}

node_t*
new_ternary(node_t *cond, node_t *yes, node_t *no) 
{
    node_t *node = new_node(NODE_TERNARY);
    node->ternary.cond = cond;
    node->ternary.yes = yes;
    node->ternary.no = no;
    return node;
}

node_t*
new_call(node_t *body, nodes_t *args)
{
    node_t *node = new_node(NODE_CALL);
    node->call.body = body;
    node->call.args = args;
    return node;
}

node_t*
new_return(node_t *expr)
{
    node_t *node = new_node(NODE_RETURN);
    node->expr = expr;
    return node;
}

node_t*
new_func(char *name, node_t *result, node_t *body)
{
    node_t *node = new_node(NODE_FUNC);
    node->func.name = name;
    node->func.result = result;
    node->func.body = body;
    return node;
}

node_t*
new_name(char *name)
{
    node_t *node = new_node(NODE_NAME);
    node->name = name;
    return node;
}

node_t*
new_typename(char *name)
{
    node_t *node = new_node(NODE_TYPENAME);
    node->name = name;
    return node;
}

node_t*
new_pointer(node_t *type)
{
    node_t *node = new_node(NODE_POINTER);
    node->type = type;
    return node;
}

node_t*
new_compound(nodes_t *nodes)
{
    node_t *node = new_node(NODE_COMPOUND);
    node->compound = nodes;
    return node;
}

static void
dump_nodes(nodes_t *nodes)
{
    for (size_t i = 0; i < nodes->length; i++) {
        if (i != 0) {
            printf(" ");
        }
        dump_node(nodes->data + i);
    }
}

void
dump_node(node_t *node)
{
    switch (node->kind) {
    case NODE_DIGIT: 
        printf("%s", node->digit); 
        break;
    case NODE_UNARY:    
        printf("("); 
        switch (node->unary.op) {
        case UNARY_ABS: printf("abs "); break;
        case UNARY_NEG: printf("neg "); break;
        } 
        dump_node(node->unary.expr);
        printf(")");
        break;
    case NODE_BINARY: 
        printf("(");
        switch (node->binary.op) {
        case BINARY_ADD: printf("add "); break;
        case BINARY_SUB: printf("sub "); break;
        case BINARY_DIV: printf("div "); break;
        case BINARY_MUL: printf("mul "); break;
        case BINARY_REM: printf("rem "); break;
        }
        dump_node(node->binary.lhs);
        printf(" ");
        dump_node(node->binary.rhs);
        printf(")");
        break;
    case NODE_TERNARY:
        printf("(if ");
        dump_node(node->ternary.cond);
        printf(" then ");
        dump_node(node->ternary.yes);
        printf(" else ");
        dump_node(node->ternary.no);
        printf(")");
        break;
    case NODE_CALL:
        printf("(call ");
        dump_node(node->call.body);
        printf(" ");
        dump_nodes(node->call.args);
        printf(")");
        break;
    case NODE_FUNC:
        printf("(def %s ", node->func.name);
        if (node->func.result) {
            dump_node(node->func.result);
            printf(" ");
        }
        dump_node(node->func.body);
        printf(")");
        break;
    case NODE_COMPOUND:
        printf("(");
        dump_nodes(node->compound);
        printf(")");
        break;
    case NODE_NAME:
        printf("%s", node->name);
        break;
    case NODE_RETURN:
        if (node->expr) {
            printf("(return ");
            dump_node(node->expr);
            printf(")");
        } else {
            printf("return");
        }
        break;
    case NODE_TYPENAME:
        printf("%s", node->name);
        break;
    case NODE_POINTER:
        printf("(ptr ");
        dump_node(node->type);
        printf(")");
        break;
    }
}
