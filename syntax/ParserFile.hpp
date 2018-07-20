#ifndef ParserFile_hpp_29186
#define ParserFile_hpp_29186

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/syntax/ParserFileContent.hpp>

namespace gendoc { namespace syntax {

class ParserFile : public gendoc::parsing::Grammar
{
public:
    static ParserFile* Create();
    static ParserFile* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    ParserFileContent* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, int id_, gendoc::parsing::ParsingDomain* parsingDomain_);
private:
    ParserFile(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ParserFileRule;
    class IncludeDirectivesRule;
    class IncludeDirectiveRule;
    class FileAttributeRule;
    class IncludeFileNameRule;
    class NamespaceContentRule;
    class NamespaceRule;
};

} } // namespace gendoc.syntax

#endif // ParserFile_hpp_29186
