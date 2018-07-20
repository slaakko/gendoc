// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_NAMESPACE_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_NAMESPACE_SYMBOL_INCLUDED
#include <gendoc/cppsym/ContainerSymbol.hpp>
#include <set>

namespace gendoc { namespace cppsym {

class NamespaceSymbol : public ContainerSymbol
{
public:
    NamespaceSymbol(const Span& span_, const std::u32string& name_);
    std::u32string KindStr() override { return U"namespace"; }
    std::unique_ptr<dom::Element> CreateElement() override;
    bool IsNamespaceSymbol() const override { return true; }
    bool IsParentSymbol() const override { return true; }
    void AddProject(const std::u32string& projectName);
private:
    std::set<std::u32string> projects;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_NAMESPACE_SYMBOL_INCLUDED
