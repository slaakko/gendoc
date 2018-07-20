// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_NODE_INCLUDED
#define GENDOC_CPPAST_NODE_INCLUDED
#include <gendoc/parsing/Scanner.hpp>
#include <memory>

namespace gendoc { namespace cppast {

using gendoc::parsing::Span;

class Visitor;

class Node
{
public:
    Node(const Span& span_);
    virtual ~Node();
    virtual void Accept(Visitor& visitor) = 0;
    const Span& GetSpan() const { return span; }
    void SetSpanEnd(int32_t spanEnd) { span.SetEnd(spanEnd);  }
    virtual bool IsFunctionDeclaratorNode() const { return false; }
    virtual bool IsTemplateArgumentSequenceNode() const { return false; }
    virtual bool IsIdentifierNode() const { return false; }
    virtual bool IsOperatorFunctionIdNode() const { return false; }
    virtual bool IsConversionFunctionIdNode() const { return false; }
    virtual bool IsDtorIdNode() const { return false; }
    virtual bool IsTemplateIdNode() const { return false; }
private:
    Span span;
};

class UnaryNode : public Node
{
public:
    UnaryNode(const Span& span_);
    UnaryNode(const Span& span_, Node* child_);
    Node* Child() { return child.get(); }
    void SetChild(Node* child_) { child.reset(child_); }
    Node* ReleaseChild() { return child.release(); }
private:
    std::unique_ptr<Node> child;
};

class BinaryNode : public Node
{
public:
    BinaryNode(const Span& span_, Node* left_, Node* right_);
    Node* Left() { return left.get(); }
    Node* Right() { return right.get(); }
private:
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_NODE_INCLUDED
