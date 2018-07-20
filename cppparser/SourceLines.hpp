#ifndef SourceLines_hpp_30831
#define SourceLines_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>

namespace gendoc { namespace cppparser {

class SourceLines : public gendoc::parsing::Grammar
{
public:
    static SourceLines* Create();
    static SourceLines* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    int Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    SourceLines(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SourceFileRule;
    class SourceLineRule;
};

} } // namespace gendoc.cppparser

#endif // SourceLines_hpp_30831
