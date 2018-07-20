#ifndef Function_hpp_30831
#define Function_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Function.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Function : public gendoc::parsing::Grammar
{
public:
    static Function* Create();
    static Function* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Function(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class FunctionDefinitionRule;
    class ParameterListRule;
    class ParameterDeclarationRule;
    class FunctionBodyRule;
};

} } // namespace gendoc.cppparser

#endif // Function_hpp_30831
