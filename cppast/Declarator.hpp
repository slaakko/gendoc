// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_DECLARATOR_INCLUDED
#define GENDOC_CPPAST_DECLARATOR_INCLUDED
#include <gendoc/cppast/Node.hpp>
#include <gendoc/cppast/Specifier.hpp>

namespace gendoc { namespace cppast {

class IdDeclaratorNode : public Node
{
public:
    IdDeclaratorNode(const Span& span_, Node* idNode_);
    void Accept(Visitor& visitor) override;
    Node* IdNode() const { return idNode.get(); }
private:
    std::unique_ptr<Node> idNode;
};

class ArrayDeclaratorNode : public Node
{
public:
    ArrayDeclaratorNode(const Span& span_, Node* declarator_, Node* size_);
    void Accept(Visitor& visitor) override;
    Node* Declarator() { return declarator.get(); }
    Node* Size() { return size.get(); }
private:
    std::unique_ptr<Node> declarator;
    std::unique_ptr<Node> size;
};

class FunctionDeclaratorNode : public Node
{
public:
    FunctionDeclaratorNode(const Span& span_, Node* declarator_, Node* parameters_, Specifier cvSpecifiers_);
    void Accept(Visitor& visitor) override;
    bool IsFunctionDeclaratorNode() const override { return true; }
    Node* Declarator() { return declarator.get(); }
    Node* Parameters() { return parameters.get(); }
    Specifier CVSpecifiers() const { return cvSpecifiers; }
private:
    std::unique_ptr<Node> declarator;
    std::unique_ptr<Node> parameters;
    Specifier cvSpecifiers;
};

class FunctionPtrIdNode : public Node
{
public:
    FunctionPtrIdNode(const Span& span_, const std::u32string& id_);
    void Accept(Visitor& visitor) override;
    const std::u32string& Id() const { return id; }
private:
    std::u32string id;
};

class MemberFunctionPtrIdNode : public Node
{
public:
    MemberFunctionPtrIdNode(const Span& span_, Node* classNameNode_, const std::u32string& id_);
    void Accept(Visitor& visitor) override;
    Node* ClassNameNode() { return classNameNode.get(); }
    const std::u32string& Id() const { return id; }
private:
    std::unique_ptr<Node> classNameNode;
    std::u32string id;
};

class InitDeclaratorNode : public Node
{
public:
    InitDeclaratorNode(const Span& span_, Node* declarator_, Node* initializer_);
    void Accept(Visitor& visitor) override;
    Node* Declarator() { return declarator.get(); }
    Node* Initializer() { return initializer.get(); }
private:
    std::unique_ptr<Node> declarator;
    std::unique_ptr<Node> initializer;
};

class AssignmentInitializerNode : public UnaryNode
{
public:
    AssignmentInitializerNode(const Span& span_, Node* initializer_);
    void Accept(Visitor& visitor) override;
};

class ExpressionListInitializerNode : public UnaryNode
{
public:
    ExpressionListInitializerNode(const Span& span_, Node* initializer_);
    void Accept(Visitor& visitor) override;
};

class ExpressionInitializerNode : public UnaryNode
{
public:
    ExpressionInitializerNode(const Span& span_, Node* expr_);
    void Accept(Visitor& visitor) override;
};

class BracedInitializerListNode : public UnaryNode
{
public:
    BracedInitializerListNode(const Span& span_);
    void Accept(Visitor& visitor) override;
    void AddInitializer(const Span& span, Node* initializer);
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    Span endBraceSpan;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_DECLARATOR_INCLUDED
