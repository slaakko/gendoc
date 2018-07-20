#ifndef Enumeration_hpp_30831
#define Enumeration_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Enumeration.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Enumeration : public gendoc::parsing::Grammar
{
public:
    static Enumeration* Create();
    static Enumeration* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    EnumTypeNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Enumeration(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class EnumDeclarationRule;
    class OpaqueEnumDeclarationRule;
    class EnumSpecifierRule;
    class EnumHeadRule;
    class EnumKeyRule;
    class EnumNameRule;
    class EnumBaseRule;
    class EnumeratorsRule;
    class EnumeratorDefinitionRule;
    class EnumeratorRule;
};

} } // namespace gendoc.cppparser

#endif // Enumeration_hpp_30831
