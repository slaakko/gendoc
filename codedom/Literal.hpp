// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CODEDOM_LITERAL_INCLUDED
#define GENDOC_CODEDOM_LITERAL_INCLUDED
#include <gendoc/codedom/Object.hpp>

namespace gendoc { namespace codedom {

class Literal: public CppObject
{
public:
    Literal(const std::u32string& name_);
    int Rank() const override { return 24; }
    void Accept(Visitor& visitor) override;
};

} } // namespace gendoc::codedom

#endif // GENDOC_CODEDOM_LITERAL_INCLUDED

