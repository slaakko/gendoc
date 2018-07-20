// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_CONTAINER_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_CONTAINER_SYMBOL_INCLUDED
#include <gendoc/cppsym/Symbol.hpp>
#include <gendoc/cppsym/Scope.hpp>

namespace gendoc { namespace cppsym {

class FunctionGroupSymbol;
class ClassGroupSymbol;

struct ByName
{
    bool operator()(Symbol* left, Symbol* right) const
    {
        return left->NameBefore(right);
    }
};

class ContainerSymbol : public Symbol
{
public:
    ContainerSymbol(const Span& span_, const std::u32string& name_);
    virtual void AddMember(std::unique_ptr<Symbol>&& member);
    ContainerScope* GetContainerScope() override { return &containerScope; }
    bool IsContainerSymbol() const override { return true; }
    virtual int NumBases() const { return 0; }
    virtual ContainerSymbol* GetBase(int i) const { return nullptr; }
    std::unique_ptr<dom::Element> Namespaces() override;
    std::unique_ptr<dom::Element> Classes() override;
    std::unique_ptr<dom::Element> Constructors() override;
    std::unique_ptr<dom::Element> Functions() override;
    std::unique_ptr<dom::Element> EnumTypes() override;
    std::unique_ptr<dom::Element> Enumerators() override;
    std::unique_ptr<dom::Element> Typedefs() override;
    std::unique_ptr<dom::Element> Variables() override;
    std::unique_ptr<dom::Element> Grammars() override;
private:
    ContainerScope containerScope;
    std::vector<std::unique_ptr<Symbol>> members;
    FunctionGroupSymbol* MakeFunctionGroupSymbol(const Span& span_, const std::u32string& groupName);
    ClassGroupSymbol* MakeClassGroupSymbol(const Span& span_, const std::u32string& groupName);
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_CONTAINER_SYMBOL_INCLUDED
