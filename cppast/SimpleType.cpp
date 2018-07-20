// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppast/SimpleType.hpp>
#include <gendoc/cppast/Visitor.hpp>

namespace gendoc { namespace cppast {

const char32_t* simpleTypeSpecifierStr[uint8_t(SimpleTypeSpecifier::max)]
{
    U"char", U"char16_t", U"char32_t", U"wchar_t", U"bool", U"short", U"int", U"long", U"signed", U"unsigned", U"float", U"double", U"void", U"auto"
};

std::u32string ToString(SimpleTypeSpecifier specifier)
{
    return simpleTypeSpecifierStr[uint8_t(specifier)];
}

SimpleTypeNode::SimpleTypeNode(const Span& span_, const std::vector<SimpleTypeSpecifier>& specifiers_) : Node(span_), specifiers(specifiers_)
{
}

void SimpleTypeNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::u32string SimpleTypeNode::Str()
{
    std::u32string str;
    bool first = true;
    for (SimpleTypeSpecifier specifier : specifiers)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            str.append(U" ");
        }
        str.append(ToString(specifier));
    }
    return str;
}

std::u32string SimpleTypeNode::Id()
{
    std::u32string id;
    bool first = true;
    for (SimpleTypeSpecifier specifier : specifiers)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            id.append(U".");
        }
        id.append(ToString(specifier));
    }
    return id;
}

} } // namespace gendoc::cppast

