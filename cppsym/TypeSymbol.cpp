// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/TypeSymbol.hpp>

namespace gendoc {namespace cppsym {

TypeSymbol::TypeSymbol(const Span& span_, const std::u32string& name_) : ContainerSymbol(span_, name_)
{
}

std::unique_ptr<dom::Element> TypeSymbol::CreateElement()
{
    return std::unique_ptr<dom::Element>(new dom::Element(U"type"));
}

bool TypeSymbol::HasBaseClass(TypeSymbol* type, int& distance)
{
    return false;
}

int TypeSymbol::MatchValue(TypeSymbol* argumentType) 
{ 
    if (argumentType == nullptr) return 1;
    if (argumentType == this) return std::numeric_limits<int>::max();
    return 0; 
}

} } // namespace gendoc::cppsym
