// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_NAMESPACE_INCLUDED
#define GENDOC_CPPAST_NAMESPACE_INCLUDED
#include <gendoc/cppast/Declaration.hpp>

namespace gendoc { namespace cppast {

class NamespaceNode : public UnaryNode
{
public:
    NamespaceNode(const Span& span_);
    NamespaceNode(const Span& span_, const std::u32string& namespaceName_);
    void Accept(Visitor& visitor) override;
    const std::u32string& NamespaceName() const { return namespaceName; }
    void AddMember(const Span& span, Node* member);
private:
    std::u32string namespaceName;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_NAMESPACE_INCLUDED
