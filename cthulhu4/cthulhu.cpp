#include "cthulhu.h"

#include <iostream>
#include <fstream>

#include <fmt/format.h>

namespace {
    auto grammar = R"(
        unit    <- import* decl* { no_ast_opt }

        # import syntax
        import  <- USING LIST(ident, COLON2) items? SEMI { no_ast_opt }
        items   <- '(' (CSV(ident) / DOT3) ')'  { no_ast_opt }

        # toplevel declarations
        decl        <- attribs* (alias / variant / union / record / func / var SEMI) { no_ast_opt }

        attribs     <- AT (attrib / '[' CSV(attrib) ']')
        attrib      <- qualified call?

        alias       <- USING ident '=' type SEMI
        union       <- UNION ident '{' fields? '}' { no_ast_opt }
        record      <- RECORD ident '{' fields? '}' { no_ast_opt }
        variant     <- VARIANT ident result? '{' CSV(case)? '}' { no_ast_opt }
        func        <- DEF ident fparams? result? body { no_ast_opt }
        fparams     <- '(' CSV(param)? ')'
        result      <- COLON type
        param       <- attribs* ident COLON (type / DOT3) init?
        body        <- (ASSIGN expr)? SEMI / compound
        var         <- VAR names init?
        names       <- qualified '(' CSV(ident) ')' / name / '[' CSV(name) ']'
        name        <- ident (COLON type)?
        init        <- ASSIGN expr

        # tagged union syntax
        case    <- ident data? ('=' expr)?
        data    <- '(' LIST(field, ',') ')'

        field       <- attribs* ident ':' type bitfield?
        bitfield    <- '[' LIST(bitrange, ',') ']'
        bitrange    <- expr (DOT2 expr)?
        fields      <- LIST(field, ',') { no_ast_opt }

        # type syntax
        type        <- attribs* (basictype error? / error)

        basictype   <- (pointer / array / closure / qualified / mutable)
        error       <- NOT (type / sum)?
        sum         <- '(' LIST(type, '/') ')'

        mutable     <- 'var' type
        array       <- '[' type (':' expr)? ']'
        pointer     <- '*' type { no_ast_opt }
        closure     <- '(' LIST(type, ',')? ')' '->' type
        qualified   <- LIST(ident, '::')

        # statements

        stmt        <- compound / return / break / continue / while / if / guard / expr ';' / switch / assign / for / asm / raise
        for         <- 'for' range else?
        range       <- '(' names DOT2 expr ')' stmt / names DOT2 expr compound
        while       <- 'while' label? cond else?
        continue    <- 'continue' ';'
        break       <- 'break' ident? ';'
        return      <- 'return' expr? ';'
        compound    <- '{' stmt* '}'
        if          <- 'if' cond elif* else?
        elif        <- 'else' 'if' cond
        else        <- 'else' stmt
        label       <- ':' ident
        cond        <- expr compound / '(' expr ')' stmt / first condition? compound / '(' first ')' stmt
        first       <- 'var' names '=' expr
        guard       <- var compound? ';'
        switch      <- 'switch' match
        match       <- expr '{' branches '}' / '(' expr ')' branches / 'var' names '=' expr condition? '{' branches '}'
        branches    <- branch* default?
        branch      <- expr ':' stmt*
        default     <- 'else' ':' stmt*
        assign      <- expr asop expr ';'
        condition   <- 'when' expr
        raise       <- 'raise' expr ';'

        asop        <- < '=' / '+=' / '-=' / '/=' / '*=' / '%=' / '&=' / '|=' / '^=' / '<<=' / '>>=' >

        asm         <- 'asm' '{' opcode* '}'
        opcode      <- ident (LIST(operand, ',')? ';' / ':')
        operand     <- soperand (':' soperand)?
        soperand    <- expr / '[' expr ']' / DOT expr

        expr <- OP(bexpr)

        # expressions
        bexpr    <- prefix (binop prefix)* {
                precedence
                    L NOT
                    L || &&
                    L & |
                    L ^
                    L EQ NEQ
                    L < <= > >=
                    L << >>
                    L + -
                    L / % *
            }

        binop    <- < '+' !'=' 
                / '-' !'=' 
                / '*' !'=' 
                / '/' !'=' 
                / '%' !'=' 
                / '&&'
                / '||'
                / '&' !('=' / '&')
                / '|' !('=' / '|')
                / NEQ / NOT
                / EQ
                / '<<' !'=' 
                / '>>' !'=' / '<=' / '>=' / '<' / '>'
            >

        unop   <- < '!' / '+' / '-' / '*' / '&' >

        atom <- 'try'? (number / qualified / 'true' / 'false' / string / OP('(') expr OP(')') / lambda) postfix*

        prefix  <- atom / OP(unop) prefix / '{' LIST(arg, ',')? '}'
        postfix <- '[' expr ']' / DOT ident / OP('->') ident / call / ternary / 'as' type
        call    <- OP('(') LIST(arg, ',')? OP(')')
        ternary <- OP('?') expr? OP(':') expr
        arg     <-  (DOT ident ASSIGN)? expr
        lambda  <- '[' CSV(capture)? ']' fparams? lresult? lbody
        capture     <- '&'? qualified
        lresult     <- '->' type
        lbody       <- expr / compound

        # basic blocks
        number  <- < (base2 / base10 / base16) ident? > ~spacing

        base10  <- < [0-9]+ >
        base2   <- < '0b' [01]+ >
        base16  <- < '0x' [0-9a-fA-F]+ >

        string  <- < ['] CHARS([']) ['] / '"""' CHARS(["""]) '"""' / ["] CHARS(["]) ["] >
        CHARS(D)    <- (!D char)*
        char    <- '\\' [nrt'"\[\]\\] / !'\\' .

        # keywords
        ~RECORD     <- 'record'
        ~UNION      <- 'union'
        ~USING      <- 'using'
        ~DEF        <- 'def'
        ~VARIANT    <- 'variant'
        ~VAR        <- 'var'

        # reserved keywords
        ~LET        <- 'let'
        ~FINAL      <- 'final'
        ~COMPILE    <- 'compile'

        KEYWORD <- RECORD / UNION / USING / DEF / VARIANT / VAR
                    / LET / FINAL / COMPILE

        # symbols
        ~SEMI       <- ';'
        ~COMMA      <- ','
        ~COLON      <- ':' !':'
        ~COLON2     <- '::'
        ~DOT        <- '.' !'.'
        ~DOT2       <- '..' !'.'
        ~DOT3       <- '...'
        ~ASSIGN     <- '=' !'='
        ~EQ         <- '=='
        ~NEQ        <- '!='
        ~NOT        <- '!' !'='
        ~AT         <- '@'

        ident   <- !KEYWORD < [a-zA-Z_][a-zA-Z0-9_]* / '$' > 

        %whitespace <- spacing
        %word       <- ident

        spacing     <- (comment / space)*
        space       <- [ \t\r\n]
        comment     <- '#' (!line .)* line?
        line        <- [\r\n]+

        OP(I)       <- I ~spacing
        LIST(I, D)  <- I (D I)*
        CSV(I)      <- LIST(I, COMMA)
    )";
}

namespace cthulhu {

using namespace peg;
using namespace peg::udl;

namespace {
    // all compiled units
    std::unordered_set<std::shared_ptr<Context>> units;

    // create a builtin symbol
    /*std::shared_ptr<Symbol> builtin(const std::string& name) {
        return std::make_shared<Symbol>(Symbol::SCALAR, name);
    }

    // all builtin types
    std::unordered_set<std::shared_ptr<Symbol>> builtins = {
        builtin("char"), builtin("short"), builtin("int"), builtin("long"), builtin("isize"),
        builtin("uchar"), builtin("ushort"), builtin("uint"), builtin("ulong"), builtin("usize"),
        builtin("void"), builtin("bool"), builtin("str")
    };*/

    // all include directories
    std::vector<fs::path> dirs = {
        fs::current_path(),
        fs::current_path()/"lib"
    };

    // client provided handles
    Handles* handles;

    // our global parser instance
    peg::parser parse;
}

void init(Handles* h) {
    handles = h;

    parse.log = [](auto line, auto col, const auto& msg) {
        fmt::print("{}:{}: {}\n", line, col, msg);
    };

    if (!parse.load_grammar(grammar)) {
        throw std::runtime_error("failed to load grammar");
    }

    parse.enable_packrat_parsing();
    parse.enable_ast();
}

Context::Context(fs::path name, std::string source) : name(name), text(source) {
    if (!parse.parse(text, tree)) {
        throw std::runtime_error(fmt::format("failed to parse source `{}`", name.string()));
    }

    tree = parse.optimize_ast(tree);
}

void Context::includes() {
    auto path = [](std::shared_ptr<Ast> ast) {
        fs::path where;
        for (auto part : ast->nodes) {
            if (part->tag != "ident"_)
                break;
            
            where /= part->token;
        }

        return where;
    };

    auto items = [](std::shared_ptr<Ast> ast) {
        std::optional<std::vector<std::string>> out;

        if (ast->tag == "items"_) {
            std::vector<std::string> all;

            for (auto item : ast->nodes) {
                all.push_back(item->token_to_string());
            }

            out = all;
        }

        return out;
    };

    for (auto node : tree->nodes) {
        if (node->tag != "import"_)
            continue;

        auto body = path(node);
        auto list = items(node->nodes.back());

        include(body, list);
    }
}

void Context::include(const fs::path& path, const std::optional<std::vector<std::string>>& items) {
    auto mod = Context::open(path, name.parent_path());

    std::vector<std::string> vec;
    for (const auto& part : path) {
        vec.push_back(part.string());
    }

    submodules.push_back({ vec, items, mod });
}

std::shared_ptr<Context> Context::open(const fs::path& path, const fs::path& cwd) {
    auto read = [](fs::path it) {
        it.replace_extension("ct");

        for (auto unit : units) {
            if (unit->name == it) {
                return unit;
            }
        }

        if (auto text = handles->open(it); text) {
            auto mod = std::make_shared<Context>(it, text.value());
            units.insert(mod);
            mod->includes();
            return mod;
        }

        return std::shared_ptr<Context>();
    };

    // first search relative to `cwd`
    auto relative = cwd/path;
    if (auto mod = read(relative); mod) {
        return mod;
    }

    // then search in the include directories
    for (const auto& dir : dirs) {
        auto where = dir/path;
        if (auto mod = read(where); mod) {
            return mod;
        }
    }

    throw std::runtime_error(fmt::format("failed to import `{}`", path.string()));
}

}
