#pragma once

#include "scan/node.h"

#include "std/vector.h"

#include "cthulhu/tree/ops.h"

#include <gmp.h>

typedef struct obr_t obr_t;

///
/// symbols
///

typedef enum obr_visibility_t {
    eObrVisPrivate,         ///< default (no suffix)
    eObrVisPublic,          ///< `*` suffix
    eObrVisPublicReadOnly   ///< `-` suffix
} obr_visibility_t;

typedef struct obr_symbol_t {
    scan_t *scan;
    where_t where;

    obr_visibility_t visibility;
    char *name;
} obr_symbol_t;

///
/// ast
///

typedef enum obr_kind_t {
    /* types */
    eObrTypeName,
    eObrTypeQual,
    eObrTypePointer,
    eObrTypeArray,
    eObrTypeRecord,
    eObrTypeSignature,

    /* exprs */
    eObrExprDigit,
    eObrExprString,

    eObrExprName,
    eObrExprField,

    eObrExprUnary,
    eObrExprBinary,
    eObrExprCompare,
    eObrExprIs,
    eObrExprIn,

    /* stmts */
    eObrStmtReturn,
    eObrStmtWhile,
    eObrStmtAssign,

    /* decls */
    eObrDeclVar,
    eObrDeclConst,
    eObrDeclType,
    eObrDeclProcedure,

    /* other */
    eObrField,
    eObrParam,
    eObrReceiver,

    /* modules */
    eObrModule,
    eObrImport
} obr_kind_t;

typedef struct obr_t {
    obr_kind_t kind;
    node_t *node;

    union {
        struct {
            union {
                /* eObrExprField */
                char *field;

                /* eObrExprUnary */
                unary_t unary;
            };

            /* eObrStmtReturn */
            obr_t *expr;
        };

        /* eObrStmtWhile */
        struct {
            obr_t *cond;
            vector_t *then;
        };

        /* eObrExprBinary|eObrExprCompare|eObrExprIn|eObrExprIs */
        struct {
            union {
                compare_t compare;
                binary_t binary;
            };

            obr_t *lhs;
            obr_t *rhs;
        };

        /* eObrStmtAssign */
        struct {
            obr_t *dst;
            obr_t *src;
        };

        /* eObrTypePointer */
        obr_t *pointer;

        /* eObrTypeArray */
        obr_t *array;

        /* eObrTypeRecord */
        vector_t *fields;

        /* eObrExprDigit */
        mpz_t digit;

        /* eObrExprName */
        char *object;

        struct {
            char *name;
            obr_visibility_t visibility;

            union {
                /* eObrModule */
                struct {
                    vector_t *imports;
                    vector_t *decls;
                };

                /* eObrDeclVar|eObrDeclType|eObrReceiver|eObrParam */
                struct {
                    bool mut;
                    obr_t *type;
                };

                /* eObrDeclConst */
                obr_t *value;

                /* eObrDeclProcedure|eObrTypeSignature */
                struct {
                    obr_t *receiver;
                    vector_t *params;
                    obr_t *result;

                    vector_t *locals;
                    vector_t *body;
                };

                /* eObrImport|eObrTypeQual */
                char *symbol;
            };
        };
    };
} obr_t;

/* modules */

obr_t *obr_module(scan_t *scan, where_t where, char *name, char *end, vector_t *imports, vector_t *decls);
obr_t *obr_import(scan_t *scan, where_t where, char *name, char *symbol);

/* decls */

obr_t *obr_decl_type(scan_t *scan, where_t where, obr_symbol_t *symbol, obr_t *type);
obr_t *obr_decl_var(obr_symbol_t *symbol, obr_t *type);
obr_t *obr_decl_const(scan_t *scan, where_t where, obr_symbol_t *symbol, obr_t *value);

obr_t *obr_decl_procedure(
    scan_t *scan, where_t where, obr_symbol_t *symbol,
    obr_t *receiver, vector_t *params, obr_t *result,
    vector_t *locals, vector_t *body, char *end
);

/* exprs */

obr_t *obr_expr_name(scan_t *scan, where_t where, char *name);
obr_t *obr_expr_field(scan_t *scan, where_t where, obr_t *expr, char *field);

obr_t *obr_expr_is(scan_t *scan, where_t where, obr_t *lhs, obr_t *rhs);
obr_t *obr_expr_in(scan_t *scan, where_t where, obr_t *lhs, obr_t *rhs);

obr_t *obr_expr_compare(scan_t *scan, where_t where, compare_t op, obr_t *lhs, obr_t *rhs);
obr_t *obr_expr_binary(scan_t *scan, where_t where, binary_t op, obr_t *lhs, obr_t *rhs);
obr_t *obr_expr_unary(scan_t *scan, where_t where, unary_t op, obr_t *expr);

obr_t *obr_expr_digit(scan_t *scan, where_t where, const mpz_t digit);
obr_t *obr_expr_string(scan_t *scan, where_t where, char *text, size_t length);

/* stmts */

obr_t *obr_stmt_return(scan_t *scan, where_t where, obr_t *expr);
obr_t *obr_stmt_while(scan_t *scan, where_t where, obr_t *cond, vector_t *then);
obr_t *obr_stmt_assign(scan_t *scan, where_t where, obr_t *dst, obr_t *src);

/* types */

obr_t *obr_type_name(scan_t *scan, where_t where, char *symbol);
obr_t *obr_type_qual(scan_t *scan, where_t where, char *name, char *symbol);
obr_t *obr_type_pointer(scan_t *scan, where_t where, obr_t *type);
obr_t *obr_type_array(scan_t *scan, where_t where, obr_t *type);
obr_t *obr_type_record(scan_t *scan, where_t where, vector_t *fields);

/* extras */

obr_t *obr_field(obr_symbol_t *symbol, obr_t *type);
obr_t *obr_param(obr_symbol_t *symbol, obr_t *type, bool mut);
obr_t *obr_receiver(scan_t *scan, where_t where, bool mut, char *name, char *type);

/* partial symbols */

obr_symbol_t *obr_symbol(scan_t *scan, where_t where, char *name, obr_visibility_t visibility);

vector_t *obr_expand_vars(vector_t *symbols, obr_t *type);
vector_t *obr_expand_fields(vector_t *symbols, obr_t *type);
vector_t *obr_expand_params(vector_t *symbols, obr_t *type, bool mut);
