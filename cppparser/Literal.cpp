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
#include <sstream>

namespace gendoc { namespace cppparser {

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

Literal::Literal(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Literal"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

LiteralNode* Literal::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    LiteralNode* result = *static_cast<gendoc::parsing::ValueObject<LiteralNode*>*>(value.get());
    stack.pop();
    return result;
}

class Literal::LiteralRule : public gendoc::parsing::Rule
{
public:
    LiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("LiteralNode*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<LiteralNode*>(context->value)));
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
        gendoc::parsing::NonterminalParser* floatingLiteralNonterminalParser = GetNonterminal(ToUtf32("FloatingLiteral"));
        floatingLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostFloatingLiteral));
        gendoc::parsing::NonterminalParser* integerLiteralNonterminalParser = GetNonterminal(ToUtf32("IntegerLiteral"));
        integerLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostIntegerLiteral));
        gendoc::parsing::NonterminalParser* characterLiteralNonterminalParser = GetNonterminal(ToUtf32("CharacterLiteral"));
        characterLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostCharacterLiteral));
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
        context->value = context->fromFloatingLiteral;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIntegerLiteral;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCharacterLiteral;
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
    void PostFloatingLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFloatingLiteral_value = std::move(stack.top());
            context->fromFloatingLiteral = *static_cast<gendoc::parsing::ValueObject<FloatingLiteralNode*>*>(fromFloatingLiteral_value.get());
            stack.pop();
        }
    }
    void PostIntegerLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIntegerLiteral_value = std::move(stack.top());
            context->fromIntegerLiteral = *static_cast<gendoc::parsing::ValueObject<IntegerLiteralNode*>*>(fromIntegerLiteral_value.get());
            stack.pop();
        }
    }
    void PostCharacterLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCharacterLiteral_value = std::move(stack.top());
            context->fromCharacterLiteral = *static_cast<gendoc::parsing::ValueObject<CharacterLiteralNode*>*>(fromCharacterLiteral_value.get());
            stack.pop();
        }
    }
    void PostStringLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStringLiteral_value = std::move(stack.top());
            context->fromStringLiteral = *static_cast<gendoc::parsing::ValueObject<StringLiteralNode*>*>(fromStringLiteral_value.get());
            stack.pop();
        }
    }
    void PostBooleanLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBooleanLiteral_value = std::move(stack.top());
            context->fromBooleanLiteral = *static_cast<gendoc::parsing::ValueObject<LiteralNode*>*>(fromBooleanLiteral_value.get());
            stack.pop();
        }
    }
    void PostPointerLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPointerLiteral_value = std::move(stack.top());
            context->fromPointerLiteral = *static_cast<gendoc::parsing::ValueObject<LiteralNode*>*>(fromPointerLiteral_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromFloatingLiteral(), fromIntegerLiteral(), fromCharacterLiteral(), fromStringLiteral(), fromBooleanLiteral(), fromPointerLiteral() {}
        LiteralNode* value;
        FloatingLiteralNode* fromFloatingLiteral;
        IntegerLiteralNode* fromIntegerLiteral;
        CharacterLiteralNode* fromCharacterLiteral;
        StringLiteralNode* fromStringLiteral;
        LiteralNode* fromBooleanLiteral;
        LiteralNode* fromPointerLiteral;
    };
};

class Literal::FloatingLiteralRule : public gendoc::parsing::Rule
{
public:
    FloatingLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("FloatingLiteralNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::string"), ToUtf32("m")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Suffix"), ToUtf32("suffix")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<FloatingLiteralNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A4Action));
        gendoc::parsing::NonterminalParser* s1NonterminalParser = GetNonterminal(ToUtf32("s1"));
        s1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FloatingLiteralRule>(this, &FloatingLiteralRule::Posts1));
        gendoc::parsing::NonterminalParser* s2NonterminalParser = GetNonterminal(ToUtf32("s2"));
        s2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FloatingLiteralRule>(this, &FloatingLiteralRule::Posts2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(context->m);
        double x;
        s >> x;
        context->value = new FloatingLiteralNode(span, x, context->suffix, std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->m = ToUtf8(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->suffix = context->froms1;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->m = ToUtf8(std::u32string(matchBegin, matchEnd));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->suffix = context->froms2;
    }
    void Posts1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms1_value = std::move(stack.top());
            context->froms1 = *static_cast<gendoc::parsing::ValueObject<Suffix>*>(froms1_value.get());
            stack.pop();
        }
    }
    void Posts2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms2_value = std::move(stack.top());
            context->froms2 = *static_cast<gendoc::parsing::ValueObject<Suffix>*>(froms2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), m(), suffix(), froms1(), froms2() {}
        FloatingLiteralNode* value;
        std::string m;
        Suffix suffix;
        Suffix froms1;
        Suffix froms2;
    };
};

class Literal::FloatingSuffixRule : public gendoc::parsing::Rule
{
public:
    FloatingSuffixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Suffix"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Suffix>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FloatingSuffixRule>(this, &FloatingSuffixRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FloatingSuffixRule>(this, &FloatingSuffixRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Suffix::f;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Suffix::l;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Suffix value;
    };
};

class Literal::IntegerLiteralRule : public gendoc::parsing::Rule
{
public:
    IntegerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("IntegerLiteralNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("uint64_t"), ToUtf32("x")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Suffix"), ToUtf32("suffix")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Base"), ToUtf32("base")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<IntegerLiteralNode*>(context->value)));
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
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A6Action));
        gendoc::parsing::NonterminalParser* hlNonterminalParser = GetNonterminal(ToUtf32("hl"));
        hlNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IntegerLiteralRule>(this, &IntegerLiteralRule::Posthl));
        gendoc::parsing::NonterminalParser* s3NonterminalParser = GetNonterminal(ToUtf32("s3"));
        s3NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IntegerLiteralRule>(this, &IntegerLiteralRule::Posts3));
        gendoc::parsing::NonterminalParser* olNonterminalParser = GetNonterminal(ToUtf32("ol"));
        olNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IntegerLiteralRule>(this, &IntegerLiteralRule::Postol));
        gendoc::parsing::NonterminalParser* s2NonterminalParser = GetNonterminal(ToUtf32("s2"));
        s2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IntegerLiteralRule>(this, &IntegerLiteralRule::Posts2));
        gendoc::parsing::NonterminalParser* dlNonterminalParser = GetNonterminal(ToUtf32("dl"));
        dlNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IntegerLiteralRule>(this, &IntegerLiteralRule::Postdl));
        gendoc::parsing::NonterminalParser* s1NonterminalParser = GetNonterminal(ToUtf32("s1"));
        s1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IntegerLiteralRule>(this, &IntegerLiteralRule::Posts1));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IntegerLiteralNode(span, context->x, context->suffix, context->base, std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->x = context->fromhl;
        context->base = Base::hex;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->suffix = context->froms3;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->x = context->fromol;
        context->base = Base::octal;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->suffix = context->froms2;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->x = context->fromdl;
        context->base = Base::decimal;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->suffix = context->froms1;
    }
    void Posthl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromhl_value = std::move(stack.top());
            context->fromhl = *static_cast<gendoc::parsing::ValueObject<uint64_t>*>(fromhl_value.get());
            stack.pop();
        }
    }
    void Posts3(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms3_value = std::move(stack.top());
            context->froms3 = *static_cast<gendoc::parsing::ValueObject<Suffix>*>(froms3_value.get());
            stack.pop();
        }
    }
    void Postol(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromol_value = std::move(stack.top());
            context->fromol = *static_cast<gendoc::parsing::ValueObject<uint64_t>*>(fromol_value.get());
            stack.pop();
        }
    }
    void Posts2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms2_value = std::move(stack.top());
            context->froms2 = *static_cast<gendoc::parsing::ValueObject<Suffix>*>(froms2_value.get());
            stack.pop();
        }
    }
    void Postdl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromdl_value = std::move(stack.top());
            context->fromdl = *static_cast<gendoc::parsing::ValueObject<uint64_t>*>(fromdl_value.get());
            stack.pop();
        }
    }
    void Posts1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms1_value = std::move(stack.top());
            context->froms1 = *static_cast<gendoc::parsing::ValueObject<Suffix>*>(froms1_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), x(), suffix(), base(), fromhl(), froms3(), fromol(), froms2(), fromdl(), froms1() {}
        IntegerLiteralNode* value;
        uint64_t x;
        Suffix suffix;
        Base base;
        uint64_t fromhl;
        Suffix froms3;
        uint64_t fromol;
        Suffix froms2;
        uint64_t fromdl;
        Suffix froms1;
    };
};

class Literal::DecimalLiteralRule : public gendoc::parsing::Rule
{
public:
    DecimalLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DecimalLiteralRule>(this, &DecimalLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> context->value;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        uint64_t value;
    };
};

class Literal::OctalLiteralRule : public gendoc::parsing::Rule
{
public:
    OctalLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OctalLiteralRule>(this, &OctalLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = 8 * context->value + *matchBegin - U'0';
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        uint64_t value;
    };
};

class Literal::HexadecimalLiteralRule : public gendoc::parsing::Rule
{
public:
    HexadecimalLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<HexadecimalLiteralRule>(this, &HexadecimalLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> std::hex >> context->value;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        uint64_t value;
    };
};

class Literal::IntegerSuffixRule : public gendoc::parsing::Rule
{
public:
    IntegerSuffixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Suffix"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Suffix>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerSuffixRule>(this, &IntegerSuffixRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerSuffixRule>(this, &IntegerSuffixRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerSuffixRule>(this, &IntegerSuffixRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerSuffixRule>(this, &IntegerSuffixRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerSuffixRule>(this, &IntegerSuffixRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerSuffixRule>(this, &IntegerSuffixRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerSuffixRule>(this, &IntegerSuffixRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IntegerSuffixRule>(this, &IntegerSuffixRule::A7Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Suffix::u;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | Suffix::ll;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Suffix::u;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | Suffix::l;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Suffix::ll;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | Suffix::u;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Suffix::l;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | Suffix::u;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Suffix value;
    };
};

class Literal::CharacterLiteralRule : public gendoc::parsing::Rule
{
public:
    CharacterLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("CharacterLiteralNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("char32_t"), ToUtf32("prefix")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("chars")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<CharacterLiteralNode*>(context->value)));
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
        gendoc::parsing::NonterminalParser* cCharNonterminalParser = GetNonterminal(ToUtf32("CChar"));
        cCharNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharacterLiteralRule>(this, &CharacterLiteralRule::PostCChar));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CharacterLiteralNode(span, context->prefix, context->chars, std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->prefix = *matchBegin;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->chars.append(1, context->fromCChar);
    }
    void PostCChar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCChar_value = std::move(stack.top());
            context->fromCChar = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromCChar_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), prefix(), chars(), fromCChar() {}
        CharacterLiteralNode* value;
        char32_t prefix;
        std::u32string chars;
        char32_t fromCChar;
    };
};

class Literal::CCharRule : public gendoc::parsing::Rule
{
public:
    CCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("char32_t"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<char32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CCharRule>(this, &CCharRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CCharRule>(this, &CCharRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CCharRule>(this, &CCharRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CCharRule>(this, &CCharRule::A3Action));
        gendoc::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CCharRule>(this, &CCharRule::Postescape));
        gendoc::parsing::NonterminalParser* hexDigit4NonterminalParser = GetNonterminal(ToUtf32("HexDigit4"));
        hexDigit4NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CCharRule>(this, &CCharRule::PostHexDigit4));
        gendoc::parsing::NonterminalParser* hexDigit8NonterminalParser = GetNonterminal(ToUtf32("HexDigit8"));
        hexDigit8NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CCharRule>(this, &CCharRule::PostHexDigit8));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = *matchBegin;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromescape;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromHexDigit4);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromHexDigit8);
    }
    void Postescape(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromescape_value = std::move(stack.top());
            context->fromescape = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromescape_value.get());
            stack.pop();
        }
    }
    void PostHexDigit4(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromHexDigit4_value = std::move(stack.top());
            context->fromHexDigit4 = *static_cast<gendoc::parsing::ValueObject<uint16_t>*>(fromHexDigit4_value.get());
            stack.pop();
        }
    }
    void PostHexDigit8(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromHexDigit8_value = std::move(stack.top());
            context->fromHexDigit8 = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromHexDigit8_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromescape(), fromHexDigit4(), fromHexDigit8() {}
        char32_t value;
        char32_t fromescape;
        uint16_t fromHexDigit4;
        uint32_t fromHexDigit8;
    };
};

class Literal::HexDigit4Rule : public gendoc::parsing::Rule
{
public:
    HexDigit4Rule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint16_t"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<uint16_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<HexDigit4Rule>(this, &HexDigit4Rule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> std::hex >> context->value;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        uint16_t value;
    };
};

class Literal::HexDigit8Rule : public gendoc::parsing::Rule
{
public:
    HexDigit8Rule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint32_t"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<uint32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<HexDigit8Rule>(this, &HexDigit8Rule::A0Action));
        gendoc::parsing::NonterminalParser* h1NonterminalParser = GetNonterminal(ToUtf32("h1"));
        h1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<HexDigit8Rule>(this, &HexDigit8Rule::Posth1));
        gendoc::parsing::NonterminalParser* h2NonterminalParser = GetNonterminal(ToUtf32("h2"));
        h2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<HexDigit8Rule>(this, &HexDigit8Rule::Posth2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> std::hex >> context->value;
    }
    void Posth1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromh1_value = std::move(stack.top());
            context->fromh1 = *static_cast<gendoc::parsing::ValueObject<uint16_t>*>(fromh1_value.get());
            stack.pop();
        }
    }
    void Posth2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromh2_value = std::move(stack.top());
            context->fromh2 = *static_cast<gendoc::parsing::ValueObject<uint16_t>*>(fromh2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromh1(), fromh2() {}
        uint32_t value;
        uint16_t fromh1;
        uint16_t fromh2;
    };
};

class Literal::StringLiteralRule : public gendoc::parsing::Rule
{
public:
    StringLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("StringLiteralNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("encodingPrefix")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("chars")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<StringLiteralNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A4Action));
        gendoc::parsing::NonterminalParser* e1NonterminalParser = GetNonterminal(ToUtf32("e1"));
        e1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StringLiteralRule>(this, &StringLiteralRule::Poste1));
        gendoc::parsing::NonterminalParser* sCharNonterminalParser = GetNonterminal(ToUtf32("SChar"));
        sCharNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StringLiteralRule>(this, &StringLiteralRule::PostSChar));
        gendoc::parsing::NonterminalParser* e2NonterminalParser = GetNonterminal(ToUtf32("e2"));
        e2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StringLiteralRule>(this, &StringLiteralRule::Poste2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new StringLiteralNode(span, context->encodingPrefix, context->chars, std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->encodingPrefix = context->frome1;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->chars.append(1, context->fromSChar);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->encodingPrefix = context->frome2;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->chars.append(1, *matchBegin);
    }
    void Poste1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frome1_value = std::move(stack.top());
            context->frome1 = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(frome1_value.get());
            stack.pop();
        }
    }
    void PostSChar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSChar_value = std::move(stack.top());
            context->fromSChar = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromSChar_value.get());
            stack.pop();
        }
    }
    void Poste2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frome2_value = std::move(stack.top());
            context->frome2 = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(frome2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), encodingPrefix(), chars(), frome1(), fromSChar(), frome2() {}
        StringLiteralNode* value;
        std::u32string encodingPrefix;
        std::u32string chars;
        std::u32string frome1;
        char32_t fromSChar;
        std::u32string frome2;
    };
};

class Literal::EncodingPrefixRule : public gendoc::parsing::Rule
{
public:
    EncodingPrefixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

class Literal::SCharRule : public gendoc::parsing::Rule
{
public:
    SCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("char32_t"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<char32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SCharRule>(this, &SCharRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SCharRule>(this, &SCharRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SCharRule>(this, &SCharRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SCharRule>(this, &SCharRule::A3Action));
        gendoc::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SCharRule>(this, &SCharRule::Postescape));
        gendoc::parsing::NonterminalParser* hexDigit4NonterminalParser = GetNonterminal(ToUtf32("HexDigit4"));
        hexDigit4NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SCharRule>(this, &SCharRule::PostHexDigit4));
        gendoc::parsing::NonterminalParser* hexDigit8NonterminalParser = GetNonterminal(ToUtf32("HexDigit8"));
        hexDigit8NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SCharRule>(this, &SCharRule::PostHexDigit8));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = *matchBegin;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromescape;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromHexDigit4);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromHexDigit8);
    }
    void Postescape(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromescape_value = std::move(stack.top());
            context->fromescape = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromescape_value.get());
            stack.pop();
        }
    }
    void PostHexDigit4(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromHexDigit4_value = std::move(stack.top());
            context->fromHexDigit4 = *static_cast<gendoc::parsing::ValueObject<uint16_t>*>(fromHexDigit4_value.get());
            stack.pop();
        }
    }
    void PostHexDigit8(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromHexDigit8_value = std::move(stack.top());
            context->fromHexDigit8 = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromHexDigit8_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromescape(), fromHexDigit4(), fromHexDigit8() {}
        char32_t value;
        char32_t fromescape;
        uint16_t fromHexDigit4;
        uint32_t fromHexDigit8;
    };
};

class Literal::BooleanLiteralRule : public gendoc::parsing::Rule
{
public:
    BooleanLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("LiteralNode*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<LiteralNode*>(context->value)));
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
        context->value = new BooleanLiteralNode(span, true, std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BooleanLiteralNode(span, false, std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        LiteralNode* value;
    };
};

class Literal::PointerLiteralRule : public gendoc::parsing::Rule
{
public:
    PointerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("LiteralNode*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<LiteralNode*>(context->value)));
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
        context->value = new NullPtrLiteralNode(span, std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        LiteralNode* value;
    };
};

void Literal::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Literal::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("digit_sequence"), this, ToUtf32("gendoc.parsing.stdlib.digit_sequence")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("escape"), this, ToUtf32("gendoc.parsing.stdlib.escape")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("sign"), this, ToUtf32("gendoc.parsing.stdlib.sign")));
    AddRule(new LiteralRule(ToUtf32("Literal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("FloatingLiteral"), ToUtf32("FloatingLiteral"), 0)),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("IntegerLiteral"), ToUtf32("IntegerLiteral"), 0))),
                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("CharacterLiteral"), ToUtf32("CharacterLiteral"), 0))),
                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("StringLiteral"), ToUtf32("StringLiteral"), 0))),
                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("BooleanLiteral"), ToUtf32("BooleanLiteral"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                new gendoc::parsing::NonterminalParser(ToUtf32("PointerLiteral"), ToUtf32("PointerLiteral"), 0)))));
    AddRule(new FloatingLiteralRule(ToUtf32("FloatingLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("FractionalConstant"), ToUtf32("FractionalConstant"), 0),
                                        new gendoc::parsing::OptionalParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0))))),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("FloatingSuffix"), 0)))))),
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0)))),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("s2"), ToUtf32("FloatingSuffix"), 0)))))))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("FractionalConstant"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0)),
                        new gendoc::parsing::CharParser('.')),
                    new gendoc::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0))),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0),
                    new gendoc::parsing::CharParser('.'))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ExponentPart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharSetParser(ToUtf32("eE")),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("sign"), ToUtf32("sign"), 0))),
                new gendoc::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0)))));
    AddRule(new FloatingSuffixRule(ToUtf32("FloatingSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::CharSetParser(ToUtf32("fF"))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::CharSetParser(ToUtf32("lL"))))));
    AddRule(new IntegerLiteralRule(ToUtf32("IntegerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::TokenParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("hl"), ToUtf32("HexadecimalLiteral"), 0)),
                                new gendoc::parsing::OptionalParser(
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("s3"), ToUtf32("IntegerSuffix"), 0)))))),
                        new gendoc::parsing::TokenParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("ol"), ToUtf32("OctalLiteral"), 0)),
                                new gendoc::parsing::OptionalParser(
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("s2"), ToUtf32("IntegerSuffix"), 0))))))),
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("dl"), ToUtf32("DecimalLiteral"), 0)),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("IntegerSuffix"), 0)))))))))));
    AddRule(new DecimalLiteralRule(ToUtf32("DecimalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharSetParser(ToUtf32("1-9")),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::DigitParser()))))));
    AddRule(new OctalLiteralRule(ToUtf32("OctalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('0'),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::CharSetParser(ToUtf32("0-7")))))))));
    AddRule(new HexadecimalLiteralRule(ToUtf32("HexadecimalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::StringParser(ToUtf32("0x")),
                        new gendoc::parsing::StringParser(ToUtf32("0X")))),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::PositiveParser(
                            new gendoc::parsing::HexDigitParser())))))));
    AddRule(new IntegerSuffixRule(ToUtf32("IntegerSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                new gendoc::parsing::CharSetParser(ToUtf32("uU"))),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::StringParser(ToUtf32("ll")),
                                                new gendoc::parsing::StringParser(ToUtf32("LL"))))))))),
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::CharSetParser(ToUtf32("uU"))),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                        new gendoc::parsing::CharSetParser(ToUtf32("lL")))))))),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::StringParser(ToUtf32("ll")),
                                    new gendoc::parsing::StringParser(ToUtf32("LL"))))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                    new gendoc::parsing::CharSetParser(ToUtf32("uU")))))))),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A6"),
                        new gendoc::parsing::CharSetParser(ToUtf32("lL"))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A7"),
                                new gendoc::parsing::CharSetParser(ToUtf32("uU"))))))))));
    AddRule(new CharacterLiteralRule(ToUtf32("CharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::CharSetParser(ToUtf32("uUL"))))),
                            new gendoc::parsing::CharParser('\'')),
                        new gendoc::parsing::PositiveParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("CChar"), ToUtf32("CChar"), 0))))),
                    new gendoc::parsing::CharParser('\''))))));
    AddRule(new CCharRule(ToUtf32("CChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::CharSetParser(ToUtf32("\r\n\\\'"), true)),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0))),
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('u'),
                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("HexDigit4"), ToUtf32("HexDigit4"), 0)))),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('U'),
                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("HexDigit8"), ToUtf32("HexDigit8"), 0))))));
    AddRule(new HexDigit4Rule(ToUtf32("HexDigit4"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::HexDigitParser(),
                            new gendoc::parsing::HexDigitParser()),
                        new gendoc::parsing::HexDigitParser()),
                    new gendoc::parsing::HexDigitParser())))));
    AddRule(new HexDigit8Rule(ToUtf32("HexDigit8"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("h1"), ToUtf32("HexDigit4"), 0),
                    new gendoc::parsing::NonterminalParser(ToUtf32("h2"), ToUtf32("HexDigit4"), 0))))));
    AddRule(new StringLiteralRule(ToUtf32("StringLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::OptionalParser(
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("e1"), ToUtf32("EncodingPrefix"), 0)))),
                                    new gendoc::parsing::CharParser('\"')),
                                new gendoc::parsing::KleeneStarParser(
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("SChar"), ToUtf32("SChar"), 0))))),
                            new gendoc::parsing::CharParser('\"'))),
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::OptionalParser(
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("e2"), ToUtf32("EncodingPrefix"), 0)))),
                                        new gendoc::parsing::CharParser('R')),
                                    new gendoc::parsing::CharParser('\"')),
                                new gendoc::parsing::KleeneStarParser(
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                            new gendoc::parsing::CharSetParser(ToUtf32("\r\n\""), true))))),
                            new gendoc::parsing::CharParser('\"'))))))));
    AddRule(new EncodingPrefixRule(ToUtf32("EncodingPrefix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::StringParser(ToUtf32("u8")),
                    new gendoc::parsing::CharParser('u')),
                new gendoc::parsing::CharParser('U')),
            new gendoc::parsing::CharParser('L'))));
    AddRule(new SCharRule(ToUtf32("SChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::CharSetParser(ToUtf32("\r\n\\\""), true)),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0))),
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('u'),
                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("HexDigit4"), ToUtf32("HexDigit4"), 0)))),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('U'),
                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("HexDigit8"), ToUtf32("HexDigit8"), 0))))));
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

} } // namespace gendoc.cppparser
