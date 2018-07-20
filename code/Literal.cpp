#include "Literal.hpp"
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

namespace gendoc { namespace code {

using namespace gendoc::codedom;
using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Literal* Literal::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Literal* Literal::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Literal* grammar(new Literal(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Literal::Literal(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Literal"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.code")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::codedom::Literal* Literal::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    gendoc::codedom::Literal* result = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Literal*>*>(value.get());
    stack.pop();
    return result;
}

class Literal::LiteralRule : public gendoc::parsing::Rule
{
public:
    LiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Literal*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A5Action));
        gendoc::parsing::NonterminalParser* integerLiteralNonterminalParser = GetNonterminal(ToUtf32("IntegerLiteral"));
        integerLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostIntegerLiteral));
        gendoc::parsing::NonterminalParser* characterLiteralNonterminalParser = GetNonterminal(ToUtf32("CharacterLiteral"));
        characterLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostCharacterLiteral));
        gendoc::parsing::NonterminalParser* floatingLiteralNonterminalParser = GetNonterminal(ToUtf32("FloatingLiteral"));
        floatingLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostFloatingLiteral));
        gendoc::parsing::NonterminalParser* stringLiteralNonterminalParser = GetNonterminal(ToUtf32("StringLiteral"));
        stringLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostStringLiteral));
        gendoc::parsing::NonterminalParser* booleanLiteralNonterminalParser = GetNonterminal(ToUtf32("BooleanLiteral"));
        booleanLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostBooleanLiteral));
        gendoc::parsing::NonterminalParser* pointerLiteralNonterminalParser = GetNonterminal(ToUtf32("PointerLiteral"));
        pointerLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostPointerLiteral));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIntegerLiteral;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCharacterLiteral;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFloatingLiteral;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromStringLiteral;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBooleanLiteral;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPointerLiteral;
    }
    void PostIntegerLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIntegerLiteral_value = std::move(stack.top());
            context->fromIntegerLiteral = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Literal*>*>(fromIntegerLiteral_value.get());
            stack.pop();
        }
    }
    void PostCharacterLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCharacterLiteral_value = std::move(stack.top());
            context->fromCharacterLiteral = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Literal*>*>(fromCharacterLiteral_value.get());
            stack.pop();
        }
    }
    void PostFloatingLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFloatingLiteral_value = std::move(stack.top());
            context->fromFloatingLiteral = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Literal*>*>(fromFloatingLiteral_value.get());
            stack.pop();
        }
    }
    void PostStringLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStringLiteral_value = std::move(stack.top());
            context->fromStringLiteral = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Literal*>*>(fromStringLiteral_value.get());
            stack.pop();
        }
    }
    void PostBooleanLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBooleanLiteral_value = std::move(stack.top());
            context->fromBooleanLiteral = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Literal*>*>(fromBooleanLiteral_value.get());
            stack.pop();
        }
    }
    void PostPointerLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPointerLiteral_value = std::move(stack.top());
            context->fromPointerLiteral = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Literal*>*>(fromPointerLiteral_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromIntegerLiteral(), fromCharacterLiteral(), fromFloatingLiteral(), fromStringLiteral(), fromBooleanLiteral(), fromPointerLiteral() {}
        gendoc::codedom::Literal* value;
        gendoc::codedom::Literal* fromIntegerLiteral;
        gendoc::codedom::Literal* fromCharacterLiteral;
        gendoc::codedom::Literal* fromFloatingLiteral;
        gendoc::codedom::Literal* fromStringLiteral;
        gendoc::codedom::Literal* fromBooleanLiteral;
        gendoc::codedom::Literal* fromPointerLiteral;
    };
};

class Literal::IntegerLiteralRule : public gendoc::parsing::Rule
{
public:
    IntegerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Literal*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::codedom::Literal* value;
    };
};

class Literal::CharacterLiteralRule : public gendoc::parsing::Rule
{
public:
    CharacterLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Literal*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::codedom::Literal* value;
    };
};

class Literal::CCharSequenceRule : public gendoc::parsing::Rule
{
public:
    CCharSequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CCharSequenceRule>(this, &CCharSequenceRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

class Literal::FloatingLiteralRule : public gendoc::parsing::Rule
{
public:
    FloatingLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Literal*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::codedom::Literal* value;
    };
};

class Literal::StringLiteralRule : public gendoc::parsing::Rule
{
public:
    StringLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Literal*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::codedom::Literal* value;
    };
};

class Literal::BooleanLiteralRule : public gendoc::parsing::Rule
{
public:
    BooleanLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Literal*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BooleanLiteralRule>(this, &BooleanLiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BooleanLiteralRule>(this, &BooleanLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(U"true");
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(U"false");
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::codedom::Literal* value;
    };
};

class Literal::PointerLiteralRule : public gendoc::parsing::Rule
{
public:
    PointerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Literal*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PointerLiteralRule>(this, &PointerLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::codedom::Literal(U"nullptr");
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::codedom::Literal* value;
    };
};

void Literal::GetReferencedGrammars()
{
}

void Literal::CreateRules()
{
    AddRule(new LiteralRule(ToUtf32("Literal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("IntegerLiteral"), ToUtf32("IntegerLiteral"), 0)),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("CharacterLiteral"), ToUtf32("CharacterLiteral"), 0))),
                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("FloatingLiteral"), ToUtf32("FloatingLiteral"), 0))),
                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("StringLiteral"), ToUtf32("StringLiteral"), 0))),
                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("BooleanLiteral"), ToUtf32("BooleanLiteral"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                new gendoc::parsing::NonterminalParser(ToUtf32("PointerLiteral"), ToUtf32("PointerLiteral"), 0)))));
    AddRule(new IntegerLiteralRule(ToUtf32("IntegerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("dl"), ToUtf32("DecimalLiteral"), 0),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("is1"), ToUtf32("IntegerSuffix"), 0))))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("ol"), ToUtf32("OctalLiteral"), 0),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("is2"), ToUtf32("IntegerSuffix"), 0)))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("hl"), ToUtf32("HexadecimalLiteral"), 0),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("is3"), ToUtf32("IntegerSuffix"), 0))))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("DecimalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharSetParser(ToUtf32("1-9")),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::DigitParser())))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("OctalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('0'),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("OctalDigit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::CharSetParser(ToUtf32("0-7"))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("HexadecimalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::StringParser(ToUtf32("0x")),
                        new gendoc::parsing::StringParser(ToUtf32("0X")))),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::PositiveParser(
                        new gendoc::parsing::HexDigitParser()))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("IntegerSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("UnsignedSuffix"), ToUtf32("UnsignedSuffix"), 0),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("LongLongSuffix"), ToUtf32("LongLongSuffix"), 0),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("LongSuffix"), ToUtf32("LongSuffix"), 0)))))),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("LongLongSuffix"), ToUtf32("LongLongSuffix"), 0),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("UnsignedSuffix"), ToUtf32("UnsignedSuffix"), 0))))),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("LongSuffix"), ToUtf32("LongSuffix"), 0),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("UnsignedSuffix"), ToUtf32("UnsignedSuffix"), 0)))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("UnsignedSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::CharParser('u'),
            new gendoc::parsing::CharParser('U'))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("LongLongSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::StringParser(ToUtf32("ll")),
            new gendoc::parsing::StringParser(ToUtf32("LL")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("LongSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::CharParser('l'),
            new gendoc::parsing::CharParser('L'))));
    AddRule(new CharacterLiteralRule(ToUtf32("CharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("NarrowCharacterLiteral"), ToUtf32("NarrowCharacterLiteral"), 0)),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("UniversalCharacterLiteral"), ToUtf32("UniversalCharacterLiteral"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("WideCharacterLiteral"), ToUtf32("WideCharacterLiteral"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("NarrowCharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('\''),
                    new gendoc::parsing::NonterminalParser(ToUtf32("CCharSequence"), ToUtf32("CCharSequence"), 0)),
                new gendoc::parsing::CharParser('\'')))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("UniversalCharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('u'),
                            new gendoc::parsing::CharParser('\'')),
                        new gendoc::parsing::NonterminalParser(ToUtf32("cs1"), ToUtf32("CCharSequence"), 0)),
                    new gendoc::parsing::CharParser('\''))),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('U'),
                            new gendoc::parsing::CharParser('\'')),
                        new gendoc::parsing::NonterminalParser(ToUtf32("cs2"), ToUtf32("CCharSequence"), 0)),
                    new gendoc::parsing::CharParser('\''))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("WideCharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('L'),
                        new gendoc::parsing::CharParser('\'')),
                    new gendoc::parsing::NonterminalParser(ToUtf32("CCharSequence"), ToUtf32("CCharSequence"), 0)),
                new gendoc::parsing::CharParser('\'')))));
    AddRule(new CCharSequenceRule(ToUtf32("CCharSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::PositiveParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("CChar"), ToUtf32("CChar"), 0))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("CChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::CharSetParser(ToUtf32("\'\\\n"), true),
            new gendoc::parsing::NonterminalParser(ToUtf32("EscapeSequence"), ToUtf32("EscapeSequence"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("EscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("SimpleEscapeSequence"), ToUtf32("SimpleEscapeSequence"), 0),
                new gendoc::parsing::NonterminalParser(ToUtf32("OctalEscapeSequence"), ToUtf32("OctalEscapeSequence"), 0)),
            new gendoc::parsing::NonterminalParser(ToUtf32("HexadecimalEscapeSequence"), ToUtf32("HexadecimalEscapeSequence"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SimpleEscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::StringParser(ToUtf32("\\\'")),
                                                new gendoc::parsing::StringParser(ToUtf32("\\\""))),
                                            new gendoc::parsing::StringParser(ToUtf32("\\?"))),
                                        new gendoc::parsing::StringParser(ToUtf32("\\\\"))),
                                    new gendoc::parsing::StringParser(ToUtf32("\\a"))),
                                new gendoc::parsing::StringParser(ToUtf32("\\b"))),
                            new gendoc::parsing::StringParser(ToUtf32("\\f"))),
                        new gendoc::parsing::StringParser(ToUtf32("\\n"))),
                    new gendoc::parsing::StringParser(ToUtf32("\\r"))),
                new gendoc::parsing::StringParser(ToUtf32("\\t"))),
            new gendoc::parsing::StringParser(ToUtf32("\\v")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("OctalEscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('\\'),
                    new gendoc::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0)),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0))),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("HexadecimalEscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::StringParser(ToUtf32("\\x")),
            new gendoc::parsing::PositiveParser(
                new gendoc::parsing::HexDigitParser()))));
    AddRule(new FloatingLiteralRule(ToUtf32("FloatingLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("FractionalConstant"), ToUtf32("FractionalConstant"), 0),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("FloatingSuffix"), ToUtf32("FloatingSuffix"), 0))))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0),
                            new gendoc::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0)),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("FloatingSuffix"), ToUtf32("FloatingSuffix"), 0))))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("FractionalConstant"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0)),
                    new gendoc::parsing::CharParser('.')),
                new gendoc::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0)),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0),
                new gendoc::parsing::CharParser('.')))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("DigitSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::DigitParser())));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ExponentPart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::CharParser('e'),
                        new gendoc::parsing::CharParser('E'))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Sign"), ToUtf32("Sign"), 0))),
            new gendoc::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Sign"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::CharParser('+'),
            new gendoc::parsing::CharParser('-'))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("FloatingSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::CharSetParser(ToUtf32("fFlL"))));
    AddRule(new StringLiteralRule(ToUtf32("StringLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("EncodingPrefix"), ToUtf32("EncodingPrefix"), 0)),
                            new gendoc::parsing::CharParser('\"')),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("SCharSequence"), ToUtf32("SCharSequence"), 0))),
                    new gendoc::parsing::CharParser('\"'))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("EncodingPrefix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::StringParser(ToUtf32("u8")),
                    new gendoc::parsing::CharParser('u')),
                new gendoc::parsing::CharParser('U')),
            new gendoc::parsing::CharParser('L'))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SCharSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::GroupingParser(
            new gendoc::parsing::PositiveParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("SChar"), ToUtf32("SChar"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::CharSetParser(ToUtf32("\"\\\n"), true),
            new gendoc::parsing::NonterminalParser(ToUtf32("EscapeSequence"), ToUtf32("EscapeSequence"), 0))));
    AddRule(new BooleanLiteralRule(ToUtf32("BooleanLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::KeywordParser(ToUtf32("true"))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::KeywordParser(ToUtf32("false"))))));
    AddRule(new PointerLiteralRule(ToUtf32("PointerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("nullptr")))));
}

} } // namespace gendoc.code
