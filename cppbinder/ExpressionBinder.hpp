// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPBINDER_EXPRESSION_BINDER_INCLUDED
#define GENDOC_CPPBINDER_EXPRESSION_BINDER_INCLUDED
#include <gendoc/cppbinder/BoundSourceFile.hpp>
#include <gendoc/cppsym/SymbolTable.hpp>
#include <gendoc/cppsym/Scope.hpp>
#include <gendoc/cppast/Node.hpp>

namespace gendoc { namespace cppbinder {

using namespace gendoc::cppast;
using namespace gendoc::cppsym;

std::vector<Symbol*> BindExpression(Node* node, SymbolTable& symbolTable, ContainerScope* containerScope, BoundSourceFile* boundSourceFile, FunctionSymbol* currentFunction);

} } // namespace gendoc::cppbinder

#endif // GENDOC_CPPBINDER_EXPRESSION_BINDER_INCLUDED
