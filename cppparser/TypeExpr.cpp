#include "TypeExpr.hpp"
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
#include <gendoc/cppparser/SimpleType.hpp>
#include <gendoc/cppparser/Identifier.hpp>
#include <gendoc/cppparser/Class.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

TypeExpr* TypeExpr::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

TypeExpr* TypeExpr::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    TypeExpr* grammar(new TypeExpr(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

TypeExpr::TypeExpr(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("TypeExpr"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

Node* TypeExpr::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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

class TypeExpr::TypeExprRule : public gendoc::parsing::Rule
{
public:
    TypeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A0Action));
        gendoc::parsing::NonterminalParser* prefixTypeExprNonterminalParser = GetNonterminal(ToUtf32("PrefixTypeExpr"));
        prefixTypeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TypeExprRule>(this, &TypeExprRule::PrePrefixTypeExpr));
        prefixTypeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeExprRule>(this, &TypeExprRule::PostPrefixTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeExprNode(span, context->fromPrefixTypeExpr);
    }
    void PrePrefixTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPrefixTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPrefixTypeExpr_value = std::move(stack.top());
            context->fromPrefixTypeExpr = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromPrefixTypeExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromPrefixTypeExpr() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromPrefixTypeExpr;
    };
};

class TypeExpr::PrefixTypeExprRule : public gendoc::parsing::Rule
{
public:
    PrefixTypeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrefixTypeExprRule>(this, &PrefixTypeExprRule::A0Action));
        gendoc::parsing::NonterminalParser* cVSpecifierSequenceNonterminalParser = GetNonterminal(ToUtf32("CVSpecifierSequence"));
        cVSpecifierSequenceNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrefixTypeExprRule>(this, &PrefixTypeExprRule::PostCVSpecifierSequence));
        gendoc::parsing::NonterminalParser* postfixTypeExprNonterminalParser = GetNonterminal(ToUtf32("PostfixTypeExpr"));
        postfixTypeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrefixTypeExprRule>(this, &PrefixTypeExprRule::PrePostfixTypeExpr));
        postfixTypeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrefixTypeExprRule>(this, &PrefixTypeExprRule::PostPostfixTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = CreatePrefixTypeExprNode(span, context->fromCVSpecifierSequence, context->fromPostfixTypeExpr);
    }
    void PostCVSpecifierSequence(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCVSpecifierSequence_value = std::move(stack.top());
            context->fromCVSpecifierSequence = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromCVSpecifierSequence_value.get());
            stack.pop();
        }
    }
    void PrePostfixTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPostfixTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPostfixTypeExpr_value = std::move(stack.top());
            context->fromPostfixTypeExpr = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromPostfixTypeExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromCVSpecifierSequence(), fromPostfixTypeExpr() {}
        ParsingContext* ctx;
        Node* value;
        Specifier fromCVSpecifierSequence;
        Node* fromPostfixTypeExpr;
    };
};

class TypeExpr::CVSpecifierSequenceRule : public gendoc::parsing::Rule
{
public:
    CVSpecifierSequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Specifier"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Specifier>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CVSpecifierSequenceRule>(this, &CVSpecifierSequenceRule::A0Action));
        gendoc::parsing::NonterminalParser* cvNonterminalParser = GetNonterminal(ToUtf32("cv"));
        cvNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CVSpecifierSequenceRule>(this, &CVSpecifierSequenceRule::Postcv));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | context->fromcv;
    }
    void Postcv(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromcv_value = std::move(stack.top());
            context->fromcv = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromcv_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromcv() {}
        Specifier value;
        Specifier fromcv;
    };
};

class TypeExpr::CVSpecifierRule : public gendoc::parsing::Rule
{
public:
    CVSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Specifier"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Specifier>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CVSpecifierRule>(this, &CVSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CVSpecifierRule>(this, &CVSpecifierRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::const_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::volatile_;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Specifier value;
    };
};

class TypeExpr::PostfixTypeExprRule : public gendoc::parsing::Rule
{
public:
    PostfixTypeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("typeExpr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A4Action));
        gendoc::parsing::NonterminalParser* primaryTypeExprNonterminalParser = GetNonterminal(ToUtf32("PrimaryTypeExpr"));
        primaryTypeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::PrePrimaryTypeExpr));
        primaryTypeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::PostPrimaryTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->typeExpr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeExpr.reset(context->fromPrimaryTypeExpr);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->typeExpr.reset(new PointerNode(context->s, context->typeExpr.release()));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->typeExpr.reset(new RValueRefNode(context->s, context->typeExpr.release()));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->typeExpr.reset(new LValueRefNode(context->s, context->typeExpr.release()));
    }
    void PrePrimaryTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPrimaryTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPrimaryTypeExpr_value = std::move(stack.top());
            context->fromPrimaryTypeExpr = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromPrimaryTypeExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), typeExpr(), s(), fromPrimaryTypeExpr() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> typeExpr;
        Span s;
        Node* fromPrimaryTypeExpr;
    };
};

class TypeExpr::PrimaryTypeExprRule : public gendoc::parsing::Rule
{
public:
    PrimaryTypeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("ClassKey"), ToUtf32("key")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A3Action));
        gendoc::parsing::NonterminalParser* simpleTypeNonterminalParser = GetNonterminal(ToUtf32("SimpleType"));
        simpleTypeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PostSimpleType));
        gendoc::parsing::NonterminalParser* classKeyNonterminalParser = GetNonterminal(ToUtf32("ClassKey"));
        classKeyNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PostClassKey));
        gendoc::parsing::NonterminalParser* qualifiedIdNodeNonterminalParser = GetNonterminal(ToUtf32("QualifiedIdNode"));
        qualifiedIdNodeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PreQualifiedIdNode));
        qualifiedIdNodeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PostQualifiedIdNode));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleType;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->key = ClassKey::typename_;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->key = context->fromClassKey;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->key != ClassKey::none) context->value = new ElaborateClassNameNode(span, context->key, context->fromQualifiedIdNode);
        else context->value = context->fromQualifiedIdNode;
    }
    void PostSimpleType(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSimpleType_value = std::move(stack.top());
            context->fromSimpleType = *static_cast<gendoc::parsing::ValueObject<SimpleTypeNode*>*>(fromSimpleType_value.get());
            stack.pop();
        }
    }
    void PostClassKey(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassKey_value = std::move(stack.top());
            context->fromClassKey = *static_cast<gendoc::parsing::ValueObject<ClassKey>*>(fromClassKey_value.get());
            stack.pop();
        }
    }
    void PreQualifiedIdNode(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostQualifiedIdNode(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromQualifiedIdNode_value = std::move(stack.top());
            context->fromQualifiedIdNode = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromQualifiedIdNode_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), key(), fromSimpleType(), fromClassKey(), fromQualifiedIdNode() {}
        ParsingContext* ctx;
        Node* value;
        ClassKey key;
        SimpleTypeNode* fromSimpleType;
        ClassKey fromClassKey;
        Node* fromQualifiedIdNode;
    };
};

void TypeExpr::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Class"));
    if (!grammar0)
    {
        grammar0 = gendoc::cppparser::Class::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.SimpleType"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::SimpleType::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Identifier"));
    if (!grammar2)
    {
        grammar2 = gendoc::cppparser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void TypeExpr::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ClassKey"), this, ToUtf32("Class.ClassKey")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("QualifiedIdNode"), this, ToUtf32("Identifier.QualifiedIdNode")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("SimpleType"), this, ToUtf32("SimpleType.SimpleType")));
    AddRule(new TypeExprRule(ToUtf32("TypeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("PrefixTypeExpr"), ToUtf32("PrefixTypeExpr"), 1))));
    AddRule(new PrefixTypeExprRule(ToUtf32("PrefixTypeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("CVSpecifierSequence"), ToUtf32("CVSpecifierSequence"), 0),
                    new gendoc::parsing::NonterminalParser(ToUtf32("PostfixTypeExpr"), ToUtf32("PostfixTypeExpr"), 1))))));
    AddRule(new CVSpecifierSequenceRule(ToUtf32("CVSpecifierSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("cv"), ToUtf32("CVSpecifier"), 0))))));
    AddRule(new CVSpecifierRule(ToUtf32("CVSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::KeywordParser(ToUtf32("const"))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::KeywordParser(ToUtf32("volatile"))))));
    AddRule(new PostfixTypeExprRule(ToUtf32("PostfixTypeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("PrimaryTypeExpr"), ToUtf32("PrimaryTypeExpr"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                        new gendoc::parsing::CharParser('*')),
                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                        new gendoc::parsing::StringParser(ToUtf32("&&")))),
                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                    new gendoc::parsing::CharParser('&'))))))))));
    AddRule(new PrimaryTypeExprRule(ToUtf32("PrimaryTypeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("SimpleType"), ToUtf32("SimpleType"), 0)),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::KeywordParser(ToUtf32("typename"))),
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("ClassKey"), ToUtf32("ClassKey"), 0))))),
                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("QualifiedIdNode"), ToUtf32("QualifiedIdNode"), 1))))));
}

} } // namespace gendoc.cppparser
