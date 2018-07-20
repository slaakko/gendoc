// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/SimpleTypeSymbol.hpp>

namespace gendoc { namespace cppsym {

SimpleTypeSymbol::SimpleTypeSymbol(const Span& span_, const std::u32string& name_, const std::u32string& id_) : TypeSymbol(span_, name_), id(id_)
{
}

std::unique_ptr<dom::Element> SimpleTypeSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> typeElement(new dom::Element(U"simpleType"));
    return typeElement;
}

} } // namespace gendoc::cppsym
