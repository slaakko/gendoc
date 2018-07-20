#ifndef Identifier_hpp_29048
#define Identifier_hpp_29048

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>

namespace gendoc { namespace code {

class Identifier : public gendoc::parsing::Grammar
{
public:
    static Identifier* Create();
    static Identifier* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    std::u32string Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Identifier(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class IdentifierRule;
    class QualifiedIdRule;
};

} } // namespace gendoc.code

#endif // Identifier_hpp_29048
