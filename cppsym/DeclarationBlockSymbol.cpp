// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/DeclarationBlockSymbol.hpp>

namespace gendoc { namespace cppsym {

DeclarationBlockSymbol::DeclarationBlockSymbol(const Span& span_, const std::u32string& name_) : ContainerSymbol(span_, name_)
{
}

std::unique_ptr<dom::Element> DeclarationBlockSymbol::CreateElement()
{
    return std::unique_ptr<dom::Element>(new dom::Element(U"block"));
}

} } // namespace gendoc::cppsym
