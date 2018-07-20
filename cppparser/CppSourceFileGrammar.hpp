#ifndef CppSourceFileGrammar_hpp_30831
#define CppSourceFileGrammar_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class CppSourceFile : public gendoc::parsing::Grammar
{
public:
    static CppSourceFile* Create();
    static CppSourceFile* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, SourceFileNode* sourceFile);
private:
    CppSourceFile(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SourceFileRule;
    class DeclarationsRule;
};

} } // namespace gendoc.cppparser

#endif // CppSourceFileGrammar_hpp_30831
