#ifndef Literal_hpp_29048
#define Literal_hpp_29048

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/codedom/Literal.hpp>

namespace gendoc { namespace code {

class Literal : public gendoc::parsing::Grammar
{
public:
    static Literal* Create();
    static Literal* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    gendoc::codedom::Literal* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Literal(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class LiteralRule;
    class IntegerLiteralRule;
    class CharacterLiteralRule;
    class CCharSequenceRule;
    class FloatingLiteralRule;
    class StringLiteralRule;
    class BooleanLiteralRule;
    class PointerLiteralRule;
};

} } // namespace gendoc.code

#endif // Literal_hpp_29048
