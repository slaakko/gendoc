#ifndef SolutionGrammar_hpp_31054
#define SolutionGrammar_hpp_31054

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Solution.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Solution : public gendoc::parsing::Grammar
{
public:
    static Solution* Create();
    static Solution* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    gendoc::cppast::Solution* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Solution(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SolutionRule;
    class SolutionHeaderRule;
    class ProjectsRule;
    class ProjectRule;
    class ProjectContentRule;
    class ProjectSectionRule;
    class ProjectDependencySectionRule;
    class ProjectDependencySectionContentRule;
    class DependentProjectRule;
    class GlobalRule;
    class GlobalSectionsRule;
    class FormatVersionRule;
    class ProgramVersionRule;
    class MajorVersionRule;
    class MinorVersionRule;
    class RevisionRule;
    class BuildNumberRule;
    class GuidStringRule;
    class GuidRule;
    class HexGuidRule;
    class FilePathRule;
};

} } // namespace gendoc.cppparser

#endif // SolutionGrammar_hpp_31054
