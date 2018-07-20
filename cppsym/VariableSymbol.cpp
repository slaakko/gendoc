// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/VariableSymbol.hpp>

namespace gendoc { namespace cppsym {

VariableSymbol::VariableSymbol(const Span& span_, const std::u32string& name_) : Symbol(span_, name_), specifiers(Specifier::none), type(nullptr)
{
}

void VariableSymbol::AddSpecifiers(Specifier specifiers_)
{
    specifiers = specifiers | specifiers_;
}

std::unique_ptr<dom::Element> VariableSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> variableElement(new dom::Element(U"variable"));
    variableElement->SetAttribute(U"type", type->Id());
    if (specifiers != Specifier::none)
    {
        variableElement->SetAttribute(U"specifiers", SpecifierStr(specifiers));
    }
    return variableElement;
}

} } // namespace gendoc::cppsym