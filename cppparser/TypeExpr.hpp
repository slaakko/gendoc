#ifndef TypeExpr_hpp_30831
#define TypeExpr_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/TypeExpr.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>
#include <gendoc/cppast/Expression.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class TypeExpr : public gendoc::parsing::Grammar
{
public:
    static TypeExpr* Create();
    static TypeExpr* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    TypeExpr(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class TypeExprRule;
    class PrefixTypeExprRule;
    class CVSpecifierSequenceRule;
    class CVSpecifierRule;
    class PostfixTypeExprRule;
    class PrimaryTypeExprRule;
};

} } // namespace gendoc.cppparser

#endif // TypeExpr_hpp_30831
