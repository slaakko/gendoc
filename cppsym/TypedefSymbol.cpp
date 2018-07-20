// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/TypedefSymbol.hpp>

namespace gendoc { namespace cppsym {

TypedefSymbol::TypedefSymbol(const Span& span_, const std::u32string& name_) : TypeSymbol(span_, name_), type(nullptr)
{
}

std::unique_ptr<dom::Element> TypedefSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> typeElement(new dom::Element(U"typedef"));
    typeElement->SetAttribute(U"type", type->Id());
    return typeElement;
}

} } // namespace gendoc::cppsym
