#pragma once

#include "ctu/ast/ast.h"
#include "ctu/util/report.h"
#include "ctu/type/type.h"

#include <gmp.h>

typedef enum {
    /* integer literal */
    LIR_DIGIT,

    /* reference to a variable */
    LIR_NAME,

    /* binary expresion */
    LIR_BINARY,

    /* unary expression */
    LIR_UNARY,

    /* function call */
    LIR_CALL,

    /* assign to a variable */
    LIR_ASSIGN,

    LIR_WHILE,
    LIR_BRANCH,
    LIR_STMTS,

    LIR_VALUE,
    LIR_DEFINE,

    LIR_MODULE,

    LIR_FORWARD,

    /* error handling */
    LIR_POISON
} leaf_t;

/* lowered intermediate representation */
typedef struct lir_t {
    leaf_t leaf;

    /* the node that this ir originated from */
    node_t *node;

    /* the type this node resolved to */
    const type_t *type;

    union {
        /**
         * LIR_POISON
         * 
         */
        const char *msg;

        /**
         * LIR_DIGIT
         * 
         * integer literal
         */
        mpz_t digit;

        struct lir_t *it;

        struct {
            struct lir_t *dst;
            struct lir_t *src;
        };

        struct {
            binary_t binary;
            struct lir_t *lhs;
            struct lir_t *rhs;
        };

        struct {
            unary_t unary;
            struct lir_t *operand;
        };

        struct {
            struct lir_t *cond;
            struct lir_t *then;
            struct lir_t *other;
        };

        vector_t *stmts;

        struct {
            struct lir_t *func;
            vector_t *args;
        };

        struct {
            const char *name;

            union {
                /**
                 * LIR_EMPTY
                 * 
                 * a forward declared decl and the type its going to be
                 */
                struct {
                    leaf_t expected;
                    void *ctx;
                };

                /**
                 * LIR_VALUE
                 * 
                 * a value
                 */
                struct lir_t *init;

                /**
                 * LIR_DEFINE
                 * 
                 * a function
                 */
                struct {
                    const char *entry;
                    vector_t *locals;
                    struct lir_t *body;
                };
            };
        };

        /** 
         * LIR_MODULE 
         * 
         * vector of all global variables and procedures.
         */
        struct {
            vector_t *vars;
            vector_t *funcs;
        };
    };

    /* internal user data, frontends shouldnt touch this */
    void *data;
} lir_t;
 
lir_t *lir_forward(node_t *node, const char *name, leaf_t expected, void *ctx);
lir_t *lir_module(node_t *node, vector_t *vars, vector_t *funcs);
void add_module_var(lir_t *mod, lir_t *var);
void add_module_func(lir_t *mod, lir_t *func);

lir_t *lir_int(node_t *node, const type_t *type, int digit);
lir_t *lir_digit(node_t *node, const type_t *type, mpz_t digit);
lir_t *lir_name(node_t *node, const type_t *type, lir_t *it);

lir_t *lir_binary(node_t *node, const type_t *type, binary_t binary, lir_t *lhs, lir_t *rhs);
lir_t *lir_unary(node_t *node, const type_t *type, unary_t unary, lir_t *operand);
lir_t *lir_call(node_t *node, lir_t *func, vector_t *args);

lir_t *lir_assign(node_t *node, lir_t *dst, lir_t *src);
lir_t *lir_while(node_t *node, lir_t *cond, lir_t *then);
lir_t *lir_branch(node_t *node, lir_t *cond, lir_t *then, lir_t *other);
lir_t *lir_stmts(node_t *node, vector_t *stmts);

lir_t *lir_poison(node_t *node, const char *msg);

void lir_value(reports_t *reports, lir_t *dst, const type_t *type, lir_t *init);
void lir_define(reports_t *reports, lir_t *dst, const type_t *type, vector_t *locals, lir_t *body);
void lir_begin(reports_t *reports, lir_t *dst, leaf_t leaf);
bool lir_ok(const lir_t *lir);
bool lir_is(const lir_t *lir, leaf_t leaf);

vector_t *lir_recurses(lir_t *lir, const lir_t *root);
char *print_lir(const lir_t *lir);
const type_t *lir_type(const lir_t *lir);
