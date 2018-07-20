// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_DECLARATION_BLOCK_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_DECLARATION_BLOCK_SYMBOL_INCLUDED
#include <gendoc/cppsym/ContainerSymbol.hpp>

namespace gendoc { namespace cppsym {

class DeclarationBlockSymbol : public ContainerSymbol
{
public:
    DeclarationBlockSymbol(const Span& span_, const std::u32string& name_);
    std::u32string KindStr() override { return U"declaration_block"; }
    std::unique_ptr<dom::Element> CreateElement() override;
    bool IsDeclarationBlockSymbol() const override { return true; }
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_DECLARATION_BLOCK_SYMBOL_INCLUDED
