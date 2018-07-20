#ifndef Declaration_hpp_30831
#define Declaration_hpp_30831

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/cppast/Namespace.hpp>
#include <gendoc/cppparser/ParsingContext.hpp>
#include <gendoc/cppparser/TypeExpr.hpp>
#include <gendoc/cppparser/Literal.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
class Declaration : public gendoc::parsing::Grammar
{
public:
    static Declaration* Create();
    static Declaration* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Declaration(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class DeclarationRule;
    class NamespaceDefinitionRule;
    class NamedNamespaceDefinitionRule;
    class UnnamedNamespaceDefinitionRule;
    class BlockDeclarationRule;
    class SimpleDeclarationRule;
    class DeclSpecifiersRule;
    class DeclSpecifierRule;
    class StorageClassSpecifierRule;
    class FunctionSpecifierRule;
    class AliasDeclarationRule;
    class UsingDirectiveRule;
    class UsingDeclarationRule;
    class TypedefDeclarationRule;
    class LinkageSpecificationRule;
};

} } // namespace gendoc.cppparser

#endif // Declaration_hpp_30831
