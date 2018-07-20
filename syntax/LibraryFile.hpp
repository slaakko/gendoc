#ifndef LibraryFile_hpp_29186
#define LibraryFile_hpp_29186

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/parsing/ParsingDomain.hpp>

namespace gendoc { namespace syntax {

class LibraryFile : public gendoc::parsing::Grammar
{
public:
    static LibraryFile* Create();
    static LibraryFile* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::ParsingDomain* parsingDomain);
private:
    LibraryFile(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class LibraryFileRule;
    class NamespaceContentRule;
    class NamespaceRule;
    class GrammarRule;
    class GrammarContentRule;
    class RuleRule;
};

} } // namespace gendoc.syntax

#endif // LibraryFile_hpp_29186
