#ifndef Rule_hpp_29186
#define Rule_hpp_29186

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/parsing/Rule.hpp>

namespace gendoc { namespace syntax {

class Rule : public gendoc::parsing::Grammar
{
public:
    static Rule* Create();
    static Rule* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    gendoc::parsing::Rule* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Scope* enclosingScope);
private:
    Rule(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class RuleRule;
    class RuleHeaderRule;
    class RuleBodyRule;
};

} } // namespace gendoc.syntax

#endif // Rule_hpp_29186
