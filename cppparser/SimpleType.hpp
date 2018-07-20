#ifndef SimpleType_hpp_30831
#define SimpleType_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/SimpleType.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class SimpleType : public gendoc::parsing::Grammar
{
public:
    static SimpleType* Create();
    static SimpleType* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    SimpleTypeNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    SimpleType(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SimpleTypeRule;
    class SimpleTypeSpecifierRule;
};

} } // namespace gendoc.cppparser

#endif // SimpleType_hpp_30831
