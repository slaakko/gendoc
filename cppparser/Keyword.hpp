#ifndef Keyword_hpp_30831
#define Keyword_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>

namespace gendoc { namespace cppparser {

class Keyword : public gendoc::parsing::Grammar
{
public:
    static Keyword* Create();
    static Keyword* Create(gendoc::parsing::ParsingDomain* parsingDomain);
private:
    std::vector<std::u32string> keywords0;
    Keyword(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
};

} } // namespace gendoc.cppparser

#endif // Keyword_hpp_30831
