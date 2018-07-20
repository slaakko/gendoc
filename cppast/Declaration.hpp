// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_DECLARATION_INCLUDED
#define GENDOC_CPPAST_DECLARATION_INCLUDED
#include <gendoc/cppast/Declarator.hpp>
#include <gendoc/cppast/Specifier.hpp>
#include <gendoc/cppast/Literal.hpp>

namespace gendoc { namespace cppast {

class SimpleDeclarationNode : public Node
{
public:
    SimpleDeclarationNode(const Span& span_, Specifier specifiers_, Node* typeExpr_, Node* declarator_);
    void Accept(Visitor& visitor) override;
    Specifier Specifiers() const { return specifiers; }
    Node* TypeExpr() { return typeExpr.get(); }
    Node* Declarator() { return declarator.get(); }
private:
    Specifier specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> declarator;
};

class AliasDeclarationNode : public Node
{
public:
    AliasDeclarationNode(const Span& span_, const std::u32string& id_, Node* typeExpr_);
    void Accept(Visitor& visitor) override;
    const std::u32string& Id() const { return id; }
    Node* TypeExpr() { return typeExpr.get(); }
private:
    std::u32string id;
    std::unique_ptr<Node> typeExpr;
};

class UsingDirectiveNode : public Node
{
public:
    UsingDirectiveNode(const Span& span_, Node* namespaceName_);
    void Accept(Visitor& visitor) override;
    Node* NamespaceName() { return namespaceName.get(); }
private:
    std::unique_ptr<Node> namespaceName;
};

class UsingDeclarationNode : public Node
{
public:
    UsingDeclarationNode(const Span& span_, Node* qualifiedId_);
    void Accept(Visitor& visitor) override;
    Node* QualifiedId() { return qualifiedId.get(); }
private:
    std::unique_ptr<Node> qualifiedId;
};

class TypedefNode : public Node
{
public:
    TypedefNode(const Span& span_, Node* typeExpr_, Node* declarator_);
    void Accept(Visitor& visitor) override;
    Node* TypeExpr() { return typeExpr.get(); }
    Node* Declarator() { return declarator.get(); }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> declarator;
};

class DeclarationSequenceNode : public BinaryNode
{
public:
    DeclarationSequenceNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class LinkageSpecificationNode : public Node
{
public:
    LinkageSpecificationNode(const Span& span_, StringLiteralNode* language_);
    void Accept(Visitor& visitor) override;
    StringLiteralNode* Language() { return language.get(); }
    Node* Declarations() { return declarations.get(); }
    void AddDeclaration(const Span& span_, Node* declaration);
private:
    std::unique_ptr<StringLiteralNode> language;
    std::unique_ptr<Node> declarations;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_DECLARATION_INCLUDED
