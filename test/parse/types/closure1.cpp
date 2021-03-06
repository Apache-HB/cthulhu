#include "tstream.hpp"
#include "tlexer.hpp"
#include "tparse.hpp"

int main() {
    auto stream = StringStream("() -> void");
    auto lexer = TestLexer(&stream);
    auto parse = TestParser(&lexer);

    vec<ptr<NameType>> names = {
        MAKE<NameType>(
            MAKE<Ident>(
                Token(Token::IDENT, { .ident = lexer.idents.intern("void") })
            )
        )
    };

    parse.expect(
        [&]{ return parse.parseType(); }, 
        MAKE<ClosureType>(vec<ptr<Type>>(), MAKE<QualifiedType>(names))
    );

    parse.finish();
}
