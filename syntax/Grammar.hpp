#ifndef Grammar_hpp_29186
#define Grammar_hpp_29186

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/ParsingDomain.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;
class GrammarGrammar : public gendoc::parsing::Grammar
{
public:
    static GrammarGrammar* Create();
    static GrammarGrammar* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    gendoc::parsing::Grammar* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Scope* enclosingScope, ParsingDomain* parsingDomain);
private:
    GrammarGrammar(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class GrammarRule;
    class GrammarContentRule;
    class StartClauseRule;
    class SkipClauseRule;
};

} } // namespace gendoc.syntax

#endif // Grammar_hpp_29186
