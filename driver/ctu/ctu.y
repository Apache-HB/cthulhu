%define parse.error verbose
%define api.pure full
%lex-param { void *scan }
%parse-param { void *scan } { scan_t *x }
%locations
%expect 0
%define api.prefix {ctu}

%code top {
    #include "cthulhu/ast/interop.h"
}

%code requires {
    #define YYSTYPE CTUSTYPE
    #define YYLTYPE CTULTYPE
    
    #include "scan.h"
    #include "ast.h"
}

%{
#include "scan.h"
int ctulex();
void ctuerror(where_t *where, void *state, scan_t *scan, const char *msg);
%}

%union {
    bool boolean;
    char *ident;
    mpz_t mpz;

    ast_t *ast;
    vector_t *vector;
}

%token<ident>
    IDENT "identifier"

%token<mpz>
    INTEGER "integer literal"

%token<boolean>
    BOOLEAN "boolean literal"


%token
    AT "`@`"
    COLON "`:`"
    COLON2 "`::`"
    SEMICOLON "`;`"
    COMMA "`,`"
    DOT "`.`"
    DOT3 "`...`"
    ARROW "`->`"
    LSHIFT "`<<`"
    RSHIFT "`>>`"

    BEGIN_TEMPLATE "`!<` (template)"
    END_TEMPLATE "`>` (template)"

    NOT "`!`"
    EQUALS "`=`"
    EQ "`==`"
    NEQ "`!=`"

    ADD "`+`"
    SUB "`-`"
    MUL "`*`"
    DIV "`/`"
    MOD "`%`"

    AND "`&&`"
    OR "`||`"

    BITAND "`&`"
    BITOR "`|`"
    BITXOR "`^`"

    LPAREN "`(`"
    RPAREN "`)`"
    LSQUARE "`[`"
    RSQUARE "`]`"
    LBRACE "`{`"
    RBRACE "`}`"

    GT "`>`"
    GTE "`>=`"
    LT "`<`"
    LTE "`<=`"

    MODULE "`module`"
    IMPORT "`import`"
    EXPORT "`export`"

    DEF "`def`"
    VAR "`var`"
    FINAL "`final`"

    OBJECT "`object`"
    STRUCT "`struct`"
    UNION "`union`"
    VARIANT "`variant`"
    TYPE "`type`"

    IF "`if`"
    ELSE "`else`"
    WHILE "`while`"
    FOR "`for`"
    BREAK "`break`"
    CONTINUE "`continue`"
    RETURN "`return`"

    LAMBDA "`lambda`"
    AS "`as`"

    NIL "`null`"

    MATCH "`match`"
    DEFAULT "`default`"
    CASE "`case`"
    SELECT "`select`"
    ORDER "`order`"
    ON "`on`"
    FROM "`from`"
    WHERE "`where`"
    WHEN "`when`"
    THEN "`then`"

    IN "`in`"
    OUT "`out`"
    INOUT "`inout`"

    PRIVATE "`private`"
    PUBLIC "`public`"
    PROTECTED "`protected`"

    ASYNC "`async`"
    AWAIT "`await`"
    YIELD "`yield`"

    CONTRACT "`contract`"
    REQUIRES "`requires`"
    ASSERT "`assert`"
    ENSURES "`ensures`"
    INVARIANT "`invariant`"

    SIZEOF "`sizeof`"
    ALIGNOF "`alignof`"
    OFFSETOF "`offsetof`"
    TYPEOF "`typeof`"
    UUIDOF "`uuidof`"

    TRY "`try`"
    CATCH "`catch`"
    FINALLY "`finally`"
    THROW "`throw`"

    COMPILED "`compile`"
    STATIC "`static`"

%type<ast>
    modspec decl 
    structdecl uniondecl variantdecl aliasdecl
    field type types opttypes import
    expr primary variant

%type<vector>
    path decls decllist
    fieldlist aggregates
    typelist imports importlist
    variants variantlist

%start program

%%

program: modspec imports decls { scan_set(x, ast_program(x, @$, $1, $2, $3)); }
    ;

imports: %empty { $$ = vector_new(0); }
    | importlist { $$ = $1; }
    ;

importlist: import { $$ = vector_init($1); }
    | importlist import { vector_push(&$1, $2); $$ = $1; }
    ;

import: IMPORT path SEMICOLON { $$ = ast_import(x, @$, $2); }
    ;

decls: %empty { $$ = vector_new(0); }
    | decllist { $$ = $1; }
    ;

decllist: decl { $$ = vector_init($1); }
    | decllist decl { vector_push(&$1, $2); $$ = $1; }
    ;

decl: structdecl { $$ = $1; }
    | uniondecl { $$ = $1; }
    | aliasdecl { $$ = $1; }
    | variantdecl { $$ = $1; }
    ;

aliasdecl: TYPE IDENT EQUALS type SEMICOLON { $$ = ast_typealias(x, @$, $2, $4); }
    ;

structdecl: STRUCT IDENT aggregates { $$ = ast_structdecl(x, @$, $2, $3); }
    ;

uniondecl: UNION IDENT aggregates { $$ = ast_uniondecl(x, @$, $2, $3); }
    ;

variantdecl: VARIANT IDENT LBRACE variants RBRACE { $$ = ast_variantdecl(x, @$, $2, $4); }
    ;

variants: %empty { $$ = vector_new(0); }
    | variantlist { $$ = $1; }
    ;

variantlist: variant { $$ = vector_init($1); }
    | variantlist COMMA variant { vector_push(&$1, $3); $$ = $1; }
    ;

variant: IDENT { $$ = ast_field(x, @$, $1, NULL); }
    | IDENT LPAREN type RPAREN { $$ = ast_field(x, @$, $1, $3); }
    ;

aggregates: LBRACE fieldlist RBRACE { $$ = $2; }
    ;

fieldlist: field { $$ = vector_init($1); }
    | fieldlist field { vector_push(&$1, $2); $$ = $1; }
    ;

field: IDENT COLON type SEMICOLON { $$ = ast_field(x, @$, $1, $3); }
    ;

modspec: %empty { $$ = NULL; }
    | MODULE path SEMICOLON { $$ = ast_module(x, @$, $2); }
    ;

type: path { $$ = ast_typename(x, @$, $1); }
    | MUL type { $$ = ast_pointer(x, @$, $2, false); }
    | LSQUARE MUL RSQUARE type { $$ = ast_pointer(x, @$, $4, true); }
    | LSQUARE expr RSQUARE type { $$ = ast_array(x, @$, $2, $4); }
    | DEF LPAREN opttypes RPAREN ARROW type { $$ = ast_closure(x, @$, $3, $6); } 
    | LPAREN type RPAREN { $$ = $2; }
    ;

opttypes: %empty { $$ = ast_typelist(vector_of(0), false); }
    | types { $$ = $1; }
    ;

types: typelist { $$ = ast_typelist($1, false); }
    | typelist COMMA DOT3 { $$ = ast_typelist($1, true); }
    ;

typelist: type { $$ = vector_init($1); }
    | typelist COMMA type { vector_push(&$1, $3); $$ = $1; }
    ;

primary: LPAREN expr RPAREN { $$ = $2; }
    | INTEGER { $$ = ast_digit(x, @$, $1); }
    | path { $$ = ast_name(x, @$, $1); }
    ;

expr: primary { $$ = $1; }
    ;

path: IDENT { $$ = vector_init($1); }
    | path COLON2 IDENT { vector_push(&$1, $3); $$ = $1; }
    ;

%%
