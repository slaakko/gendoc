#ifndef Class_hpp_30831
#define Class_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Class.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Class : public gendoc::parsing::Grammar
{
public:
    static Class* Create();
    static Class* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    ClassNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Class(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ClassDeclarationRule;
    class ForwardClassDeclarationRule;
    class ClassSpecifierRule;
    class MemberSpecificationsRule;
    class MemberSpecificationRule;
    class MemberDeclarationRule;
    class SpecialMemberFunctionDeclarationRule;
    class ClassHeadRule;
    class ClassKeyRule;
    class ClassNameRule;
    class ClassVirtSpecifiersRule;
    class ClassVirtSpecifierRule;
    class BaseClauseRule;
    class BaseClassSpecifierListRule;
    class BaseClassSpecifierRule;
    class BaseSpecifiersRule;
    class BaseSpecifierRule;
    class AccessSpecifierRule;
    class VirtPureSpecifiersRule;
    class VirtSpecifierRule;
    class PureSpecifierRule;
    class SpecialMemberFunctionDefinitionRule;
    class CtorInitializerRule;
    class MemberInitializerListRule;
    class MemberInitializerRule;
    class MemberInitializerIdRule;
};

} } // namespace gendoc.cppparser

#endif // Class_hpp_30831
