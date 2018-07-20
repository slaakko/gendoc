#ifndef Template_hpp_30831
#define Template_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Template.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Template : public gendoc::parsing::Grammar
{
public:
    static Template* Create();
    static Template* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Template(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class TemplateDeclarationRule;
    class TemplateParameterListRule;
    class TemplateParameterRule;
    class TypeParameterRule;
    class SimpleTemplateIdRule;
    class TemplateIdRule;
    class TemplateNameRule;
    class TemplateArgumentListRule;
    class TemplateArgumentRule;
    class ExplicitInstantiationRule;
    class ExplicitSpecializationRule;
};

} } // namespace gendoc.cppparser

#endif // Template_hpp_30831
