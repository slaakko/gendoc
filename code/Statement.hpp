#ifndef Statement_hpp_29048
#define Statement_hpp_29048

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/codedom/Statement.hpp>

namespace gendoc { namespace code {

class Statement : public gendoc::parsing::Grammar
{
public:
    static Statement* Create();
    static Statement* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    gendoc::codedom::CompoundStatement* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Statement(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class StatementRule;
    class LabeledStatementRule;
    class LabelRule;
    class EmptyStatementRule;
    class ExpressionStatementRule;
    class CompoundStatementRule;
    class SelectionStatementRule;
    class IfStatementRule;
    class SwitchStatementRule;
    class IterationStatementRule;
    class WhileStatementRule;
    class DoStatementRule;
    class ForStatementRule;
    class ForInitStatementRule;
    class JumpStatementRule;
    class BreakStatementRule;
    class ContinueStatementRule;
    class ReturnStatementRule;
    class GotoStatementRule;
    class GotoTargetRule;
    class DeclarationStatementRule;
    class ConditionRule;
    class TryStatementRule;
    class HandlerSeqRule;
    class HandlerRule;
    class ExceptionDeclarationRule;
};

} } // namespace gendoc.code

#endif // Statement_hpp_29048
