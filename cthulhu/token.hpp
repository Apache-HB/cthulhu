#pragma once

#include "fwd.hpp"

#include <stddef.h>

namespace cthulhu {
    struct Range {
        Range();
        Range(Lexer* lexer);
        Range(Lexer* lexer, size_t offset, size_t line, size_t column, size_t length);

        Range to(const Range& end) const;

        Lexer* lexer;
        size_t offset;
        size_t line;
        size_t column;
        size_t length;
    };

    enum struct Key {
#define KEY(id, _) id,
#define OP(id, _) id,
#include "keys.inc"
    };

    struct Number {
        Number(size_t num, const utf8::string* suf);
        size_t number;
        const utf8::string* suffix;
    };

    union TokenData {
        Key key;
        const utf8::string* ident;
        const utf8::string* string;
        c32 letter;
        Number digit;
    };

    struct Token {
        enum Type {
            IDENT,
            KEY,
            STRING,
            INT,
            CHAR,
            END,
            INVALID
        };

        Token();
        Token(Range where, Type type, TokenData data);

        bool is(Type other) const;
        bool operator==(const Token& other) const;
        bool operator!=(const Token& other) const;

        Key key() const;
        const utf8::string* ident() const;
        const utf8::string* string() const;
        Number number() const;
        c32 letter() const;
        bool valid() const;

    private:
        Range where;
        Type type;
        TokenData data;
    };
}
