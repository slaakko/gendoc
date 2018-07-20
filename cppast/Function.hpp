// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_FUNCTION_INCLUDED
#define GENDOC_CPPAST_FUNCTION_INCLUDED
#include <gendoc/cppast/Statement.hpp>
#include <gendoc/cppast/Specifier.hpp>

namespace gendoc { namespace cppast {

class ParameterNode : public Node
{
public:
    ParameterNode(const Span& span_, Node* typeExpr_, Node* declarator_);
    void Accept(Visitor& visitor) override;
    Node* TypeExpr() { return typeExpr.get(); }
    Node* Declarator() { return declarator.get(); }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> declarator;
};

class ParameterSequenceNode : public BinaryNode
{
public:
    ParameterSequenceNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class FunctionNode : public Node
{
public:
    FunctionNode(const Span& span_, Specifier specifiers_, Node* typeExpr_, Node* declarator_, CompoundStatementNode* body_);
    void Accept(Visitor& visitor) override;
    Specifier Specifiers() const { return specifiers; }
    Node* TypeExpr() { return typeExpr.get(); }
    Node* Declarator() { return declarator.get(); }
    CompoundStatementNode* Body() { return body.get(); }
private:
    Specifier specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> declarator;
    std::unique_ptr<CompoundStatementNode> body;
};

bool IsFunctionDeclarator(Node* declaratorNode);
bool HasPureSpecifier(Specifier specifiers);

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_FUNCTION_INCLUDED
