#ifndef IncludeDirectiveGrammar_hpp_30831
#define IncludeDirectiveGrammar_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/SourceFile.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class IncludeDirective : public gendoc::parsing::Grammar
{
public:
    static IncludeDirective* Create();
    static IncludeDirective* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, SourceFileNode* sourceFileNode);
private:
    IncludeDirective(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SourceFileRule;
    class SourceLineRule;
    class IncludeDirectiveLineRule;
    class FilePathRule;
};

} } // namespace gendoc.cppparser

#endif // IncludeDirectiveGrammar_hpp_30831
