// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/ParameterSymbol.hpp>
#include <gendoc/cppsym/FunctionSymbol.hpp>
#include <gendoc/util/Unicode.hpp>

namespace gendoc { namespace cppsym {

using namespace gendoc::unicode;

ParameterSymbol::ParameterSymbol(const Span& span_, const std::u32string& name_) : Symbol(span_, name_), type(nullptr)
{
}

std::u32string ParameterSymbol::IdStr()
{
    std::u32string idStr = Symbol::IdStr();
    if (Parent()->IsFunctionSymbol())
    {
        FunctionSymbol* function = static_cast<FunctionSymbol*>(Parent());
        std::u32string functionIndexStr = ToUtf32(std::to_string(function->Index()));
        idStr.append(1, '.').append(functionIndexStr);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    return idStr;
}

std::unique_ptr<dom::Element> ParameterSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> parameterElement(new dom::Element(U"parameter"));
    parameterElement->SetAttribute(U"type", type->Id());
    return parameterElement;
}

} } // namespace gendoc::cppsym
