// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_SIMPLE_TYPE_INCLUDED
#define GENDOC_CPPAST_SIMPLE_TYPE_INCLUDED
#include <stdint.h>
#include <gendoc/cppast/Node.hpp>

namespace gendoc { namespace cppast {

enum class SimpleTypeSpecifier : uint8_t
{
    char_, char16, char32, wchar, bool_, short_, int_, long_, signed_, unsigned_, float_, double_, void_, auto_, max
};

std::u32string ToString(SimpleTypeSpecifier specifier);

class SimpleTypeNode : public Node
{
public:
    SimpleTypeNode(const Span& span_, const std::vector<SimpleTypeSpecifier>& specifiers_);
    void Accept(Visitor& visitor) override;
    const std::vector<SimpleTypeSpecifier>& Specifiers() const { return specifiers; }
    std::u32string Str();
    std::u32string Id();
private:
    std::vector<SimpleTypeSpecifier> specifiers;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_SIMPLE_TYPE_INCLUDED
