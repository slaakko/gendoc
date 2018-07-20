#ifndef Declarator_hpp_30831
#define Declarator_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Declarator.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>
#include <gendoc/cppast/Expression.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Declarator : public gendoc::parsing::Grammar
{
public:
    static Declarator* Create();
    static Declarator* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Declarator(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class InitDeclaratorRule;
    class DeclaratorRule;
    class InitializerRule;
    class BraceOrEqualInitializerRule;
    class BracedInitializerListRule;
    class InitializerClauseRule;
    class InitializerListRule;
};

} } // namespace gendoc.cppparser

#endif // Declarator_hpp_30831
