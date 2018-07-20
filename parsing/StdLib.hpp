#ifndef StdLib_hpp_28794
#define StdLib_hpp_28794

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <stdint.h>

namespace gendoc { namespace parsing {

class stdlib : public gendoc::parsing::Grammar
{
public:
    static stdlib* Create();
    static stdlib* Create(gendoc::parsing::ParsingDomain* parsingDomain);
private:
    stdlib(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class intRule;
    class uintRule;
    class longRule;
    class ulongRule;
    class hexuintRule;
    class hexRule;
    class hex_literalRule;
    class realRule;
    class urealRule;
    class numRule;
    class boolRule;
    class identifierRule;
    class qualified_idRule;
    class escapeRule;
    class charRule;
    class stringRule;
};

} } // namespace gendoc.parsing

#endif // StdLib_hpp_28794
