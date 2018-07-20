// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/TemplateSymbol.hpp>

namespace gendoc { namespace cppsym {

TemplateParameterSymbol::TemplateParameterSymbol(const Span& span_, const std::u32string& name_) : TypeSymbol(span_, name_)
{
}

std::unique_ptr<dom::Element> TemplateParameterSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> templateParameter(new dom::Element(U"templateParameter"));
    return templateParameter;
}

} } // namespace gendoc::cppsym
