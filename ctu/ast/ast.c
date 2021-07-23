#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#include "ctu/util/str.h"
#include "ctu/util/report.h"
#include "ctu/util/util.h"

#include "ctu/debug/ast.h"

static node_t *new_node(scanner_t *scanner, where_t where, ast_t kind) {
    node_t *node = ctu_malloc(sizeof(node_t));

    node->kind = kind;
    node->scanner = scanner;
    node->where = where;
    node->typeof = NULL;
    node->implicit = false;
    node->exported = false;
    node->mut = false;
    node->used = false;

    return node;
}

static node_t *new_decl(scanner_t *scanner, where_t where, ast_t kind, char *name) {
    node_t *decl = new_node(scanner, where, kind);
    decl->name = name;
    decl->local = NOT_LOCAL;
    return decl;
}

const char *get_decl_name(node_t *node) {
    switch (node->kind) {
    case AST_DECL_FUNC: case AST_DECL_VAR: case AST_DECL_PARAM:
    case AST_DECL_RECORD: case AST_DECL_FIELD:
        return node->name;

    default:
        reportf(LEVEL_INTERNAL, node, "node is not a declaration");
        return "not-a-decl";
    }
}

const char *get_symbol_name(node_t *node) {
    switch (node->kind) {
    case AST_SYMBOL:
        return node->ident;

    default:
        reportf(LEVEL_INTERNAL, node, "node is not a symbol");
        return "not-a-symbol";
    }
}

const char *get_resolved_name(node_t *node) {
    switch (node->kind) {
    case AST_TYPE:
        return node->nameof;

    default:
        return get_decl_name(node);
    }
}

const char *get_field_name(node_t *node) {
    switch (node->kind) {
    case AST_DECL_FIELD:
        return node->name;
    
    default:
        reportf(LEVEL_INTERNAL, node, "node is not a field");
        return "not-a-field";
    }
}

bool is_discard_name(const char *name) {
    return name[0] == '$';
}

type_t *raw_type(node_t *node) {
    return node->typeof;
}

type_t *get_type(node_t *node) {
    type_t *type = raw_type(node);
    
    return type == NULL
        ? new_unresolved(node)
        : type;
}

nodes_t *get_stmts(node_t *node) {
    ASSERT(node->kind == AST_STMTS)("node->kind != AST_STMTS when calling get_stmts");

    return node->stmts;
}

bool is_math_op(binary_t op) {
    switch (op) {
    case BINARY_ADD: case BINARY_SUB:
    case BINARY_DIV: case BINARY_MUL:
    case BINARY_REM: 
        return true;
    default:
        return false;
    }
}

bool is_comparison_op(binary_t op) {
    switch (op) {
    case BINARY_GT: case BINARY_GTE:
    case BINARY_LT: case BINARY_LTE:
        return true;
    default:
        return false;
    }
}

bool is_equality_op(binary_t op) {
    switch (op) {
    case BINARY_EQ: case BINARY_NEQ:
        return true;
    default:
        return false;
    }
}

bool is_deref(node_t *expr) { 
    return expr->kind == AST_UNARY && expr->unary == UNARY_DEREF;
}

bool is_access(node_t *expr) {
    return expr->kind == AST_ACCESS;
}

nodes_t *ast_append(nodes_t *list, node_t *node) {
    if (list->len + 1 >= list->size) {
        list->size += 4;
        list->data = ctu_realloc(list->data, sizeof(node_t*) * list->size);
    }
    list->data[list->len++] = node;
    return list;
}

nodes_t *ast_list(node_t *init) {
    nodes_t *nodes = ctu_malloc(sizeof(nodes_t));

    nodes->data = ctu_malloc(sizeof(node_t*) * 4);
    nodes->len = 0;
    nodes->size = 4;

    if (init)
        ast_append(nodes, init);

    return nodes;
}

node_t *ast_at(nodes_t *list, size_t idx) {
    ASSERT(idx < list->len)("indexing out of bounds (%zu > %zu)", idx, list->len);
    return list->data[idx];
}

node_t *ast_kind_at(nodes_t *list, size_t idx, ast_t kind) {
    node_t *node = ast_at(list, idx);
    ASSERT(node->kind == kind)("unexpected node `%d` at `%zu`", node->kind, idx);
    return node;
}

size_t ast_len(nodes_t *list) {
    return list->len;
}

symbol_t *ast_symbol_list(char *init) {
    symbol_t *symbol = ctu_malloc(sizeof(symbol_t));
    symbol->parts = NULL;
    symbol->len = 0;
    symbol->size = 0;
    return ast_symbol_append(symbol, init);
}

symbol_t *ast_symbol_append(symbol_t *it, char *name) {
    if (it->len + 1 >= it->size) {
        it->size += 4;
        it->parts = ctu_realloc(it->parts, sizeof(char*) * it->size);
    }
    it->parts[it->len++] = name;
    return it;
}

node_t *make_implicit(node_t *node) {
    node->implicit = true;
    return node;
}

node_t *make_exported(node_t *node) {
    node->exported = true;
    return node;
}

node_t *ast_build(scanner_t *scanner, nodes_t *imports, nodes_t *decls) {
    node_t *node = new_node(NULL, NOWHERE, AST_ROOT);

    node->file = scanner->path;
    node->imports = imports;
    node->decls = decls;

    return node;
}

static void add_integer_type(node_t *node, char *str) {
    while (isxdigit(*str)) {
        str++;
    }

    bool sign = true;
    if (*str == 'u') {
        *str = 0;
        sign = false;
        str += 1;
    }

    integer_t kind = INTEGER_INT;

    switch (*str) {
    case 't':
        /* char suffix */
        kind = INTEGER_CHAR;
        break;
    case 's':
        /* short suffix */
        kind = INTEGER_SHORT;
        break;
    case 'i':
        /* int suffix */
        kind = INTEGER_INT;
        break;
    case 'l':
        /* long suffix */
        kind = INTEGER_LONG;
        break;
    case 'z':
        /* size suffix */
        kind = INTEGER_SIZE;
        break;
    case 'p':
        /* ptr suffix */
        kind = INTEGER_INTPTR;
        break;
    case 'm':
        /* max suffix */
        kind = INTEGER_INTMAX;
        break;
    
    case '\0': break;

    default:
        assert("invalid suffix `%c`", *str);
        break;
    }

    *str = 0;

    node->sign = sign;
    node->integer = kind;
}

node_t *ast_digit(scanner_t *scanner, where_t where, char *digit, int base) {
    node_t *node = new_node(scanner, where, AST_DIGIT);

    add_integer_type(node, digit);

    if (mpz_init_set_str(node->num, digit, base)) {
        assert("failed to initialize mpz_t digit");
    }

    ctu_free(digit);

    sanitize_range(
        get_int_type(node->sign, node->integer),
        node->num,
        scanner,
        where
    );

    return node;
}

node_t *ast_bool(scanner_t *scanner, where_t where, bool boolean) {
    node_t *node = new_node(scanner, where, AST_BOOL);

    node->boolean = boolean;

    return node;
}

static char *escape_string(const char *str) {
    size_t len = strlen(str);
    char *out = ctu_malloc(len + 1);

    size_t idx = 0;
    size_t dst = 0;

    while (str[idx]) {
        char c = str[idx++];
        if (c == '\\') {
            char n = str[idx++];
            switch (n) {
            case '\\': out[dst++] = '\\'; break;
            case 'n': out[dst++] = '\n'; break;
            case 't': out[dst++] = '\t'; break;
            case 'r': out[dst++] = '\r'; break;
            case '"': out[dst++] = '"'; break;
            case '\'': out[dst++] = '\''; break;
            case '0': out[dst++] = '\0'; break;
            case 'a': out[dst++] = '\a'; break;
            case 'b': out[dst++] = '\b'; break;
            case 'e': out[dst++] = '\x1b'; break;
            case 'v': out[dst++] = '\v'; break;
            case 'f': out[dst++] = '\f'; break;
            default:
                reportf(LEVEL_ERROR, NULL, "invalid escape sequence `\\%c`", n);
                break;
            }
        } else {
            out[dst++] = c;
        }
    }

    return out;
}

node_t *ast_string(scanner_t *scanner, where_t where, char *string) {
    node_t *node = new_node(scanner, where, AST_STRING);

    char *in = string + 1;
    string[strlen(string) - 1] = '\0';

    node->string = escape_string(in);

    ctu_free(string);

    return node;
}

node_t *ast_symbol(scanner_t *scanner, where_t where, char *text) {
    node_t *node = new_node(scanner, where, AST_SYMBOL);

    node->ident = text;

    return node;
}

node_t *ast_pointer(scanner_t *scanner, where_t where, node_t *ptr) {
    node_t *node = new_node(scanner, where, AST_PTR);
    node->ptr = ptr;
    return node;
}

node_t *ast_unary(scanner_t *scanner, where_t where, unary_t unary, node_t *expr) {
    node_t *node = new_node(scanner, where, AST_UNARY);

    node->unary = unary;
    node->expr = expr;

    return node;
}

node_t *ast_binary(scanner_t *scanner, where_t where, binary_t binary, node_t *lhs, node_t *rhs) {
    node_t *node = new_node(scanner, where, AST_BINARY);

    node->binary = binary;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

node_t *ast_call(scanner_t *scanner, where_t where, node_t *body, nodes_t *args) {
    node_t *node = new_node(scanner, where, AST_CALL);

    node->expr = body;
    node->args = args;

    return node;
}

node_t *ast_cast(scanner_t *scanner, where_t where, node_t *expr, node_t *cast) {
    node_t *node = new_node(scanner, where, AST_CAST);

    node->expr = expr;
    node->cast = cast;

    return node;
}

node_t *ast_stmts(scanner_t *scanner, where_t where, nodes_t *stmts) {
    node_t *node = new_node(scanner, where, AST_STMTS);

    node->stmts = stmts;

    return node;
}

node_t *ast_return(scanner_t *scanner, where_t where, node_t *expr) {
    node_t *node = new_node(scanner, where, AST_RETURN);

    node->expr = expr;

    return node;
}

node_t *ast_branch(scanner_t *scanner, where_t where, node_t *cond, node_t *branch) {
    node_t *node = new_node(scanner, where, AST_BRANCH);

    node->cond = cond;
    node->branch = branch;
    node->next = NULL;

    return node;
}

node_t *add_branch(node_t *branch, node_t *next) {
    branch->next = next;
    return branch;
}

node_t *ast_assign(scanner_t *scanner, where_t where, node_t *dst, node_t *src) {
    node_t *node = new_node(scanner, where, AST_ASSIGN);

    node->dst = dst;
    node->src = src;

    return node;
}

node_t *ast_while(scanner_t *scanner, where_t where, node_t *cond, node_t *body) {
    node_t *node = new_node(scanner, where, AST_WHILE);

    node->cond = cond;
    node->next = body;

    return node;
}

node_t *ast_decl_func(
    scanner_t *scanner, where_t where, 
    char *name, nodes_t *params,
    node_t *result, node_t *body) {

    node_t *node = new_decl(scanner, where, AST_DECL_FUNC, name);

    node->params = params;
    node->result = result;
    node->body = body;

    return node;
}

node_t *ast_decl_param(scanner_t *scanner, where_t where, char *name, node_t *type) {
    node_t *node = new_decl(scanner, where, AST_DECL_PARAM, name);

    node->type = type;

    return node;
}

node_t *ast_decl_var(scanner_t *scanner, where_t where, bool mut, char *name, node_t *type, node_t *init) {
    node_t *node = new_decl(scanner, where, AST_DECL_VAR, name);

    node->mut = mut;
    node->type = type;
    node->init = init;

    return node;
}

node_t *ast_mut(scanner_t *scanner, where_t where, node_t *it) {
    node_t *node = new_node(scanner, where, AST_MUT);
    node->next = it;
    return node;
}

node_t *ast_type(const char *name) {
    node_t *node = new_node(NULL, NOWHERE, AST_TYPE);
    node->nameof = name;
    return node;
}

node_t *ast_access(scanner_t *scanner, where_t where, node_t *expr, char *name, bool indirect) {
    node_t *node = new_node(scanner, where, AST_ACCESS);
    
    node->target = expr;
    node->field = name;
    node->indirect = indirect;

    return node;
}

node_t *ast_decl_record(scanner_t *scanner, where_t where, char *name, nodes_t *fields) {
    node_t *node = new_decl(scanner, where, AST_DECL_RECORD, name);

    node->fields = fields;

    return node;
}

node_t *ast_field(scanner_t *scanner, where_t where, char *name, node_t *type) {
    node_t *node = new_node(scanner, where, AST_DECL_FIELD);

    node->name = name;
    node->ftype = type;

    return node;
}

node_t *ast_import(scanner_t *scanner, where_t where, symbol_t *path) {
    node_t *node = new_node(scanner, where, AST_DECL_IMPORT);

    node->path = path;

    return node;
}

void free_ast_list(nodes_t *list, bool free_items) {
    if (free_items) {
        for (size_t i = 0; i < ast_len(list); i++) {
            ctu_free(ast_at(list, i));
        }
    }

    ctu_free(list->data);
    ctu_free(list);
}

bool is_exported(node_t *node) {
    return node->exported;
}

void mark_used(node_t *node) {
    node->used = true;
}

bool is_used(node_t *node) {
    return node->used;
}

nodes_t *all_decls(node_t *root) {
    ASSERT(root->kind == AST_ROOT)("all_decls must be called on a root");
    return root->decls;

}

nodes_t *all_imports(node_t *root) {
    ASSERT(root->kind == AST_ROOT)("all_imports must be called on a root");
    return root->imports;
}

char *last_symbol(symbol_t *symbol) {
    return symbol->parts[symbol->len - 1];
}
