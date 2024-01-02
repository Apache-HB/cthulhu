%define parse.error verbose
%define api.pure full
%define api.push-pull push
%lex-param { void *scan }
%parse-param { void *scan } { cpp_extra_t *x }
%locations
%expect 0
%define api.prefix {cpp}

%code top {
    #include "cpp/scan.h"
}

%code requires {
    #include "core/text.h"
    #include "std/vector.h"
    #include "cpp/scan.h"
    #include "base/log.h"
    #define YYSTYPE CPPSTYPE
    #define YYLTYPE CPPLTYPE
}

%{
int cpplex(void *yylval, void *yylloc, void *yyscanner);
void cpperror(where_t *where, void *state, cpp_extra_t *extra, const char *msg);
%}

%union {
    text_t text;
    char paste;

    vector_t *vec;

    cpp_number_t *number;

    void *ast;
}

%token<text>
    TOK_WHITESPACE "whitespace"
    TOK_BLOCK_COMMENT "block comment"
    TOK_IDENT "identifier"
    TOK_STRING "string literal"
    TOK_LINE_COMMENT

%token<number> TOK_NUMBER

%token<paste> TOK_PASTE

%token
    TOK_BEGIN_DIRECTIVE "#"
    TOK_END_INCLUDE
    TOK_END_DIRECTIVE

    /* preprocessor directives */
    TOK_INCLUDE "include"
    TOK_DEFINE "define"
    TOK_UNDEF "undef"
    TOK_IFDEF "ifdef"
    TOK_IFNDEF "ifndef"
    TOK_IF "if"
    TOK_ELIF "elif"
    TOK_ELSE "else"
    TOK_ENDIF "endif"
    TOK_ERROR "error"
    TOK_WARNING "warning"
    TOK_PRAGMA "pragma"
    TOK_LINE "line"

    TOK_DEFINED "defined"

    TOK_LPAREN "("
    TOK_RPAREN ")"

    TOK_PLUS
    TOK_MINUS
    TOK_MUL
    TOK_DIV
    TOK_MOD
    TOK_SHL
    TOK_SHR
    TOK_LT
    TOK_GT
    TOK_LTE
    TOK_GTE
    TOK_EQ
    TOK_NEQ
    TOK_AND
    TOK_OR
    TOK_XOR
    TOK_BITAND
    TOK_BITOR
    TOK_NOT
    TOK_BITNOT
    TOK_QUESTION
    TOK_COLON
    TOK_COMMA

%type<vec>
    directive_body
    expr_list opt_expr_list

%type<ast>
    condition

%start entry

%%

entry: %empty
    | entry item
    ;

item: TOK_IDENT { cpp_push_ident(x, $1); }
    | TOK_NUMBER { cpp_push_output(x, $1->text); }
    | TOK_STRING { cpp_push_output(x, $1); }
    | TOK_PASTE { cpp_push_output_single(x, $1); }
    | begin_directive ws any_directive { cpp_leave_directive(x); }
    | TOK_BLOCK_COMMENT { cpp_push_output(x, $1); }
    | TOK_WHITESPACE { cpp_push_output(x, $1); }
    | TOK_LINE_COMMENT { cpp_push_output(x, $1); }
    ;

begin_directive: TOK_BEGIN_DIRECTIVE { cpp_enter_directive(x); }
    ;

any_directive: directive end_directive
    | TOK_INCLUDE ws end_include
    ;

end_directive: TOK_END_DIRECTIVE
    | TOK_LINE_COMMENT { cpp_push_output(x, $1); }
    ;

end_include: TOK_END_INCLUDE
    | TOK_LINE_COMMENT { cpp_push_output(x, $1); }
    ;

directive: TOK_IDENT directive_body
    | TOK_DEFINE ws TOK_IDENT directive_body { cpp_add_define(x, @$, $3, $4); }
    | TOK_UNDEF ws TOK_IDENT ws { cpp_remove_define(x, @$, $3); }
    | TOK_IFDEF ws TOK_IDENT ws { cpp_ifdef(x, @$, $3); }
    | TOK_IFNDEF ws TOK_IDENT ws { cpp_ifndef(x, @$, $3); }
    | TOK_IF condition { cpp_if(x, @$); }
    | TOK_ELIF condition { cpp_elif(x, @$); }
    | TOK_ELSE ws { cpp_else(x, @$); }
    | TOK_ENDIF ws { cpp_endif(x, @$); }
    | TOK_ERROR directive_body
    | TOK_WARNING directive_body
    | TOK_PRAGMA directive_body
    | TOK_LINE directive_body
    ;

primary: TOK_LPAREN condition TOK_RPAREN
    | TOK_IDENT
    | TOK_IDENT TOK_LPAREN opt_expr_list TOK_RPAREN
    | TOK_NUMBER
    | TOK_PASTE
    ;

opt_expr_list: %empty { $$ = vector_of(0); }
    | expr_list { $$ = $1; }
    ;

expr_list: condition { $$ = vector_init($1); }
    | expr_list TOK_COMMA condition { vector_push(&$1, $3); $$ = $1; }
    ;

text: TOK_IDENT
    | TOK_PASTE
    ;

unary_expr: ws primary ws
    | ws TOK_MINUS unary_expr
    | ws TOK_PLUS unary_expr
    | ws TOK_NOT unary_expr
    | ws TOK_BITNOT unary_expr
    | ws TOK_DEFINED ws text
    | ws TOK_DEFINED ws TOK_LPAREN ws text ws TOK_RPAREN
    ;

mul_expr: unary_expr
    | mul_expr TOK_MUL unary_expr
    | mul_expr TOK_DIV unary_expr
    | mul_expr TOK_MOD unary_expr
    ;

add_expr: mul_expr
    | add_expr TOK_PLUS mul_expr
    | add_expr TOK_MINUS mul_expr
    ;

shift_expr: add_expr
    | shift_expr TOK_SHL add_expr
    | shift_expr TOK_SHR add_expr
    ;

compare_expr: shift_expr
    | compare_expr TOK_LT shift_expr
    | compare_expr TOK_GT shift_expr
    | compare_expr TOK_LTE shift_expr
    | compare_expr TOK_GTE shift_expr
    ;

equality_expr: compare_expr
    | equality_expr TOK_EQ compare_expr
    | equality_expr TOK_NEQ compare_expr
    ;

bitand_expr: equality_expr
    | bitand_expr TOK_BITAND equality_expr
    ;

xor_expr: bitand_expr
    | xor_expr TOK_XOR bitand_expr
    ;

ior_expr: xor_expr
    | ior_expr TOK_BITOR xor_expr
    ;

and_expr: ior_expr
    | and_expr TOK_AND ior_expr
    ;

or_expr: and_expr
    | or_expr TOK_OR and_expr
    ;

ternary: or_expr
    | or_expr TOK_QUESTION or_expr TOK_COLON ternary
    ;

condition: ternary { $$ = NULL; }
    ;

directive_body: %empty { $$ = vector_of(0); }
    | directive_body TOK_NUMBER { vector_push(&$1, cpp_token_new(x, TOK_NUMBER, &$2, @$)); $$ = $1; }
    | directive_body TOK_IDENT { vector_push(&$1, cpp_token_new(x, TOK_IDENT, &$2, @$)); $$ = $1; }
    | directive_body TOK_STRING { vector_push(&$1, cpp_token_new(x, TOK_STRING, &$2, @$)); $$ = $1; }
    | directive_body TOK_PASTE { vector_push(&$1, cpp_token_new(x, TOK_PASTE, &$2, @$)); $$ = $1; }
    | directive_body TOK_WHITESPACE { vector_push(&$1, cpp_token_new(x, TOK_WHITESPACE, &$2, @$)); $$ = $1; }
    | directive_body TOK_BLOCK_COMMENT { vector_push(&$1, cpp_token_new(x, TOK_BLOCK_COMMENT, &$2, @$)); $$ = $1; }
    | directive_body TOK_LPAREN
    | directive_body TOK_RPAREN
    | directive_body TOK_PLUS
    | directive_body TOK_MINUS
    | directive_body TOK_MUL
    | directive_body TOK_DIV
    | directive_body TOK_MOD
    | directive_body TOK_SHL
    | directive_body TOK_SHR
    | directive_body TOK_LT
    | directive_body TOK_GT
    | directive_body TOK_LTE
    | directive_body TOK_GTE
    | directive_body TOK_EQ
    | directive_body TOK_NEQ
    | directive_body TOK_AND
    | directive_body TOK_OR
    | directive_body TOK_XOR
    | directive_body TOK_BITAND
    | directive_body TOK_BITOR
    | directive_body TOK_NOT
    | directive_body TOK_BITNOT
    | directive_body TOK_QUESTION
    | directive_body TOK_COLON
    | directive_body TOK_COMMA
    ;

ws: %empty
    | ws TOK_WHITESPACE
    | ws TOK_BLOCK_COMMENT
    ;

%%