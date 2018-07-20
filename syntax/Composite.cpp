#include "Composite.hpp"
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
#include <gendoc/parsing/Composite.hpp>
#include <gendoc/syntax/Primary.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Composite* Composite::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Composite* Composite::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Composite* grammar(new Composite(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Composite::Composite(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Composite"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.syntax")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::parsing::Parser* Composite::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Scope* enclosingScope)
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

class Composite::AlternativeRule : public gendoc::parsing::Rule
{
public:
    AlternativeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AlternativeRule>(this, &AlternativeRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AlternativeRule>(this, &AlternativeRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AlternativeRule>(this, &AlternativeRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AlternativeRule>(this, &AlternativeRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AlternativeRule>(this, &AlternativeRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AlternativeRule>(this, &AlternativeRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AlternativeParser(context->value, context->fromright);
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromleft;
        gendoc::parsing::Parser* fromright;
    };
};

class Composite::SequenceRule : public gendoc::parsing::Rule
{
public:
    SequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SequenceRule>(this, &SequenceRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SequenceRule>(this, &SequenceRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SequenceRule>(this, &SequenceRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SequenceRule>(this, &SequenceRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SequenceRule>(this, &SequenceRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SequenceRule>(this, &SequenceRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SequenceParser(context->value, context->fromright);
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromleft;
        gendoc::parsing::Parser* fromright;
    };
};

class Composite::DifferenceRule : public gendoc::parsing::Rule
{
public:
    DifferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DifferenceRule>(this, &DifferenceRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DifferenceRule>(this, &DifferenceRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DifferenceRule>(this, &DifferenceRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DifferenceRule>(this, &DifferenceRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DifferenceRule>(this, &DifferenceRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DifferenceRule>(this, &DifferenceRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DifferenceParser(context->value, context->fromright);
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromleft;
        gendoc::parsing::Parser* fromright;
    };
};

class Composite::ExclusiveOrRule : public gendoc::parsing::Rule
{
public:
    ExclusiveOrRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExclusiveOrRule>(this, &ExclusiveOrRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExclusiveOrRule>(this, &ExclusiveOrRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExclusiveOrParser(context->value, context->fromright);
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromleft;
        gendoc::parsing::Parser* fromright;
    };
};

class Composite::IntersectionRule : public gendoc::parsing::Rule
{
public:
    IntersectionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntersectionRule>(this, &IntersectionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntersectionRule>(this, &IntersectionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IntersectionRule>(this, &IntersectionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IntersectionRule>(this, &IntersectionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IntersectionRule>(this, &IntersectionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IntersectionRule>(this, &IntersectionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IntersectionParser(context->value, context->fromright);
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromleft;
        gendoc::parsing::Parser* fromright;
    };
};

class Composite::ListRule : public gendoc::parsing::Rule
{
public:
    ListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ListRule>(this, &ListRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ListRule>(this, &ListRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ListRule>(this, &ListRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ListRule>(this, &ListRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ListRule>(this, &ListRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ListRule>(this, &ListRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ListParser(context->value, context->fromright);
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromleft;
        gendoc::parsing::Parser* fromright;
    };
};

class Composite::PostfixRule : public gendoc::parsing::Rule
{
public:
    PostfixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A3Action));
        gendoc::parsing::NonterminalParser* primaryNonterminalParser = GetNonterminal(ToUtf32("Primary"));
        primaryNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PostfixRule>(this, &PostfixRule::PrePrimary));
        primaryNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixRule>(this, &PostfixRule::PostPrimary));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrimary;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KleeneStarParser(context->value);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PositiveParser(context->value);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OptionalParser(context->value);
    }
    void PrePrimary(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void PostPrimary(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPrimary_value = std::move(stack.top());
            context->fromPrimary = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromPrimary_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromPrimary() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromPrimary;
    };
};

void Composite::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.syntax.Primary"));
    if (!grammar0)
    {
        grammar0 = gendoc::syntax::Primary::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Composite::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Primary"), this, ToUtf32("Primary.Primary")));
    AddRule(new AlternativeRule(ToUtf32("Alternative"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Sequence"), 1)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('|'),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Sequence"), 1)))))))));
    AddRule(new SequenceRule(ToUtf32("Sequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Difference"), 1)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Difference"), 1)))))));
    AddRule(new DifferenceRule(ToUtf32("Difference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ExclusiveOr"), 1)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('-'),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ExclusiveOr"), 1)))))))));
    AddRule(new ExclusiveOrRule(ToUtf32("ExclusiveOr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Intersection"), 1)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('^'),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Intersection"), 1)))))))));
    AddRule(new IntersectionRule(ToUtf32("Intersection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("List"), 1)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('&'),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("List"), 1))))))));
    AddRule(new ListRule(ToUtf32("List"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Postfix"), 1)),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('%'),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Postfix"), 1))))))));
    AddRule(new PostfixRule(ToUtf32("Postfix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("Primary"), ToUtf32("Primary"), 1)),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::CharParser('*')),
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::CharParser('+'))),
                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                            new gendoc::parsing::CharParser('?'))))))));
}

} } // namespace gendoc.syntax
