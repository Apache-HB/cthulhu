#pragma once

#include <stdint.h>

#include "scanner.h"
#include "type.h"

typedef struct {
    struct node_t *data;
    size_t len, size;
} nodes_t;

typedef enum {
    /**
     * expressions
     */
    AST_DIGIT,
    AST_IDENT,
    AST_UNARY,
    AST_BINARY,

    /**
     * statements
     */
    AST_STMTS,
    AST_RETURN,
    AST_BRANCH,

    /**
     * declarations
     */
    AST_DECL_FUNC,
    AST_DECL_VAR,

    /**
     * implementation details
     */
    AST_TYPE
} ast_t;

typedef enum {
    UNARY_ABS, /* +expr */
    UNARY_NEG, /* -expr */
    UNARY_TRY /* expr? */
} unary_t;

typedef enum {
    BINARY_ADD, /* expr + expr */
    BINARY_SUB, /* expr - expr */
    BINARY_MUL, /* expr * expr */
    BINARY_DIV, /* expr / expr */
    BINARY_REM /* expr % expr */
} binary_t;

typedef struct node_t {
    /**
     * the type of this node 
     */
    ast_t kind;

    /**
     * the scanner that produced this node
     */
    scanner_t *scanner;

    /**
     * where in the scanner this node came from
     */
    where_t where;

    /**
     * the type of this nodes expression in its current context
     */
    type_t *typeof;

    union {
        /* AST_IDENT */
        char *ident;

        /* AST_DIGIT */
        uint64_t digit;

        /* AST_STMTS */
        nodes_t *stmts;

        struct {
            /* AST_UNARY */
            unary_t unary;

            /* AST_RETURN */
            struct node_t *expr;
        };

        /* AST_BINARY */
        struct {
            binary_t binary;
            struct node_t *lhs;
            struct node_t *rhs;
        };

        /* AST_BRANCH */
        struct {
            struct node_t *cond;
            struct node_t *branch;
        };

        /* AST_DECL */
        struct {
            union {
                char *name;

                /* AST_TYPE */
                const char *nameof;
            };

            union {
                /* AST_DECL_FUNC */
                struct {
                    struct node_t *result;
                    struct node_t *body;
                };

                /* AST_DECL_VAR */
                struct {
                    struct node_t *type;
                    struct node_t *init;
                };
            };
        };
    };
} node_t;

const char *node_name(node_t *node);

nodes_t *ast_list(node_t *init);
nodes_t *ast_append(nodes_t *list, node_t *node);

node_t *ast_digit(scanner_t *scanner, where_t where, char *digit);
node_t *ast_ident(scanner_t *scanner, where_t where, char *text);

node_t *ast_unary(scanner_t *scanner, where_t where, unary_t unary, node_t *expr);
node_t *ast_binary(scanner_t *scanner, where_t where, binary_t binary, node_t *lhs, node_t *rhs);

node_t *ast_stmts(scanner_t *scanner, where_t where, nodes_t *stmts);
node_t *ast_return(scanner_t *scanner, where_t where, node_t *expr);
node_t *ast_branch(scanner_t *scanner, where_t where, node_t *cond, node_t *branch);

node_t *ast_decl_func(scanner_t *scanner, where_t where, char *name, node_t *body);

node_t *ast_type(const char *name, type_t *typeof);

void connect_type(node_t *node, type_t *type);
