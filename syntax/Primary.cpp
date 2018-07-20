#include "Primary.hpp"
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
#include <gendoc/code/Expression.hpp>
#include <gendoc/code/Statement.hpp>
#include <gendoc/syntax/Primitive.hpp>
#include <gendoc/syntax/Composite.hpp>
#include <gendoc/syntax/Element.hpp>
#include <gendoc/parsing/Nonterminal.hpp>
#include <gendoc/codedom/Statement.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::code;
using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Primary* Primary::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Primary* Primary::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Primary* grammar(new Primary(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Primary::Primary(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Primary"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.syntax")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::parsing::Parser* Primary::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Scope* enclosingScope)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<gendoc::parsing::Scope*>(enclosingScope)));
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
    gendoc::parsing::Parser* result = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(value.get());
    stack.pop();
    return result;
}

class Primary::PrimaryRule : public gendoc::parsing::Rule
{
public:
    PrimaryRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A5Action));
        gendoc::parsing::NonterminalParser* ruleCallNonterminalParser = GetNonterminal(ToUtf32("RuleCall"));
        ruleCallNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostRuleCall));
        gendoc::parsing::NonterminalParser* primitiveNonterminalParser = GetNonterminal(ToUtf32("Primitive"));
        primitiveNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostPrimitive));
        gendoc::parsing::NonterminalParser* groupingNonterminalParser = GetNonterminal(ToUtf32("Grouping"));
        groupingNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreGrouping));
        groupingNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostGrouping));
        gendoc::parsing::NonterminalParser* tokenNonterminalParser = GetNonterminal(ToUtf32("Token"));
        tokenNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreToken));
        tokenNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostToken));
        gendoc::parsing::NonterminalParser* expectationNonterminalParser = GetNonterminal(ToUtf32("Expectation"));
        expectationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreExpectation));
        expectationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostExpectation));
        gendoc::parsing::NonterminalParser* actionNonterminalParser = GetNonterminal(ToUtf32("Action"));
        actionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreAction));
        actionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostAction));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRuleCall;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrimitive;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGrouping;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromToken;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpectation;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAction;
    }
    void PostRuleCall(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromRuleCall_value = std::move(stack.top());
            context->fromRuleCall = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromRuleCall_value.get());
            stack.pop();
        }
    }
    void PostPrimitive(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPrimitive_value = std::move(stack.top());
            context->fromPrimitive = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromPrimitive_value.get());
            stack.pop();
        }
    }
    void PreGrouping(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void PostGrouping(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromGrouping_value = std::move(stack.top());
            context->fromGrouping = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromGrouping_value.get());
            stack.pop();
        }
    }
    void PreToken(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void PostToken(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromToken_value = std::move(stack.top());
            context->fromToken = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromToken_value.get());
            stack.pop();
        }
    }
    void PreExpectation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
    }
    void PostExpectation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpectation_value = std::move(stack.top());
            context->fromExpectation = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromExpectation_value.get());
            stack.pop();
        }
    }
    void PreAction(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
    }
    void PostAction(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAction_value = std::move(stack.top());
            context->fromAction = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromAction_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromRuleCall(), fromPrimitive(), fromGrouping(), fromToken(), fromExpectation(), fromAction() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromRuleCall;
        gendoc::parsing::Parser* fromPrimitive;
        gendoc::parsing::Parser* fromGrouping;
        gendoc::parsing::Parser* fromToken;
        gendoc::parsing::Parser* fromExpectation;
        gendoc::parsing::Parser* fromAction;
    };
};

class Primary::RuleCallRule : public gendoc::parsing::Rule
{
public:
    RuleCallRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RuleCallRule>(this, &RuleCallRule::A0Action));
        gendoc::parsing::NonterminalParser* ntWithArgsNonterminalParser = GetNonterminal(ToUtf32("ntWithArgs"));
        ntWithArgsNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::PostntWithArgs));
        gendoc::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::PostExpressionList));
        gendoc::parsing::NonterminalParser* ntNonterminalParser = GetNonterminal(ToUtf32("nt"));
        ntNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::Postnt));
        gendoc::parsing::NonterminalParser* aliasNonterminalParser = GetNonterminal(ToUtf32("Alias"));
        aliasNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::PostAlias));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::u32string ruleName;
        if (!context->fromntWithArgs.empty())
        {
            ruleName = context->fromntWithArgs;
        }
        else
        {
            ruleName = context->fromnt;
        }
        std::u32string alias = context->fromAlias;
        std::u32string name = !alias.empty() ? alias : ruleName;
        NonterminalParser * parser(new NonterminalParser(name, ruleName));
        parser->SetSpan(span);
        if (!context->fromExpressionList.empty())
        {
            parser->SetArguments(context->fromExpressionList);
        }
        context->value = parser;
    }
    void PostntWithArgs(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromntWithArgs_value = std::move(stack.top());
            context->fromntWithArgs = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromntWithArgs_value.get());
            stack.pop();
        }
    }
    void PostExpressionList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
    void Postnt(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromnt_value = std::move(stack.top());
            context->fromnt = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromnt_value.get());
            stack.pop();
        }
    }
    void PostAlias(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAlias_value = std::move(stack.top());
            context->fromAlias = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromAlias_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromntWithArgs(), fromExpressionList(), fromnt(), fromAlias() {}
        gendoc::parsing::Parser* value;
        std::u32string fromntWithArgs;
        std::vector<gendoc::codedom::CppObject*> fromExpressionList;
        std::u32string fromnt;
        std::u32string fromAlias;
    };
};

class Primary::NonterminalRule : public gendoc::parsing::Rule
{
public:
    NonterminalRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NonterminalRule>(this, &NonterminalRule::A0Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NonterminalRule>(this, &NonterminalRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdentifier;
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
        std::u32string value;
        std::u32string fromIdentifier;
    };
};

class Primary::AliasRule : public gendoc::parsing::Rule
{
public:
    AliasRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AliasRule>(this, &AliasRule::A0Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AliasRule>(this, &AliasRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdentifier;
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
        std::u32string value;
        std::u32string fromIdentifier;
    };
};

class Primary::GroupingRule : public gendoc::parsing::Rule
{
public:
    GroupingRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<GroupingRule>(this, &GroupingRule::A0Action));
        gendoc::parsing::NonterminalParser* alternativeNonterminalParser = GetNonterminal(ToUtf32("Alternative"));
        alternativeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GroupingRule>(this, &GroupingRule::PreAlternative));
        alternativeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GroupingRule>(this, &GroupingRule::PostAlternative));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::parsing::GroupingParser(context->fromAlternative);
    }
    void PreAlternative(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void PostAlternative(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAlternative_value = std::move(stack.top());
            context->fromAlternative = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromAlternative_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromAlternative() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromAlternative;
    };
};

class Primary::TokenRule : public gendoc::parsing::Rule
{
public:
    TokenRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TokenRule>(this, &TokenRule::A0Action));
        gendoc::parsing::NonterminalParser* alternativeNonterminalParser = GetNonterminal(ToUtf32("Alternative"));
        alternativeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TokenRule>(this, &TokenRule::PreAlternative));
        alternativeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TokenRule>(this, &TokenRule::PostAlternative));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TokenParser(context->fromAlternative);
    }
    void PreAlternative(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void PostAlternative(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAlternative_value = std::move(stack.top());
            context->fromAlternative = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromAlternative_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromAlternative() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromAlternative;
    };
};

class Primary::ExpectationRule : public gendoc::parsing::Rule
{
public:
    ExpectationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Parser*"), ToUtf32("child")));
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> child_value = std::move(stack.top());
        context->child = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(child_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpectationRule>(this, &ExpectationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExpectationParser(context->child);
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): child(), value() {}
        gendoc::parsing::Parser* child;
        gendoc::parsing::Parser* value;
    };
};

class Primary::ActionRule : public gendoc::parsing::Rule
{
public:
    ActionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Parser*"), ToUtf32("child")));
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> child_value = std::move(stack.top());
        context->child = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(child_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ActionRule>(this, &ActionRule::A0Action));
        gendoc::parsing::NonterminalParser* actionNonterminalParser = GetNonterminal(ToUtf32("action"));
        actionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ActionRule>(this, &ActionRule::Postaction));
        gendoc::parsing::NonterminalParser* failureActionNonterminalParser = GetNonterminal(ToUtf32("failureAction"));
        failureActionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ActionRule>(this, &ActionRule::PostfailureAction));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ActionParser(U"", context->fromaction, context->fromfailureAction, context->child);
    }
    void Postaction(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromaction_value = std::move(stack.top());
            context->fromaction = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CompoundStatement*>*>(fromaction_value.get());
            stack.pop();
        }
    }
    void PostfailureAction(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromfailureAction_value = std::move(stack.top());
            context->fromfailureAction = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CompoundStatement*>*>(fromfailureAction_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): child(), value(), fromaction(), fromfailureAction() {}
        gendoc::parsing::Parser* child;
        gendoc::parsing::Parser* value;
        gendoc::codedom::CompoundStatement* fromaction;
        gendoc::codedom::CompoundStatement* fromfailureAction;
    };
};

void Primary::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.syntax.Primitive"));
    if (!grammar0)
    {
        grammar0 = gendoc::syntax::Primitive::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.code.Statement"));
    if (!grammar1)
    {
        grammar1 = gendoc::code::Statement::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.syntax.Composite"));
    if (!grammar2)
    {
        grammar2 = gendoc::syntax::Composite::Create(pd);
    }
    AddGrammarReference(grammar2);
    gendoc::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("gendoc.code.Expression"));
    if (!grammar3)
    {
        grammar3 = gendoc::code::Expression::Create(pd);
    }
    AddGrammarReference(grammar3);
    gendoc::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("gendoc.syntax.Element"));
    if (!grammar4)
    {
        grammar4 = gendoc::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void Primary::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("CompoundStatement"), this, ToUtf32("gendoc.code.Statement.CompoundStatement")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Alternative"), this, ToUtf32("Composite.Alternative")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Primitive"), this, ToUtf32("Primitive.Primitive")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ExpressionList"), this, ToUtf32("gendoc.code.Expression.ExpressionList")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Element.Identifier")));
    AddRule(new PrimaryRule(ToUtf32("Primary"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("RuleCall"), ToUtf32("RuleCall"), 0)),
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("Primitive"), ToUtf32("Primitive"), 0))),
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Grouping"), ToUtf32("Grouping"), 1))),
                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Token"), ToUtf32("Token"), 1)))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Expectation"), ToUtf32("Expectation"), 1))))),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A5"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Action"), ToUtf32("Action"), 1)))))));
    AddRule(new RuleCallRule(ToUtf32("RuleCall"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::TokenParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("ntWithArgs"), ToUtf32("Nonterminal"), 0),
                                            new gendoc::parsing::CharParser('('))),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0)),
                                new gendoc::parsing::CharParser(')')),
                            new gendoc::parsing::NonterminalParser(ToUtf32("nt"), ToUtf32("Nonterminal"), 0))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Alias"), ToUtf32("Alias"), 0)))))));
    AddRule(new NonterminalRule(ToUtf32("Nonterminal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))));
    AddRule(new AliasRule(ToUtf32("Alias"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::CharParser(':'),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))))));
    AddRule(new GroupingRule(ToUtf32("Grouping"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('('),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Alternative"), ToUtf32("Alternative"), 1))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(')'))))));
    AddRule(new TokenRule(ToUtf32("Token"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::KeywordParser(ToUtf32("token")),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser('('))),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Alternative"), ToUtf32("Alternative"), 1))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(')'))))));
    AddRule(new ExpectationRule(ToUtf32("Expectation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::CharParser('!'))));
    AddRule(new ActionRule(ToUtf32("Action"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("action"), ToUtf32("CompoundStatement"), 0),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser('/'),
                                new gendoc::parsing::NonterminalParser(ToUtf32("failureAction"), ToUtf32("CompoundStatement"), 0)))))))));
}

} } // namespace gendoc.syntax
