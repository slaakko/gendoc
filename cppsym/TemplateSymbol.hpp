// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_TEMPLATE_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_TEMPLATE_SYMBOL_INCLUDED
#include <gendoc/cppsym/TypeSymbol.hpp>

namespace gendoc { namespace cppsym {

class TemplateParameterSymbol : public TypeSymbol
{
public:
    TemplateParameterSymbol(const Span& span_, const std::u32string& name_);
    std::u32string KindStr() override { return U"templateParameter"; }
    std::unique_ptr<dom::Element> CreateElement() override;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_TEMPLATE_SYMBOL_INCLUDED
