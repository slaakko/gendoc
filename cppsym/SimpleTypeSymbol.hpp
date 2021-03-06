// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_SIMPLE_TYPE_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_SIMPLE_TYPE_SYMBOL_INCLUDED
#include <gendoc/cppsym/TypeSymbol.hpp>

namespace gendoc { namespace cppsym {

class SimpleTypeSymbol : public TypeSymbol
{
public:
    SimpleTypeSymbol(const Span& span_, const std::u32string& name_, const std::u32string& id_);
    bool IsSimpleTypeSymbol() const override { return true; }
    std::unique_ptr<dom::Element> CreateElement() override;
    std::u32string Id() override { return id; }
private:
    std::u32string id;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_SIMPLE_TYPE_SYMBOL_INCLUDED
