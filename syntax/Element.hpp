#ifndef Element_hpp_15542
#define Element_hpp_15542

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/parsing/Scope.hpp>
#include <gendoc/parsing/Grammar.hpp>

namespace gendoc { namespace syntax {

class Element : public gendoc::parsing::Grammar
{
public:
    static Element* Create();
    static Element* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Grammar* grammar);
private:
    std::vector<std::u32string> keywords0;
    Element(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class RuleLinkRule;
    class SignatureRule;
    class ParameterListRule;
    class VariableRule;
    class ParameterRule;
    class ReturnTypeRule;
    class IdentifierRule;
    class QualifiedIdRule;
    class StringArrayRule;
};

} } // namespace gendoc.syntax

#endif // Element_hpp_15542
