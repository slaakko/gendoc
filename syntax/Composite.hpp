#ifndef Composite_hpp_29186
#define Composite_hpp_29186

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/parsing/Scope.hpp>
#include <gendoc/parsing/Parser.hpp>

namespace gendoc { namespace syntax {

class Composite : public gendoc::parsing::Grammar
{
public:
    static Composite* Create();
    static Composite* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    gendoc::parsing::Parser* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Scope* enclosingScope);
private:
    Composite(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class AlternativeRule;
    class SequenceRule;
    class DifferenceRule;
    class ExclusiveOrRule;
    class IntersectionRule;
    class ListRule;
    class PostfixRule;
};

} } // namespace gendoc.syntax

#endif // Composite_hpp_29186
