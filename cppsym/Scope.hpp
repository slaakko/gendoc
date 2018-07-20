// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_SCOPE_INCLUDED
#define GENDOC_CPPSYM_SCOPE_INCLUDED
#include <gendoc/cppast/Declaration.hpp>
#include <unordered_map>
#include <stdint.h>

namespace gendoc { namespace cppsym {

using namespace gendoc::cppast;

enum class ScopeLookup : uint8_t
{
    none = 0,
    this_ = 1 << 0,
    parent = 1 << 1,
    bases = 1 << 2,
    this_and_parent = this_ | parent,
    this_and_bases = this_ | bases,
    this_and_bases_and_parent = this_ | bases | parent
};

inline ScopeLookup operator&(ScopeLookup left, ScopeLookup right)
{
    return ScopeLookup(uint8_t(left) & uint8_t(right));
}

inline ScopeLookup operator|(ScopeLookup left, ScopeLookup right)
{
    return ScopeLookup(uint8_t(left) | uint8_t(right));
}

class Symbol;
class ContainerSymbol;

class Scope
{
public:
    virtual ~Scope();
    virtual Symbol* Lookup(const std::u32string& name) = 0;
    virtual Symbol* Lookup(const std::u32string& name, ScopeLookup scopeLookup) = 0;
    virtual Symbol* Lookup(const std::vector<std::u32string>& nameSequence, ScopeLookup scopeLoookup) = 0;
    Symbol* Lookup(Node* nameNode);
    Symbol* Lookup(Node* nameNode, ScopeLookup scopeLookup);
};

class ContainerScope : public Scope
{
public:
    ContainerScope(ContainerSymbol* container_);
    ContainerSymbol* Container() { return container; }
    ContainerSymbol* GlobalNs();
    void Install(Symbol* symbol);
    Symbol* Lookup(const std::u32string& name) override;
    Symbol* Lookup(const std::u32string& name, ScopeLookup scopeLookup) override;
    Symbol* Lookup(const std::vector<std::u32string>& nameSequence, ScopeLookup scopeLoookup) override;
private:
    ContainerSymbol* container;
    std::unordered_map<std::u32string, Symbol*> symbolMap;
};

class FileScope : public Scope
{
public:
    void Install(ContainerScope* containerScope, UsingDirectiveNode& usingDirectiveNode);
    void Install(ContainerScope* containerScope, UsingDeclarationNode& usingDeclarationNode);
    Symbol* Lookup(const std::u32string& name) override;
    Symbol* Lookup(const std::u32string& name, ScopeLookup scopeLookup) override;
    Symbol* Lookup(const std::vector<std::u32string>& nameSequence, ScopeLookup scopeLoookup) override;
private:
    std::vector<ContainerScope*> containerScopes;
    std::unordered_map<std::u32string, Symbol*> aliasSymbolMap;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_SCOPE_INCLUDED
