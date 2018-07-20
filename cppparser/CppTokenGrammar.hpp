#ifndef CppTokenGrammar_hpp_30831
#define CppTokenGrammar_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/SourceCodeWriter.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class CppToken : public gendoc::parsing::Grammar
{
public:
    static CppToken* Create();
    static CppToken* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, SourceCodeWriter* writer);
private:
    CppToken(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SourceLineRule;
    class SourceTokenRule;
};

} } // namespace gendoc.cppparser

#endif // CppTokenGrammar_hpp_30831
