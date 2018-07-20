#ifndef Declarator_hpp_29048
#define Declarator_hpp_29048

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/codedom/Declaration.hpp>

namespace gendoc { namespace code {

class Declarator : public gendoc::parsing::Grammar
{
public:
    static Declarator* Create();
    static Declarator* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    gendoc::codedom::InitDeclaratorList* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Declarator(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class InitDeclaratorListRule;
    class InitDeclaratorRule;
    class DeclaratorRule;
    class DirectDeclaratorRule;
    class DeclaratorIdRule;
    class TypeIdRule;
    class TypeRule;
    class TypeSpecifierSeqRule;
    class AbstractDeclaratorRule;
    class DirectAbstractDeclaratorRule;
    class CVQualifierSeqRule;
    class InitializerRule;
    class InitializerClauseRule;
    class InitializerListRule;
};

} } // namespace gendoc.code

#endif // Declarator_hpp_29048
