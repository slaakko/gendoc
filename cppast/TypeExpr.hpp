// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_TYPE_EXPR_INCLUDED
#define GENDOC_CPPAST_TYPE_EXPR_INCLUDED
#include <gendoc/cppast/Specifier.hpp>
#include <gendoc/cppast/Node.hpp>

namespace gendoc { namespace cppast {

class ConstNode : public UnaryNode
{
public:
    ConstNode(const Span& span_, Node* subject_);
    void Accept(Visitor& visitor) override;
};

class VolatileNode : public UnaryNode
{
public:
    VolatileNode(const Span& span_, Node* subject_);
    void Accept(Visitor& visitor) override;
};

Node* CreatePrefixTypeExprNode(const Span& span_, Specifier cvSpecifiers, Node* typeExprNode);

class PointerNode : public UnaryNode
{
public:
    PointerNode(const Span& span_, Node* subject_);
    void Accept(Visitor& visitor) override;
};

class RValueRefNode : public UnaryNode
{
public:
    RValueRefNode(const Span& span_, Node* subject_);
    void Accept(Visitor& visitor) override;
};

class LValueRefNode : public UnaryNode
{
public:
    LValueRefNode(const Span& span_, Node* subject_);
    void Accept(Visitor& visitor) override;
};

class TypeExprNode : public UnaryNode
{
public:
    TypeExprNode(const Span& span_, Node* typeExpr_);
    void Accept(Visitor& visitor) override;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_TYPE_EXPR_INCLUDED
