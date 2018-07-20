#ifndef Statement_hpp_30831
#define Statement_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Statement.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Statement : public gendoc::parsing::Grammar
{
public:
    static Statement* Create();
    static Statement* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Statement(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class StatementRule;
    class LabeledStatementRule;
    class ExpressionStatementRule;
    class CompoundStatementRule;
    class SelectionStatementRule;
    class ConditionRule;
    class IterationStatementRule;
    class ForInitStatementRule;
    class ForRangeDeclarationRule;
    class ForRangeInitializerRule;
    class JumpStatementRule;
    class DeclarationStatementRule;
    class TryBlockRule;
    class HandlerSequenceRule;
    class HandlerRule;
    class ExceptionDeclarationRule;
};

} } // namespace gendoc.cppparser

#endif // Statement_hpp_30831
