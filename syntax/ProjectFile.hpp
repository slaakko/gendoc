#ifndef ProjectFile_hpp_29186
#define ProjectFile_hpp_29186

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/syntax/Project.hpp>

namespace gendoc { namespace syntax {

class ProjectFile : public gendoc::parsing::Grammar
{
public:
    static ProjectFile* Create();
    static ProjectFile* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    Project* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    ProjectFile(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ProjectFileRule;
    class ProjectFileContentRule;
    class SourceRule;
    class ReferenceRule;
    class FilePathRule;
};

} } // namespace gendoc.syntax

#endif // ProjectFile_hpp_29186
