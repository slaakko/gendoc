#ifndef ParserTokenGrammar_hpp_4219
#define ParserTokenGrammar_hpp_4219

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/SourceCodeWriter.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class ParserToken : public gendoc::parsing::Grammar
{
public:
    static ParserToken* Create();
    static ParserToken* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, SourceCodeWriter* writer);
private:
    ParserToken(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SourceLineRule;
    class SourceTokenRule;
    class CharSetRule;
    class CharSetRangeRule;
    class CharSetCharRule;
};

} } // namespace gendoc.cppparser

#endif // ParserTokenGrammar_hpp_4219
