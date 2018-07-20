#ifndef Identifier_hpp_30831
#define Identifier_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Expression.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>

namespace gendoc { namespace cppparser {

class Identifier : public gendoc::parsing::Grammar
{
public:
    static Identifier* Create();
    static Identifier* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    std::u32string Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Identifier(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class IdentifierRule;
    class IdNodeRule;
    class NestedNameSpecifierRule;
    class QualifiedIdNodeRule;
};

} } // namespace gendoc.cppparser

#endif // Identifier_hpp_30831
