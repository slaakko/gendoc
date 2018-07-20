// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/ExternalTypeSymbol.hpp>

namespace gendoc { namespace cppsym {

ExternalTypeSymbol::ExternalTypeSymbol(const Span& span_, const std::u32string& name_, ClassKey classKey_, const std::u32string& id_) : 
    TypeSymbol(span_, name_), classKey(classKey_), id(id_)
{
}

std::unique_ptr<dom::Element> ExternalTypeSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> typeElement(new dom::Element(U"externalType"));
    return typeElement;
}

} } // namespace gendoc::cppsym
