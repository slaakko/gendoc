#ifndef Keyword_hpp_29048
#define Keyword_hpp_29048

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>

namespace gendoc { namespace code {

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

} } // namespace gendoc.code

#endif // Keyword_hpp_29048
