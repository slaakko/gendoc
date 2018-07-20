#ifndef XPath_hpp_29787
#define XPath_hpp_29787

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/xml/XmlGrammar.hpp>
#include <gendoc/xpath/XPathExpr.hpp>

namespace gendoc { namespace xpath {

class XPath : public gendoc::parsing::Grammar
{
public:
    static XPath* Create();
    static XPath* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    XPathExpr* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    std::vector<std::u32string> keywords0;
    XPath(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ExprRule;
    class OrExprRule;
    class AndExprRule;
    class EqualityExprRule;
    class RelationalExprRule;
    class AdditiveExprRule;
    class MultiplicativeExprRule;
    class UnaryExprRule;
    class UnionExprRule;
    class PathExprRule;
    class FilterExprRule;
    class LocationPathRule;
    class AbsoluteLocationPathRule;
    class RelativeLocationPathRule;
    class AbbreviatedAbsoluteLocationPathRule;
    class StepRule;
    class AbbreviatedStepRule;
    class AxisSpecifierRule;
    class AxisNameRule;
    class AbbreviatedAxisSpecifierRule;
    class PredicateRule;
    class PredicateExprRule;
    class NodeTestRule;
    class NameTestRule;
    class NodeTypeRule;
    class PrimaryExprRule;
    class VariableReferenceRule;
    class LiteralRule;
    class NumberRule;
    class FunctionCallRule;
    class ArgumentRule;
    class FunctionNameRule;
    class QNameRule;
    class PrefixedNameRule;
    class UnprefixedNameRule;
    class PrefixRule;
    class LocalPartRule;
    class NCNameRule;
};

} } // namespace gendoc.xpath

#endif // XPath_hpp_29787
