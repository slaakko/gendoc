#ifndef Expression_hpp_30831
#define Expression_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Expression.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Expression : public gendoc::parsing::Grammar
{
public:
    static Expression* Create();
    static Expression* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Expression(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ExpressionListRule;
    class PossiblyEmptyArgumentListRule;
    class ExpressionRule;
    class ConstantExpressionRule;
    class AssignmentExpressionRule;
    class ConcreteAssignmentExpressionRule;
    class AssignmentOperatorRule;
    class ConditionalExpressionRule;
    class ThrowExpressionRule;
    class LogicalOrExpressionRule;
    class LogicalAndExpressionRule;
    class InclusiveOrExpressionRule;
    class ExclusiveOrExpressionRule;
    class AndExpressionRule;
    class EqualityExpressionRule;
    class RelationalExpressionRule;
    class ShiftExpressionRule;
    class AdditiveExpressionRule;
    class MultiplicativeExpressionRule;
    class PMExpressionRule;
    class CastExpressionRule;
    class UnaryExpressionRule;
    class UnaryOperatorRule;
    class NewExpressionRule;
    class NewPlacementRule;
    class NewInitializerRule;
    class DeleteExpressionRule;
    class PostfixExpressionRule;
    class PrimaryExpressionRule;
    class CppCastExpressionRule;
    class TypeIdExpressionRule;
    class IdExpressionRule;
    class UnqualifiedIdRule;
    class QualifiedIdRule;
    class OperatorFunctionIdRule;
    class OperatorRule;
    class ConversionFunctionIdRule;
};

} } // namespace gendoc.cppparser

#endif // Expression_hpp_30831
