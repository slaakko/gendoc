// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_TEMPLATE_INCLUDED
#define GENDOC_CPPAST_TEMPLATE_INCLUDED
#include <gendoc/cppast/Expression.hpp>

namespace gendoc { namespace cppast {

class TypeParameterNode : public Node
{
public:
    TypeParameterNode(const Span& span_, const std::u32string& id_, Node* defaultTemplateParameter_, bool typenameUsed_);
    void Accept(Visitor& visitor) override;
    const std::u32string& Id() const { return id; }
    Node* DefaultTemplateParameter() { return defaultTemplateParameter.get(); }
    bool TypenameUsed() const { return typenameUsed; }
private:
    std::u32string id;
    std::unique_ptr<Node> defaultTemplateParameter;
    bool typenameUsed;
};

class TemplateParameterSequenceNode : public BinaryNode
{
public:
    TemplateParameterSequenceNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class TemplateDeclarationNode : public Node
{
public:
    TemplateDeclarationNode(const Span& span_, Node* templateParameters_, Node* declaration_);
    void Accept(Visitor& visitor) override;
    Node* TemplateParameters() { return templateParameters.get(); }
    Node* Declaration() { return declaration.get(); }
private:
    std::unique_ptr<Node> templateParameters;
    std::unique_ptr<Node> declaration;
};

class TemplateArgumentSequenceNode : public BinaryNode
{
public:
    TemplateArgumentSequenceNode(const Span& span_, Node* left_, Node* right_);
    bool IsTemplateArgumentSequenceNode() const override { return true; }
    void Accept(Visitor& visitor) override;
};

class TemplateIdNode : public Node
{
public:
    TemplateIdNode(const Span& span_, IdentifierNode* id_, Node* templateArguments_, int arity_);
    bool IsTemplateIdNode() const override { return true; }
    void Accept(Visitor& visitor) override;
    IdentifierNode* Id() { return id.get(); }
    Node* TemplateArguments() { return templateArguments.get(); }
    int Arity() const { return arity; }
private:
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> templateArguments;
    int arity;
};

class TemplateArgumentNode : public Node
{
public:
    TemplateArgumentNode(const Span& span_, Node* arg_);
    void Accept(Visitor& visitor) override;
    Node* Arg() { return arg.get(); }
private:
    std::unique_ptr<Node> arg;
};

class ExplicitInstantiationNode : public UnaryNode
{
public:
    ExplicitInstantiationNode(const Span& span_, Node* declaration_);
    void Accept(Visitor& visitor) override;
};

class ExplicitSpecializationNode : public UnaryNode
{
public:
    ExplicitSpecializationNode(const Span& span_, Node* declaration_);
    void Accept(Visitor& visitor) override;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_TEMPLATE_INCLUDED
