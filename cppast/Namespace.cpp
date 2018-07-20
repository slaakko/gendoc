// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppast/Namespace.hpp>
#include <gendoc/cppast/Visitor.hpp>

namespace gendoc { namespace cppast {

NamespaceNode::NamespaceNode(const Span& span_) : UnaryNode(span_), namespaceName()
{
}

NamespaceNode::NamespaceNode(const Span& span_, const std::u32string& namespaceName_) : UnaryNode(span_), namespaceName(namespaceName_)
{
}

void NamespaceNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void NamespaceNode::AddMember(const Span& span, Node* member)
{
    if (Child())
    {
        SetChild(new DeclarationSequenceNode(span, ReleaseChild(), member));
    }
    else
    {
        SetChild(member);
    }
}

} } // namespace gendoc::cppast
