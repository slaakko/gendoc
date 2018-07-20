#include "Template.hpp"
#include <gendoc/parsing/Action.hpp>
#include <gendoc/parsing/Rule.hpp>
#include <gendoc/parsing/ParsingDomain.hpp>
#include <gendoc/parsing/Primitive.hpp>
#include <gendoc/parsing/Composite.hpp>
#include <gendoc/parsing/Nonterminal.hpp>
#include <gendoc/parsing/Exception.hpp>
#include <gendoc/parsing/StdLib.hpp>
#include <gendoc/parsing/XmlLog.hpp>
#include <gendoc/util/Unicode.hpp>
#include <gendoc/cppparser/Declaration.hpp>
#include <gendoc/cppparser/Function.hpp>
#include <gendoc/cppparser/Identifier.hpp>
#include <gendoc/cppparser/TypeExpr.hpp>
#include <gendoc/cppparser/Expression.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Template* Template::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Template* Template::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Template* grammar(new Template(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Template::Template(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Template"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

Node* Template::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
{
    gendoc::parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());
    std::unique_ptr<gendoc::parsing::XmlLog> xmlLog;
    if (Log())
    {
        xmlLog.reset(new gendoc::parsing::XmlLog(*Log(), MaxLogLineLength()));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
    }
    gendoc::parsing::ObjectStack stack;
    std::unique_ptr<gendoc::parsing::ParsingData> parsingData(new gendoc::parsing::ParsingData(GetParsingDomain()->GetNumRules()));
    scanner.SetParsingData(parsingData.get());
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<ParsingContext*>(ctx)));
    gendoc::parsing::Match match = gendoc::parsing::Grammar::Parse(scanner, stack, parsingData.get());
    gendoc::parsing::Span stop = scanner.GetSpan();
    if (Log())
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (StartRule())
        {
            throw gendoc::parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);
        }
        else
        {
            throw gendoc::parsing::ParsingException("grammar '" + ToUtf8(Name()) + "' has no start rule", fileName, scanner.GetSpan(), start, end);
        }
    }
    std::unique_ptr<gendoc::parsing::Object> value = std::move(stack.top());
    Node* result = *static_cast<gendoc::parsing::ValueObject<Node*>*>(value.get());
    stack.pop();
    return result;
}

class Template::TemplateDeclarationRule : public gendoc::parsing::Rule
{
public:
    TemplateDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateDeclarationRule>(this, &TemplateDeclarationRule::A0Action));
        gendoc::parsing::NonterminalParser* templateParameterListNonterminalParser = GetNonterminal(ToUtf32("TemplateParameterList"));
        templateParameterListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateDeclarationRule>(this, &TemplateDeclarationRule::PreTemplateParameterList));
        templateParameterListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateDeclarationRule>(this, &TemplateDeclarationRule::PostTemplateParameterList));
        gendoc::parsing::NonterminalParser* declarationNonterminalParser = GetNonterminal(ToUtf32("Declaration"));
        declarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateDeclarationRule>(this, &TemplateDeclarationRule::PreDeclaration));
        declarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateDeclarationRule>(this, &TemplateDeclarationRule::PostDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TemplateDeclarationNode(span, context->fromTemplateParameterList, context->fromDeclaration);
    }
    void PreTemplateParameterList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTemplateParameterList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTemplateParameterList_value = std::move(stack.top());
            context->fromTemplateParameterList = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTemplateParameterList_value.get());
            stack.pop();
        }
    }
    void PreDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclaration_value = std::move(stack.top());
            context->fromDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromTemplateParameterList(), fromDeclaration() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTemplateParameterList;
        Node* fromDeclaration;
    };
};

class Template::TemplateParameterListRule : public gendoc::parsing::Rule
{
public:
    TemplateParameterListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("node")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateParameterListRule>(this, &TemplateParameterListRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateParameterListRule>(this, &TemplateParameterListRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateParameterListRule>(this, &TemplateParameterListRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateParameterListRule>(this, &TemplateParameterListRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateParameterListRule>(this, &TemplateParameterListRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateParameterListRule>(this, &TemplateParameterListRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateParameterListRule>(this, &TemplateParameterListRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->node.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->node.reset(new TemplateParameterSequenceNode(context->s, context->node.release(), context->fromright));
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), node(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> node;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Template::TemplateParameterRule : public gendoc::parsing::Rule
{
public:
    TemplateParameterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateParameterRule>(this, &TemplateParameterRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateParameterRule>(this, &TemplateParameterRule::A1Action));
        gendoc::parsing::NonterminalParser* typeParameterNonterminalParser = GetNonterminal(ToUtf32("TypeParameter"));
        typeParameterNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateParameterRule>(this, &TemplateParameterRule::PreTypeParameter));
        typeParameterNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateParameterRule>(this, &TemplateParameterRule::PostTypeParameter));
        gendoc::parsing::NonterminalParser* parameterDeclarationNonterminalParser = GetNonterminal(ToUtf32("ParameterDeclaration"));
        parameterDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateParameterRule>(this, &TemplateParameterRule::PreParameterDeclaration));
        parameterDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateParameterRule>(this, &TemplateParameterRule::PostParameterDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeParameter;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromParameterDeclaration;
    }
    void PreTypeParameter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeParameter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeParameter_value = std::move(stack.top());
            context->fromTypeParameter = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTypeParameter_value.get());
            stack.pop();
        }
    }
    void PreParameterDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostParameterDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromParameterDeclaration_value = std::move(stack.top());
            context->fromParameterDeclaration = *static_cast<gendoc::parsing::ValueObject<ParameterNode*>*>(fromParameterDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromTypeParameter(), fromParameterDeclaration() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTypeParameter;
        ParameterNode* fromParameterDeclaration;
    };
};

class Template::TypeParameterRule : public gendoc::parsing::Rule
{
public:
    TypeParameterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("typenameUsed")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeParameterRule>(this, &TypeParameterRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeParameterRule>(this, &TypeParameterRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeParameterRule>(this, &TypeParameterRule::A2Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeParameterRule>(this, &TypeParameterRule::PostIdentifier));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TypeParameterRule>(this, &TypeParameterRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeParameterRule>(this, &TypeParameterRule::PostTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeParameterNode(span, context->fromIdentifier, context->fromTypeExpr, context->typenameUsed);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typenameUsed = false;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typenameUsed = true;
    }
    void PostIdentifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PreTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context->fromTypeExpr = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), typenameUsed(), fromIdentifier(), fromTypeExpr() {}
        ParsingContext* ctx;
        Node* value;
        bool typenameUsed;
        std::u32string fromIdentifier;
        Node* fromTypeExpr;
    };
};

class Template::SimpleTemplateIdRule : public gendoc::parsing::Rule
{
public:
    SimpleTemplateIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("args")));
        AddLocalVariable(AttrOrVariable(ToUtf32("int"), ToUtf32("arity")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTemplateIdRule>(this, &SimpleTemplateIdRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTemplateIdRule>(this, &SimpleTemplateIdRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTemplateIdRule>(this, &SimpleTemplateIdRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTemplateIdRule>(this, &SimpleTemplateIdRule::A3Action));
        a3ActionParser->SetFailureAction(new gendoc::parsing::MemberFailureAction<SimpleTemplateIdRule>(this, &SimpleTemplateIdRule::A3ActionFail));
        gendoc::parsing::NonterminalParser* templateNameNonterminalParser = GetNonterminal(ToUtf32("TemplateName"));
        templateNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SimpleTemplateIdRule>(this, &SimpleTemplateIdRule::PostTemplateName));
        gendoc::parsing::NonterminalParser* templateArgumentListNonterminalParser = GetNonterminal(ToUtf32("TemplateArgumentList"));
        templateArgumentListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SimpleTemplateIdRule>(this, &SimpleTemplateIdRule::PreTemplateArgumentList));
        templateArgumentListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SimpleTemplateIdRule>(this, &SimpleTemplateIdRule::PostTemplateArgumentList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TemplateIdNode(span, context->fromTemplateName, context->args.release(), context->arity);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->BeginParsingTemplateId();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->args.reset(context->fromTemplateArgumentList);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingTemplateId();
    }
    void A3ActionFail(ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingTemplateId();
    }
    void PostTemplateName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTemplateName_value = std::move(stack.top());
            context->fromTemplateName = *static_cast<gendoc::parsing::ValueObject<IdentifierNode*>*>(fromTemplateName_value.get());
            stack.pop();
        }
    }
    void PreTemplateArgumentList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<int*>(&context->arity)));
    }
    void PostTemplateArgumentList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTemplateArgumentList_value = std::move(stack.top());
            context->fromTemplateArgumentList = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTemplateArgumentList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), args(), arity(), fromTemplateName(), fromTemplateArgumentList() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> args;
        int arity;
        IdentifierNode* fromTemplateName;
        Node* fromTemplateArgumentList;
    };
};

class Template::TemplateIdRule : public gendoc::parsing::Rule
{
public:
    TemplateIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateIdRule>(this, &TemplateIdRule::A0Action));
        gendoc::parsing::NonterminalParser* simpleTemplateIdNonterminalParser = GetNonterminal(ToUtf32("SimpleTemplateId"));
        simpleTemplateIdNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateIdRule>(this, &TemplateIdRule::PreSimpleTemplateId));
        simpleTemplateIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateIdRule>(this, &TemplateIdRule::PostSimpleTemplateId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleTemplateId;
    }
    void PreSimpleTemplateId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostSimpleTemplateId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSimpleTemplateId_value = std::move(stack.top());
            context->fromSimpleTemplateId = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromSimpleTemplateId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromSimpleTemplateId() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromSimpleTemplateId;
    };
};

class Template::TemplateNameRule : public gendoc::parsing::Rule
{
public:
    TemplateNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("IdentifierNode*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<IdentifierNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateNameRule>(this, &TemplateNameRule::A0Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateNameRule>(this, &TemplateNameRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdentifierNode(span, context->fromIdentifier);
    }
    void PostIdentifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromIdentifier() {}
        IdentifierNode* value;
        std::u32string fromIdentifier;
    };
};

class Template::TemplateArgumentListRule : public gendoc::parsing::Rule
{
public:
    TemplateArgumentListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("int*"), ToUtf32("arity")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("arg")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> arity_value = std::move(stack.top());
        context->arity = *static_cast<gendoc::parsing::ValueObject<int*>*>(arity_value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateArgumentListRule>(this, &TemplateArgumentListRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateArgumentListRule>(this, &TemplateArgumentListRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateArgumentListRule>(this, &TemplateArgumentListRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateArgumentListRule>(this, &TemplateArgumentListRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateArgumentListRule>(this, &TemplateArgumentListRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateArgumentListRule>(this, &TemplateArgumentListRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateArgumentListRule>(this, &TemplateArgumentListRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->arg.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->arg.reset(new TemplateArgumentNode(span, context->fromleft));
        ++*context->arity;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->arg.reset(new TemplateArgumentSequenceNode(span, context->arg.release(), new TemplateArgumentNode(span, context->fromright)));
        ++*context->arity;
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), arity(), value(), arg(), fromleft(), fromright() {}
        ParsingContext* ctx;
        int* arity;
        Node* value;
        std::unique_ptr<Node> arg;
        Node* fromleft;
        Node* fromright;
    };
};

class Template::TemplateArgumentRule : public gendoc::parsing::Rule
{
public:
    TemplateArgumentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateArgumentRule>(this, &TemplateArgumentRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TemplateArgumentRule>(this, &TemplateArgumentRule::A1Action));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateArgumentRule>(this, &TemplateArgumentRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateArgumentRule>(this, &TemplateArgumentRule::PostTypeExpr));
        gendoc::parsing::NonterminalParser* conditionalExpressionNonterminalParser = GetNonterminal(ToUtf32("ConditionalExpression"));
        conditionalExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TemplateArgumentRule>(this, &TemplateArgumentRule::PreConditionalExpression));
        conditionalExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TemplateArgumentRule>(this, &TemplateArgumentRule::PostConditionalExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeExpr;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConditionalExpression;
    }
    void PreTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context->fromTypeExpr = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PreConditionalExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConditionalExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromConditionalExpression_value = std::move(stack.top());
            context->fromConditionalExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromConditionalExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromTypeExpr(), fromConditionalExpression() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTypeExpr;
        Node* fromConditionalExpression;
    };
};

class Template::ExplicitInstantiationRule : public gendoc::parsing::Rule
{
public:
    ExplicitInstantiationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExplicitInstantiationRule>(this, &ExplicitInstantiationRule::A0Action));
        gendoc::parsing::NonterminalParser* declarationNonterminalParser = GetNonterminal(ToUtf32("Declaration"));
        declarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExplicitInstantiationRule>(this, &ExplicitInstantiationRule::PreDeclaration));
        declarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExplicitInstantiationRule>(this, &ExplicitInstantiationRule::PostDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExplicitInstantiationNode(span, context->fromDeclaration);
    }
    void PreDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclaration_value = std::move(stack.top());
            context->fromDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromDeclaration() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromDeclaration;
    };
};

class Template::ExplicitSpecializationRule : public gendoc::parsing::Rule
{
public:
    ExplicitSpecializationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExplicitSpecializationRule>(this, &ExplicitSpecializationRule::A0Action));
        gendoc::parsing::NonterminalParser* declarationNonterminalParser = GetNonterminal(ToUtf32("Declaration"));
        declarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExplicitSpecializationRule>(this, &ExplicitSpecializationRule::PreDeclaration));
        declarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExplicitSpecializationRule>(this, &ExplicitSpecializationRule::PostDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExplicitSpecializationNode(span, context->fromDeclaration);
    }
    void PreDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclaration_value = std::move(stack.top());
            context->fromDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromDeclaration() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromDeclaration;
    };
};

void Template::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Declaration"));
    if (!grammar0)
    {
        grammar0 = gendoc::cppparser::Declaration::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Function"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::Function::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Identifier"));
    if (!grammar2)
    {
        grammar2 = gendoc::cppparser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar2);
    gendoc::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("gendoc.cppparser.TypeExpr"));
    if (!grammar3)
    {
        grammar3 = gendoc::cppparser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar3);
    gendoc::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Expression"));
    if (!grammar4)
    {
        grammar4 = gendoc::cppparser::Expression::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void Template::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Declaration"), this, ToUtf32("Declaration.Declaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ParameterDeclaration"), this, ToUtf32("Function.ParameterDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ConditionalExpression"), this, ToUtf32("Expression.ConditionalExpression")));
    AddRule(new TemplateDeclarationRule(ToUtf32("TemplateDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("template")),
                                new gendoc::parsing::CharParser('<')),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("TemplateParameterList"), ToUtf32("TemplateParameterList"), 1))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser('>'))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Declaration"), ToUtf32("Declaration"), 1)))))));
    AddRule(new TemplateParameterListRule(ToUtf32("TemplateParameterList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("TemplateParameter"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser(','),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("TemplateParameter"), 1))))))))));
    AddRule(new TemplateParameterRule(ToUtf32("TemplateParameter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("TypeParameter"), ToUtf32("TypeParameter"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ParameterDeclaration"), ToUtf32("ParameterDeclaration"), 1)))));
    AddRule(new TypeParameterRule(ToUtf32("TypeParameter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::KeywordParser(ToUtf32("class"))),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::KeywordParser(ToUtf32("typename"))))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser('='),
                                new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)))))))));
    AddRule(new SimpleTemplateIdRule(ToUtf32("SimpleTemplateId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("TemplateName"), ToUtf32("TemplateName"), 0),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::CharParser('<'))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("TemplateArgumentList"), ToUtf32("TemplateArgumentList"), 2))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                        new gendoc::parsing::CharParser('>')))))));
    AddRule(new TemplateIdRule(ToUtf32("TemplateId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("SimpleTemplateId"), ToUtf32("SimpleTemplateId"), 1))));
    AddRule(new TemplateNameRule(ToUtf32("TemplateName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))));
    AddRule(new TemplateArgumentListRule(ToUtf32("TemplateArgumentList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("TemplateArgument"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser(','),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("TemplateArgument"), 1))))))))));
    AddRule(new TemplateArgumentRule(ToUtf32("TemplateArgument"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ConditionalExpression"), ToUtf32("ConditionalExpression"), 1)))));
    AddRule(new ExplicitInstantiationRule(ToUtf32("ExplicitInstantiation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::KeywordParser(ToUtf32("template")),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Declaration"), ToUtf32("Declaration"), 1))))));
    AddRule(new ExplicitSpecializationRule(ToUtf32("ExplicitSpecialization"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("template")),
                            new gendoc::parsing::CharParser('<')),
                        new gendoc::parsing::CharParser('>')),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Declaration"), ToUtf32("Declaration"), 1))))));
}

} } // namespace gendoc.cppparser
