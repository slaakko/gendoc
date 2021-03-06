// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_CLASS_INCLUDED
#define GENDOC_CPPAST_CLASS_INCLUDED
#include <gendoc/cppast/Statement.hpp>
#include <gendoc/cppast/Specifier.hpp>

namespace gendoc { namespace cppast {

enum class ClassKey : uint8_t
{
    none, class_, struct_, union_, typename_
};

std::u32string ToString(ClassKey classKey);

class BaseClassSpecifierNode : public Node
{
public:
    BaseClassSpecifierNode(const Span& span_, Specifier baseSpecifiers_, Node* className_);
    void Accept(Visitor& visitor) override;
    Specifier BaseSpecifiers() const { return baseSpecifiers; }
    Node* ClassName() { return className.get(); }
private:
    Specifier baseSpecifiers;
    std::unique_ptr<Node> className;
};

class BaseClassSpecifierSequenceNode : public BinaryNode
{
public:
    BaseClassSpecifierSequenceNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class ForwardClassDeclarationNode : public Node
{
public:
    ForwardClassDeclarationNode(const Span& span_, ClassKey classKey_, Node* className_);
    void Accept(Visitor& visitor) override;
    ClassKey GetClassKey() const { return classKey; }
    Node* ClassName() { return className.get(); }
private:
    ClassKey classKey;
    std::unique_ptr<Node> className;
};

class ElaborateClassNameNode : public Node
{
public:
    ElaborateClassNameNode(const Span& span_, ClassKey classKey_, Node* className_);
    void Accept(Visitor& visitor) override;
    ClassKey GetClassKey() const { return classKey; }
    Node* ClassName() { return className.get(); }
private:
    ClassKey classKey;
    std::unique_ptr<Node> className;
};

class ClassNode : public Node
{
public:
    ClassNode(const Span& span_, ClassKey classKey_, Node* className_, Specifier classVirtSpecifiers_, Node* baseClasses_);
    void Accept(Visitor& visitor) override;
    void AddDeclaration(const Span& span, Node* declaration);
    Node* ClassName() { return className.get(); }
    ClassKey GetClassKey() const { return classKey; }
    Specifier ClassVirtSpecifiers() const { return classVirtSpecifiers; }
    Node* BaseClasses() { return baseClasses.get(); }
    Node* Declarations() { return declarations.get(); }
    Specifier DefaultAccess() const;
    void SetOpenBraceSpan(const Span& openBraceSpan_) { openBraceSpan = openBraceSpan_; }
    void SetCloseBraceSpan(const Span& closeBraceSpan_) { closeBraceSpan = closeBraceSpan_; }
    const Span& OpenBraceSpan() const { return openBraceSpan; }
    const Span& CloseBraceSpan() const { return closeBraceSpan; }
private:
    ClassKey classKey;
    std::unique_ptr<Node> className;
    Specifier classVirtSpecifiers;
    std::unique_ptr<Node> baseClasses;
    std::unique_ptr<Node> declarations;
    Span openBraceSpan;
    Span closeBraceSpan;
};

class MemberAccessDeclarationNode : public Node
{
public:
    MemberAccessDeclarationNode(const Span& span_, Specifier accessSpecifier_, Node* declaration_, bool explicitAccess_);
    void Accept(Visitor& visitor) override;
    Specifier AccessSpecifier() const { return accessSpecifier; }
    Node* Declaration() { return declaration.get(); }
    bool ExplicitAccess() const { return explicitAccess; }
private:
    Specifier accessSpecifier;
    std::unique_ptr<Node> declaration;
    bool explicitAccess;
};

class MemberDeclarationNode : public Node
{
public:
    MemberDeclarationNode(const Span& span_, Specifier specifiers_, Node* typeExpr_, Node* declarator_, Node* initializer_);
    void Accept(Visitor& visitor) override;
    Specifier Specifiers() const { return specifiers; }
    Node* TypeExpr() { return typeExpr.get(); }
    Node* Declarator() { return declarator.get(); }
    Node* Initializer() { return initializer.get(); }
private:
    Specifier specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> declarator;
    std::unique_ptr<Node> initializer;
};

class SpecialMemberFunctionNode : public Node
{
public:
    SpecialMemberFunctionNode(const Span& span_, Specifier specifiers_, Node* declarator_, Node* ctorInitializer_, CompoundStatementNode* functionBody_);
    void Accept(Visitor& visitor) override;
    Specifier Specifiers() const { return specifiers; }
    Node* Declarator() { return declarator.get(); }
    Node* CtorInitializer() { return ctorInitializer.get(); }
    CompoundStatementNode* FunctionBody() { return functionBody.get(); }
private:
    Specifier specifiers;
    std::unique_ptr<Node> declarator;
    std::unique_ptr<Node> ctorInitializer;
    std::unique_ptr<CompoundStatementNode> functionBody;
};

class CtorInitializerNode : public UnaryNode
{
public:
    CtorInitializerNode(const Span& span_, Node* memberInitializers_);
    void Accept(Visitor& visitor) override;
};

class MemberInitializerNode : public Node
{
public:
    MemberInitializerNode(const Span& span_, Node* id_, Node* initializer_);
    void Accept(Visitor& visitor) override;
    Node* Id() { return id.get(); }
    Node* Initializer() { return initializer.get(); }
private:
    std::unique_ptr<Node> id;
    std::unique_ptr<Node> initializer;
};

class MemberInitializerSequenceNode : public BinaryNode
{
public:
    MemberInitializerSequenceNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_CLASS_INCLUDED
