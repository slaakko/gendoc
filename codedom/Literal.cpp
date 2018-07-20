// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/codedom/Literal.hpp>
#include <gendoc/codedom/Visitor.hpp>

namespace gendoc { namespace codedom {

Literal::Literal(const std::u32string& name_): CppObject(name_)
{
}

void Literal::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace gendoc::codedom
