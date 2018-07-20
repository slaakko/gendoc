// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPBINDER_VIRTUAL_BINDER_INCLUDED
#define GENDOC_CPPBINDER_VIRTUAL_BINDER_INCLUDED
#include <gendoc/cppsym/ClassTypeSymbol.hpp>
#include <unordered_set>

using namespace gendoc::cppsym;

namespace gendoc { namespace cppbinder {

void ResolveOverrideSets(const std::unordered_set<ClassTypeSymbol*>& classes);

} } // namespace gendoc::cppbinder

#endif // GENDOC_CPPBINDER_VIRTUAL_BINDER_INCLUDED
