#ifndef Primary_hpp_29186
#define Primary_hpp_29186

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/parsing/Parser.hpp>
#include <gendoc/parsing/Scope.hpp>

namespace gendoc { namespace syntax {

class Primary : public gendoc::parsing::Grammar
{
public:
    static Primary* Create();
    static Primary* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    gendoc::parsing::Parser* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Scope* enclosingScope);
private:
    Primary(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class PrimaryRule;
    class RuleCallRule;
    class NonterminalRule;
    class AliasRule;
    class GroupingRule;
    class TokenRule;
    class ExpectationRule;
    class ActionRule;
};

} } // namespace gendoc.syntax

#endif // Primary_hpp_29186
