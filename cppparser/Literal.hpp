#ifndef Literal_hpp_30831
#define Literal_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Literal.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Literal : public gendoc::parsing::Grammar
{
public:
    static Literal* Create();
    static Literal* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    LiteralNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Literal(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class LiteralRule;
    class FloatingLiteralRule;
    class FloatingSuffixRule;
    class IntegerLiteralRule;
    class DecimalLiteralRule;
    class OctalLiteralRule;
    class HexadecimalLiteralRule;
    class IntegerSuffixRule;
    class CharacterLiteralRule;
    class CCharRule;
    class HexDigit4Rule;
    class HexDigit8Rule;
    class StringLiteralRule;
    class EncodingPrefixRule;
    class SCharRule;
    class BooleanLiteralRule;
    class PointerLiteralRule;
};

} } // namespace gendoc.cppparser

#endif // Literal_hpp_30831
