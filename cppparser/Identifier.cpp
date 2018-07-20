#include "Identifier.hpp"
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
#include <gendoc/cppparser/Keyword.hpp>
#include <gendoc/cppparser/Template.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;
using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Identifier* Identifier::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Identifier* Identifier::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Identifier* grammar(new Identifier(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Identifier::Identifier(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Identifier"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

std::u32string Identifier::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    std::u32string result = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(value.get());
    stack.pop();
    return result;
}

class Identifier::IdentifierRule : public gendoc::parsing::Rule
{
public:
    IdentifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdentifierRule>(this, &IdentifierRule::A0Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IdentifierRule>(this, &IdentifierRule::Postidentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromidentifier;
    }
    void Postidentifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromidentifier_value = std::move(stack.top());
            context->fromidentifier = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromidentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromidentifier() {}
        std::u32string value;
        std::u32string fromidentifier;
    };
};

class Identifier::IdNodeRule : public gendoc::parsing::Rule
{
public:
    IdNodeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdNodeRule>(this, &IdNodeRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdNodeRule>(this, &IdNodeRule::A1Action));
        gendoc::parsing::NonterminalParser* simpleTemplateIdNonterminalParser = GetNonterminal(ToUtf32("SimpleTemplateId"));
        simpleTemplateIdNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IdNodeRule>(this, &IdNodeRule::PreSimpleTemplateId));
        simpleTemplateIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IdNodeRule>(this, &IdNodeRule::PostSimpleTemplateId));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IdNodeRule>(this, &IdNodeRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleTemplateId;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdentifierNode(span, context->fromIdentifier);
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
        Context(): ctx(), value(), fromSimpleTemplateId(), fromIdentifier() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromSimpleTemplateId;
        std::u32string fromIdentifier;
    };
};

class Identifier::NestedNameSpecifierRule : public gendoc::parsing::Rule
{
public:
    NestedNameSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("node")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NestedNameSpecifierRule>(this, &NestedNameSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NestedNameSpecifierRule>(this, &NestedNameSpecifierRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NestedNameSpecifierRule>(this, &NestedNameSpecifierRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NestedNameSpecifierRule>(this, &NestedNameSpecifierRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NestedNameSpecifierRule>(this, &NestedNameSpecifierRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NestedNameSpecifierRule>(this, &NestedNameSpecifierRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NestedNameSpecifierRule>(this, &NestedNameSpecifierRule::Postright));
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
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(new NestedIdNode(span, context->node.release(), context->fromright));
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
        Context(): ctx(), value(), node(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> node;
        Node* fromleft;
        Node* fromright;
    };
};

class Identifier::QualifiedIdNodeRule : public gendoc::parsing::Rule
{
public:
    QualifiedIdNodeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("node")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::A5Action));
        gendoc::parsing::NonterminalParser* ns1NonterminalParser = GetNonterminal(ToUtf32("ns1"));
        ns1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Prens1));
        ns1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Postns1));
        gendoc::parsing::NonterminalParser* id1NonterminalParser = GetNonterminal(ToUtf32("id1"));
        id1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Preid1));
        id1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Postid1));
        gendoc::parsing::NonterminalParser* ns2NonterminalParser = GetNonterminal(ToUtf32("ns2"));
        ns2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Prens2));
        ns2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Postns2));
        gendoc::parsing::NonterminalParser* id2NonterminalParser = GetNonterminal(ToUtf32("id2"));
        id2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Preid2));
        id2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Postid2));
        gendoc::parsing::NonterminalParser* id3NonterminalParser = GetNonterminal(ToUtf32("id3"));
        id3NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Preid3));
        id3NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdNodeRule>(this, &QualifiedIdNodeRule::Postid3));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->node.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(new NestedIdNode(span, new NestedIdNode(span, nullptr, context->node.release()), context->fromid1));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(context->fromns1);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(new NestedIdNode(span, context->node.release(), context->fromid2));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(context->fromns2);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(context->fromid3);
    }
    void Prens1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postns1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromns1_value = std::move(stack.top());
            context->fromns1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromns1_value.get());
            stack.pop();
        }
    }
    void Preid1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postid1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromid1_value = std::move(stack.top());
            context->fromid1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromid1_value.get());
            stack.pop();
        }
    }
    void Prens2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postns2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromns2_value = std::move(stack.top());
            context->fromns2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromns2_value.get());
            stack.pop();
        }
    }
    void Preid2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postid2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromid2_value = std::move(stack.top());
            context->fromid2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromid2_value.get());
            stack.pop();
        }
    }
    void Preid3(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postid3(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromid3_value = std::move(stack.top());
            context->fromid3 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromid3_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), node(), fromns1(), fromid1(), fromns2(), fromid2(), fromid3() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> node;
        Node* fromns1;
        Node* fromid1;
        Node* fromns2;
        Node* fromid2;
        Node* fromid3;
    };
};

void Identifier::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Template"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::Template::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Keyword"));
    if (!grammar2)
    {
        grammar2 = gendoc::cppparser::Keyword::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void Identifier::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("gendoc.parsing.stdlib.identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Keyword"), this, ToUtf32("Keyword.Keyword")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("SimpleTemplateId"), this, ToUtf32("Template.SimpleTemplateId")));
    AddRule(new IdentifierRule(ToUtf32("Identifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::DifferenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("identifier"), ToUtf32("identifier"), 0),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Keyword"), ToUtf32("Keyword"), 0))))));
    AddRule(new IdNodeRule(ToUtf32("IdNode"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("SimpleTemplateId"), ToUtf32("SimpleTemplateId"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)))));
    AddRule(new NestedNameSpecifierRule(ToUtf32("NestedNameSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("IdNode"), 1)),
                        new gendoc::parsing::StringParser(ToUtf32("::"))),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("IdNode"), 1),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::StringParser(ToUtf32("::")))))))))));
    AddRule(new QualifiedIdNodeRule(ToUtf32("QualifiedIdNode"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::StringParser(ToUtf32("::")),
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("ns1"), ToUtf32("NestedNameSpecifier"), 1))),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("id1"), ToUtf32("IdNode"), 1)))),
                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("ns2"), ToUtf32("NestedNameSpecifier"), 1)),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("id2"), ToUtf32("IdNode"), 1))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A5"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("id3"), ToUtf32("IdNode"), 1)))))));
}

} } // namespace gendoc.cppparser
