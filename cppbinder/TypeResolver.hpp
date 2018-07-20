// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPBINDER_TYPE_RESOLVER_INCLUDED
#define GENDOC_CPPBINDER_TYPE_RESOLVER_INCLUDED
#include <gendoc/cppbinder/BoundSourceFile.hpp>
#include <gendoc/cppsym/SymbolTable.hpp>
#include <gendoc/cppsym/TypeSymbol.hpp>
#include <gendoc/cppast/Node.hpp>

namespace gendoc { namespace cppbinder {

using namespace gendoc::cppsym;
using namespace gendoc::cppast;

enum class TypeResolverFlags : uint8_t
{
    none = 0,
    resolveClassGroup = 1 << 0,
    nothrow = 1 << 1,
    noExternalTypes = 1 << 2
};

inline TypeResolverFlags operator|(TypeResolverFlags left, TypeResolverFlags right)
{
    return TypeResolverFlags(uint8_t(left) | uint8_t(right));
}

inline TypeResolverFlags operator&(TypeResolverFlags left, TypeResolverFlags right)
{
    return TypeResolverFlags(uint8_t(left) & uint8_t(right));
}

TypeSymbol* ResolveType(SymbolTable& symbolTable, ContainerScope* containerScope, BoundSourceFile& boundSourceFile, Node* node);
TypeSymbol* ResolveType(SymbolTable& symbolTable, ContainerScope* containerScope, BoundSourceFile& boundSourceFile, TypeResolverFlags flags, Node* node);

} } // namespace gendoc::cppbinder

#endif // GENDOC_CPPBINDER_TYPE_RESOLVER_INCLUDED
