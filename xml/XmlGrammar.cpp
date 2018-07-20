#include "XmlGrammar.hpp"
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

namespace gendoc { namespace xml {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Xml* Xml::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Xml* Xml::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Xml* grammar(new Xml(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Xml::Xml(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Xml"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.xml")), parsingDomain_)
{
    SetOwner(0);
}

void Xml::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, XmlProcessor* processor)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<XmlProcessor*>(processor)));
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
}

class Xml::DocumentRule : public gendoc::parsing::Rule
{
public:
    DocumentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DocumentRule>(this, &DocumentRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DocumentRule>(this, &DocumentRule::A1Action));
        gendoc::parsing::NonterminalParser* prologNonterminalParser = GetNonterminal(ToUtf32("Prolog"));
        prologNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DocumentRule>(this, &DocumentRule::PreProlog));
        gendoc::parsing::NonterminalParser* elementNonterminalParser = GetNonterminal(ToUtf32("Element"));
        elementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DocumentRule>(this, &DocumentRule::PreElement));
        gendoc::parsing::NonterminalParser* miscNonterminalParser = GetNonterminal(ToUtf32("Misc"));
        miscNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DocumentRule>(this, &DocumentRule::PreMisc));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->StartDocument();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndDocument();
    }
    void PreProlog(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreElement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreMisc(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::NameRule : public gendoc::parsing::Rule
{
public:
    NameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NameRule>(this, &NameRule::A0Action));
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

class Xml::EntityValueRule : public gendoc::parsing::Rule
{
public:
    EntityValueRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A5Action));
        gendoc::parsing::NonterminalParser* pr1NonterminalParser = GetNonterminal(ToUtf32("pr1"));
        pr1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Prepr1));
        gendoc::parsing::NonterminalParser* ref1NonterminalParser = GetNonterminal(ToUtf32("ref1"));
        ref1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Preref1));
        gendoc::parsing::NonterminalParser* pr2NonterminalParser = GetNonterminal(ToUtf32("pr2"));
        pr2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Prepr2));
        gendoc::parsing::NonterminalParser* ref2NonterminalParser = GetNonterminal(ToUtf32("ref2"));
        ref2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Preref2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void Prepr1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Preref1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Prepr2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Preref2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), value() {}
        XmlProcessor* processor;
        std::u32string value;
    };
};

class Xml::AttValueRule : public gendoc::parsing::Rule
{
public:
    AttValueRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A5Action));
        gendoc::parsing::NonterminalParser* ref1NonterminalParser = GetNonterminal(ToUtf32("ref1"));
        ref1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AttValueRule>(this, &AttValueRule::Preref1));
        gendoc::parsing::NonterminalParser* ref2NonterminalParser = GetNonterminal(ToUtf32("ref2"));
        ref2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AttValueRule>(this, &AttValueRule::Preref2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->BeginAttributeValue(&context->value);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndAttributeValue();
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->BeginAttributeValue(&context->value);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndAttributeValue();
    }
    void Preref1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Preref2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), value() {}
        XmlProcessor* processor;
        std::u32string value;
    };
};

class Xml::SystemLiteralRule : public gendoc::parsing::Rule
{
public:
    SystemLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SystemLiteralRule>(this, &SystemLiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SystemLiteralRule>(this, &SystemLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
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

class Xml::PubidLiteralRule : public gendoc::parsing::Rule
{
public:
    PubidLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PubidLiteralRule>(this, &PubidLiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PubidLiteralRule>(this, &PubidLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
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

class Xml::CharDataRule : public gendoc::parsing::Rule
{
public:
    CharDataRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharDataRule>(this, &CharDataRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Text(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::CommentRule : public gendoc::parsing::Rule
{
public:
    CommentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CommentRule>(this, &CommentRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Comment(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::PIRule : public gendoc::parsing::Rule
{
public:
    PIRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("data")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PIRule>(this, &PIRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PIRule>(this, &PIRule::A1Action));
        gendoc::parsing::NonterminalParser* targetNonterminalParser = GetNonterminal(ToUtf32("target"));
        targetNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PIRule>(this, &PIRule::Posttarget));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->PI(context->fromtarget, context->data);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->data = std::u32string(matchBegin, matchEnd);
    }
    void Posttarget(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromtarget_value = std::move(stack.top());
            context->fromtarget = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromtarget_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), data(), fromtarget() {}
        XmlProcessor* processor;
        std::u32string data;
        std::u32string fromtarget;
    };
};

class Xml::PITargetRule : public gendoc::parsing::Rule
{
public:
    PITargetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PITargetRule>(this, &PITargetRule::A0Action));
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PITargetRule>(this, &PITargetRule::PostName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromName;
    }
    void PostName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromName() {}
        std::u32string value;
        std::u32string fromName;
    };
};

class Xml::CDSectRule : public gendoc::parsing::Rule
{
public:
    CDSectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CDSectRule>(this, &CDSectRule::A0Action));
        gendoc::parsing::NonterminalParser* cDataNonterminalParser = GetNonterminal(ToUtf32("CData"));
        cDataNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CDSectRule>(this, &CDSectRule::PostCData));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->CDataSection(context->fromCData);
    }
    void PostCData(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCData_value = std::move(stack.top());
            context->fromCData = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromCData_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromCData() {}
        XmlProcessor* processor;
        std::u32string fromCData;
    };
};

class Xml::CDataRule : public gendoc::parsing::Rule
{
public:
    CDataRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CDataRule>(this, &CDataRule::A0Action));
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

class Xml::PrologRule : public gendoc::parsing::Rule
{
public:
    PrologRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* xMLDeclNonterminalParser = GetNonterminal(ToUtf32("XMLDecl"));
        xMLDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrologRule>(this, &PrologRule::PreXMLDecl));
        gendoc::parsing::NonterminalParser* m1NonterminalParser = GetNonterminal(ToUtf32("m1"));
        m1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrologRule>(this, &PrologRule::Prem1));
        gendoc::parsing::NonterminalParser* docTypeDeclNonterminalParser = GetNonterminal(ToUtf32("DocTypeDecl"));
        docTypeDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrologRule>(this, &PrologRule::PreDocTypeDecl));
        gendoc::parsing::NonterminalParser* m2NonterminalParser = GetNonterminal(ToUtf32("m2"));
        m2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrologRule>(this, &PrologRule::Prem2));
    }
    void PreXMLDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Prem1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreDocTypeDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Prem2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::XMLDeclRule : public gendoc::parsing::Rule
{
public:
    XMLDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* versionInfoNonterminalParser = GetNonterminal(ToUtf32("VersionInfo"));
        versionInfoNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<XMLDeclRule>(this, &XMLDeclRule::PreVersionInfo));
        gendoc::parsing::NonterminalParser* encodingDeclNonterminalParser = GetNonterminal(ToUtf32("EncodingDecl"));
        encodingDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<XMLDeclRule>(this, &XMLDeclRule::PreEncodingDecl));
        gendoc::parsing::NonterminalParser* sDDeclNonterminalParser = GetNonterminal(ToUtf32("SDDecl"));
        sDDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<XMLDeclRule>(this, &XMLDeclRule::PreSDDecl));
    }
    void PreVersionInfo(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreEncodingDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreSDDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::VersionInfoRule : public gendoc::parsing::Rule
{
public:
    VersionInfoRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<VersionInfoRule>(this, &VersionInfoRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<VersionInfoRule>(this, &VersionInfoRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Version(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Version(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::MiscRule : public gendoc::parsing::Rule
{
public:
    MiscRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* commentNonterminalParser = GetNonterminal(ToUtf32("Comment"));
        commentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MiscRule>(this, &MiscRule::PreComment));
        gendoc::parsing::NonterminalParser* pINonterminalParser = GetNonterminal(ToUtf32("PI"));
        pINonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MiscRule>(this, &MiscRule::PrePI));
    }
    void PreComment(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePI(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::DocTypeDeclRule : public gendoc::parsing::Rule
{
public:
    DocTypeDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* rootElementNameNonterminalParser = GetNonterminal(ToUtf32("rootElementName"));
        rootElementNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DocTypeDeclRule>(this, &DocTypeDeclRule::PostrootElementName));
        gendoc::parsing::NonterminalParser* intSubsetNonterminalParser = GetNonterminal(ToUtf32("IntSubset"));
        intSubsetNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DocTypeDeclRule>(this, &DocTypeDeclRule::PreIntSubset));
    }
    void PostrootElementName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromrootElementName_value = std::move(stack.top());
            context->fromrootElementName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromrootElementName_value.get());
            stack.pop();
        }
    }
    void PreIntSubset(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromrootElementName() {}
        XmlProcessor* processor;
        std::u32string fromrootElementName;
    };
};

class Xml::DeclSepRule : public gendoc::parsing::Rule
{
public:
    DeclSepRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* pEReferenceNonterminalParser = GetNonterminal(ToUtf32("PEReference"));
        pEReferenceNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclSepRule>(this, &DeclSepRule::PrePEReference));
    }
    void PrePEReference(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::IntSubsetRule : public gendoc::parsing::Rule
{
public:
    IntSubsetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* markupDeclNonterminalParser = GetNonterminal(ToUtf32("MarkupDecl"));
        markupDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IntSubsetRule>(this, &IntSubsetRule::PreMarkupDecl));
        gendoc::parsing::NonterminalParser* declSepNonterminalParser = GetNonterminal(ToUtf32("DeclSep"));
        declSepNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IntSubsetRule>(this, &IntSubsetRule::PreDeclSep));
    }
    void PreMarkupDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreDeclSep(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::MarkupDeclRule : public gendoc::parsing::Rule
{
public:
    MarkupDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* elementDeclNonterminalParser = GetNonterminal(ToUtf32("ElementDecl"));
        elementDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreElementDecl));
        gendoc::parsing::NonterminalParser* attlistDeclNonterminalParser = GetNonterminal(ToUtf32("AttlistDecl"));
        attlistDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreAttlistDecl));
        gendoc::parsing::NonterminalParser* entityDeclNonterminalParser = GetNonterminal(ToUtf32("EntityDecl"));
        entityDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreEntityDecl));
        gendoc::parsing::NonterminalParser* notationDeclNonterminalParser = GetNonterminal(ToUtf32("NotationDecl"));
        notationDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreNotationDecl));
        gendoc::parsing::NonterminalParser* pINonterminalParser = GetNonterminal(ToUtf32("PI"));
        pINonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PrePI));
        gendoc::parsing::NonterminalParser* commentNonterminalParser = GetNonterminal(ToUtf32("Comment"));
        commentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreComment));
    }
    void PreElementDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreAttlistDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreEntityDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreNotationDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePI(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreComment(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::ExtSubsetRule : public gendoc::parsing::Rule
{
public:
    ExtSubsetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* textDeclNonterminalParser = GetNonterminal(ToUtf32("TextDecl"));
        textDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExtSubsetRule>(this, &ExtSubsetRule::PreTextDecl));
        gendoc::parsing::NonterminalParser* extSubsetDeclNonterminalParser = GetNonterminal(ToUtf32("ExtSubsetDecl"));
        extSubsetDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExtSubsetRule>(this, &ExtSubsetRule::PreExtSubsetDecl));
    }
    void PreTextDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreExtSubsetDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::ExtSubsetDeclRule : public gendoc::parsing::Rule
{
public:
    ExtSubsetDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* markupDeclNonterminalParser = GetNonterminal(ToUtf32("MarkupDecl"));
        markupDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExtSubsetDeclRule>(this, &ExtSubsetDeclRule::PreMarkupDecl));
        gendoc::parsing::NonterminalParser* conditionalSectNonterminalParser = GetNonterminal(ToUtf32("ConditionalSect"));
        conditionalSectNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExtSubsetDeclRule>(this, &ExtSubsetDeclRule::PreConditionalSect));
        gendoc::parsing::NonterminalParser* declSepNonterminalParser = GetNonterminal(ToUtf32("DeclSep"));
        declSepNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExtSubsetDeclRule>(this, &ExtSubsetDeclRule::PreDeclSep));
    }
    void PreMarkupDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreConditionalSect(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreDeclSep(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::SDDeclRule : public gendoc::parsing::Rule
{
public:
    SDDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("standalone")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A4Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Standalone(context->standalone);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = true;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = false;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), standalone() {}
        XmlProcessor* processor;
        bool standalone;
    };
};

class Xml::ElementRule : public gendoc::parsing::Rule
{
public:
    ElementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("tagName")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ElementRule>(this, &ElementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ElementRule>(this, &ElementRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ElementRule>(this, &ElementRule::A2Action));
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ElementRule>(this, &ElementRule::PostName));
        gendoc::parsing::NonterminalParser* attributeNonterminalParser = GetNonterminal(ToUtf32("Attribute"));
        attributeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ElementRule>(this, &ElementRule::PreAttribute));
        gendoc::parsing::NonterminalParser* contentNonterminalParser = GetNonterminal(ToUtf32("Content"));
        contentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ElementRule>(this, &ElementRule::PreContent));
        gendoc::parsing::NonterminalParser* eTagNonterminalParser = GetNonterminal(ToUtf32("ETag"));
        eTagNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ElementRule>(this, &ElementRule::PreETag));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->tagName = context->fromName;
        context->processor->BeginStartTag(context->tagName);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndStartTag(span, fileName);
        context->processor->EndTag(context->tagName, span, fileName);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndStartTag(span, fileName);
    }
    void PostName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
    void PreAttribute(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreContent(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreETag(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), tagName(), fromName() {}
        XmlProcessor* processor;
        std::u32string tagName;
        std::u32string fromName;
    };
};

class Xml::AttributeRule : public gendoc::parsing::Rule
{
public:
    AttributeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AttributeRule>(this, &AttributeRule::A0Action));
        gendoc::parsing::NonterminalParser* attNameNonterminalParser = GetNonterminal(ToUtf32("attName"));
        attNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AttributeRule>(this, &AttributeRule::PostattName));
        gendoc::parsing::NonterminalParser* attValueNonterminalParser = GetNonterminal(ToUtf32("attValue"));
        attValueNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AttributeRule>(this, &AttributeRule::PreattValue));
        attValueNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AttributeRule>(this, &AttributeRule::PostattValue));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->AddAttribute(context->fromattName, context->fromattValue, span, fileName);
    }
    void PostattName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromattName_value = std::move(stack.top());
            context->fromattName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromattName_value.get());
            stack.pop();
        }
    }
    void PreattValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostattValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromattValue_value = std::move(stack.top());
            context->fromattValue = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromattValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromattName(), fromattValue() {}
        XmlProcessor* processor;
        std::u32string fromattName;
        std::u32string fromattValue;
    };
};

class Xml::ETagRule : public gendoc::parsing::Rule
{
public:
    ETagRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ETagRule>(this, &ETagRule::A0Action));
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ETagRule>(this, &ETagRule::PostName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndTag(context->fromName, span, fileName);
    }
    void PostName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::ContentRule : public gendoc::parsing::Rule
{
public:
    ContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* cd1NonterminalParser = GetNonterminal(ToUtf32("cd1"));
        cd1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ContentRule>(this, &ContentRule::Precd1));
        gendoc::parsing::NonterminalParser* elementNonterminalParser = GetNonterminal(ToUtf32("Element"));
        elementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreElement));
        gendoc::parsing::NonterminalParser* referenceNonterminalParser = GetNonterminal(ToUtf32("Reference"));
        referenceNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreReference));
        gendoc::parsing::NonterminalParser* cDSectNonterminalParser = GetNonterminal(ToUtf32("CDSect"));
        cDSectNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreCDSect));
        gendoc::parsing::NonterminalParser* pINonterminalParser = GetNonterminal(ToUtf32("PI"));
        pINonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PrePI));
        gendoc::parsing::NonterminalParser* commentNonterminalParser = GetNonterminal(ToUtf32("Comment"));
        commentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreComment));
        gendoc::parsing::NonterminalParser* cd2NonterminalParser = GetNonterminal(ToUtf32("cd2"));
        cd2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ContentRule>(this, &ContentRule::Precd2));
    }
    void Precd1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreElement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreReference(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreCDSect(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePI(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreComment(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Precd2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::ElementDeclRule : public gendoc::parsing::Rule
{
public:
    ElementDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* elementNameNonterminalParser = GetNonterminal(ToUtf32("elementName"));
        elementNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ElementDeclRule>(this, &ElementDeclRule::PostelementName));
    }
    void PostelementName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromelementName_value = std::move(stack.top());
            context->fromelementName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromelementName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromelementName() {}
        XmlProcessor* processor;
        std::u32string fromelementName;
    };
};

class Xml::AttlistDeclRule : public gendoc::parsing::Rule
{
public:
    AttlistDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AttlistDeclRule>(this, &AttlistDeclRule::PostName));
        gendoc::parsing::NonterminalParser* attDefNonterminalParser = GetNonterminal(ToUtf32("AttDef"));
        attDefNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AttlistDeclRule>(this, &AttlistDeclRule::PreAttDef));
    }
    void PostName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
    void PreAttDef(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::AttDefRule : public gendoc::parsing::Rule
{
public:
    AttDefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AttDefRule>(this, &AttDefRule::PostName));
        gendoc::parsing::NonterminalParser* defaultDeclNonterminalParser = GetNonterminal(ToUtf32("DefaultDecl"));
        defaultDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AttDefRule>(this, &AttDefRule::PreDefaultDecl));
    }
    void PostName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
    void PreDefaultDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::DefaultDeclRule : public gendoc::parsing::Rule
{
public:
    DefaultDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* attValueNonterminalParser = GetNonterminal(ToUtf32("AttValue"));
        attValueNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DefaultDeclRule>(this, &DefaultDeclRule::PreAttValue));
        attValueNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DefaultDeclRule>(this, &DefaultDeclRule::PostAttValue));
    }
    void PreAttValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostAttValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAttValue_value = std::move(stack.top());
            context->fromAttValue = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromAttValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromAttValue() {}
        XmlProcessor* processor;
        std::u32string fromAttValue;
    };
};

class Xml::ConditionalSectRule : public gendoc::parsing::Rule
{
public:
    ConditionalSectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* includeSectNonterminalParser = GetNonterminal(ToUtf32("IncludeSect"));
        includeSectNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConditionalSectRule>(this, &ConditionalSectRule::PreIncludeSect));
    }
    void PreIncludeSect(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::IncludeSectRule : public gendoc::parsing::Rule
{
public:
    IncludeSectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* extSubsetDeclNonterminalParser = GetNonterminal(ToUtf32("ExtSubsetDecl"));
        extSubsetDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IncludeSectRule>(this, &IncludeSectRule::PreExtSubsetDecl));
    }
    void PreExtSubsetDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::CharRefRule : public gendoc::parsing::Rule
{
public:
    CharRefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharRefRule>(this, &CharRefRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharRefRule>(this, &CharRefRule::A1Action));
        gendoc::parsing::NonterminalParser* dcrNonterminalParser = GetNonterminal(ToUtf32("dcr"));
        dcrNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharRefRule>(this, &CharRefRule::Postdcr));
        gendoc::parsing::NonterminalParser* hcrNonterminalParser = GetNonterminal(ToUtf32("hcr"));
        hcrNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharRefRule>(this, &CharRefRule::Posthcr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = static_cast<char32_t>(context->fromdcr);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = static_cast<char32_t>(context->fromhcr);
    }
    void Postdcr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromdcr_value = std::move(stack.top());
            context->fromdcr = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromdcr_value.get());
            stack.pop();
        }
    }
    void Posthcr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromhcr_value = std::move(stack.top());
            context->fromhcr = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromhcr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromdcr(), fromhcr() {}
        char32_t value;
        uint32_t fromdcr;
        uint32_t fromhcr;
    };
};

class Xml::ReferenceRule : public gendoc::parsing::Rule
{
public:
    ReferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ReferenceRule>(this, &ReferenceRule::A0Action));
        gendoc::parsing::NonterminalParser* entityRefNonterminalParser = GetNonterminal(ToUtf32("EntityRef"));
        entityRefNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ReferenceRule>(this, &ReferenceRule::PreEntityRef));
        gendoc::parsing::NonterminalParser* charRefNonterminalParser = GetNonterminal(ToUtf32("CharRef"));
        charRefNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ReferenceRule>(this, &ReferenceRule::PostCharRef));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Text(std::u32string(1, context->fromCharRef));
    }
    void PreEntityRef(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostCharRef(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCharRef_value = std::move(stack.top());
            context->fromCharRef = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromCharRef_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromCharRef() {}
        XmlProcessor* processor;
        char32_t fromCharRef;
    };
};

class Xml::EntityRefRule : public gendoc::parsing::Rule
{
public:
    EntityRefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EntityRefRule>(this, &EntityRefRule::A0Action));
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EntityRefRule>(this, &EntityRefRule::PostName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EntityRef(context->fromName, span, fileName);
    }
    void PostName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::PEReferenceRule : public gendoc::parsing::Rule
{
public:
    PEReferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PEReferenceRule>(this, &PEReferenceRule::PostName));
    }
    void PostName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::EntityDeclRule : public gendoc::parsing::Rule
{
public:
    EntityDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* gEDeclNonterminalParser = GetNonterminal(ToUtf32("GEDecl"));
        gEDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EntityDeclRule>(this, &EntityDeclRule::PreGEDecl));
        gendoc::parsing::NonterminalParser* pEDeclNonterminalParser = GetNonterminal(ToUtf32("PEDecl"));
        pEDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EntityDeclRule>(this, &EntityDeclRule::PrePEDecl));
    }
    void PreGEDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePEDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::GEDeclRule : public gendoc::parsing::Rule
{
public:
    GEDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* entityNameNonterminalParser = GetNonterminal(ToUtf32("entityName"));
        entityNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GEDeclRule>(this, &GEDeclRule::PostentityName));
        gendoc::parsing::NonterminalParser* entityValueNonterminalParser = GetNonterminal(ToUtf32("entityValue"));
        entityValueNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GEDeclRule>(this, &GEDeclRule::PreentityValue));
    }
    void PostentityName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromentityName_value = std::move(stack.top());
            context->fromentityName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromentityName_value.get());
            stack.pop();
        }
    }
    void PreentityValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromentityName() {}
        XmlProcessor* processor;
        std::u32string fromentityName;
    };
};

class Xml::PEDeclRule : public gendoc::parsing::Rule
{
public:
    PEDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* peNameNonterminalParser = GetNonterminal(ToUtf32("peName"));
        peNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PEDeclRule>(this, &PEDeclRule::PostpeName));
        gendoc::parsing::NonterminalParser* peValueNonterminalParser = GetNonterminal(ToUtf32("peValue"));
        peValueNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PEDeclRule>(this, &PEDeclRule::PrepeValue));
    }
    void PostpeName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frompeName_value = std::move(stack.top());
            context->frompeName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(frompeName_value.get());
            stack.pop();
        }
    }
    void PrepeValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), frompeName() {}
        XmlProcessor* processor;
        std::u32string frompeName;
    };
};

class Xml::EntityDefRule : public gendoc::parsing::Rule
{
public:
    EntityDefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* entityValueNonterminalParser = GetNonterminal(ToUtf32("EntityValue"));
        entityValueNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EntityDefRule>(this, &EntityDefRule::PreEntityValue));
        entityValueNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EntityDefRule>(this, &EntityDefRule::PostEntityValue));
    }
    void PreEntityValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostEntityValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEntityValue_value = std::move(stack.top());
            context->fromEntityValue = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromEntityValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromEntityValue() {}
        XmlProcessor* processor;
        std::u32string fromEntityValue;
    };
};

class Xml::PEDefRule : public gendoc::parsing::Rule
{
public:
    PEDefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* entityValueNonterminalParser = GetNonterminal(ToUtf32("EntityValue"));
        entityValueNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PEDefRule>(this, &PEDefRule::PreEntityValue));
        entityValueNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PEDefRule>(this, &PEDefRule::PostEntityValue));
    }
    void PreEntityValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostEntityValue(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEntityValue_value = std::move(stack.top());
            context->fromEntityValue = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromEntityValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromEntityValue() {}
        XmlProcessor* processor;
        std::u32string fromEntityValue;
    };
};

class Xml::TextDeclRule : public gendoc::parsing::Rule
{
public:
    TextDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* versionInfoNonterminalParser = GetNonterminal(ToUtf32("VersionInfo"));
        versionInfoNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TextDeclRule>(this, &TextDeclRule::PreVersionInfo));
        gendoc::parsing::NonterminalParser* encodingDeclNonterminalParser = GetNonterminal(ToUtf32("EncodingDecl"));
        encodingDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TextDeclRule>(this, &TextDeclRule::PreEncodingDecl));
    }
    void PreVersionInfo(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreEncodingDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::ExtParsedEntRule : public gendoc::parsing::Rule
{
public:
    ExtParsedEntRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* textDeclNonterminalParser = GetNonterminal(ToUtf32("TextDecl"));
        textDeclNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExtParsedEntRule>(this, &ExtParsedEntRule::PreTextDecl));
        gendoc::parsing::NonterminalParser* contentNonterminalParser = GetNonterminal(ToUtf32("Content"));
        contentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExtParsedEntRule>(this, &ExtParsedEntRule::PreContent));
    }
    void PreTextDecl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreContent(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::EncodingDeclRule : public gendoc::parsing::Rule
{
public:
    EncodingDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EncodingDeclRule>(this, &EncodingDeclRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EncodingDeclRule>(this, &EncodingDeclRule::A1Action));
        gendoc::parsing::NonterminalParser* en1NonterminalParser = GetNonterminal(ToUtf32("en1"));
        en1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EncodingDeclRule>(this, &EncodingDeclRule::Posten1));
        gendoc::parsing::NonterminalParser* en2NonterminalParser = GetNonterminal(ToUtf32("en2"));
        en2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EncodingDeclRule>(this, &EncodingDeclRule::Posten2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Encoding(context->fromen1);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Encoding(context->fromen2);
    }
    void Posten1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromen1_value = std::move(stack.top());
            context->fromen1 = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromen1_value.get());
            stack.pop();
        }
    }
    void Posten2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromen2_value = std::move(stack.top());
            context->fromen2 = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromen2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromen1(), fromen2() {}
        XmlProcessor* processor;
        std::u32string fromen1;
        std::u32string fromen2;
    };
};

class Xml::EncNameRule : public gendoc::parsing::Rule
{
public:
    EncNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EncNameRule>(this, &EncNameRule::A0Action));
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

class Xml::NotationDeclRule : public gendoc::parsing::Rule
{
public:
    NotationDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<gendoc::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NotationDeclRule>(this, &NotationDeclRule::PostName));
    }
    void PostName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

void Xml::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Xml::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("uint"), this, ToUtf32("gendoc.parsing.stdlib.uint")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("hexuint"), this, ToUtf32("gendoc.parsing.stdlib.hexuint")));
    AddRule(new DocumentRule(ToUtf32("Document"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::EmptyParser()),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Prolog"), ToUtf32("Prolog"), 1),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Element"), ToUtf32("Element"), 1)),
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Misc"), ToUtf32("Misc"), 1))))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Char"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::CharSetParser(ToUtf32("\t\n\r")),
                    new gendoc::parsing::RangeParser(32, 55295)),
                new gendoc::parsing::RangeParser(57344, 65533)),
            new gendoc::parsing::RangeParser(65536, 1114111))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("S"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::CharSetParser(ToUtf32(" \t\r\n")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("NameStartChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                new gendoc::parsing::AlternativeParser(
                                                    new gendoc::parsing::AlternativeParser(
                                                        new gendoc::parsing::AlternativeParser(
                                                            new gendoc::parsing::CharParser(':'),
                                                            new gendoc::parsing::CharSetParser(ToUtf32("A-Z_a-z"))),
                                                        new gendoc::parsing::RangeParser(192, 214)),
                                                    new gendoc::parsing::RangeParser(216, 246)),
                                                new gendoc::parsing::RangeParser(248, 767)),
                                            new gendoc::parsing::RangeParser(880, 893)),
                                        new gendoc::parsing::RangeParser(895, 8191)),
                                    new gendoc::parsing::RangeParser(8204, 8205)),
                                new gendoc::parsing::RangeParser(8304, 8591)),
                            new gendoc::parsing::RangeParser(11264, 12271)),
                        new gendoc::parsing::RangeParser(12289, 55295)),
                    new gendoc::parsing::RangeParser(63744, 64975)),
                new gendoc::parsing::RangeParser(65008, 65533)),
            new gendoc::parsing::RangeParser(65536, 983039))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("NameChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("NameStartChar"), ToUtf32("NameStartChar"), 0),
                        new gendoc::parsing::CharSetParser(ToUtf32("-.0-9"))),
                    new gendoc::parsing::RangeParser(183, 183)),
                new gendoc::parsing::RangeParser(768, 879)),
            new gendoc::parsing::RangeParser(8255, 8256))));
    AddRule(new NameRule(ToUtf32("Name"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("NameStartChar"), ToUtf32("NameStartChar"), 0),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("NameChar"), ToUtf32("NameChar"), 0)))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Names"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("f"), ToUtf32("Name"), 0),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser(' '),
                        new gendoc::parsing::NonterminalParser(ToUtf32("n"), ToUtf32("Name"), 0)))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Nmtoken"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("NameChar"), ToUtf32("NameChar"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Nmtokens"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser(' '),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0)))))));
    AddRule(new EntityValueRule(ToUtf32("EntityValue"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('\"'),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                        new gendoc::parsing::CharSetParser(ToUtf32("%&\""), true)),
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("pr1"), ToUtf32("PEReference"), 1))),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("ref1"), ToUtf32("Reference"), 1)))))),
                new gendoc::parsing::CharParser('\"')),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('\''),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                        new gendoc::parsing::CharSetParser(ToUtf32("%&\'"), true)),
                                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("pr2"), ToUtf32("PEReference"), 1))),
                                new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("ref2"), ToUtf32("Reference"), 1)))))),
                new gendoc::parsing::CharParser('\'')))));
    AddRule(new AttValueRule(ToUtf32("AttValue"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::CharParser('\"')),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::CharSetParser(ToUtf32("<&\""), true)),
                                new gendoc::parsing::NonterminalParser(ToUtf32("ref1"), ToUtf32("Reference"), 1))))),
                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                    new gendoc::parsing::CharParser('\"'))),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                        new gendoc::parsing::CharParser('\'')),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                    new gendoc::parsing::CharSetParser(ToUtf32("<&\'"), true)),
                                new gendoc::parsing::NonterminalParser(ToUtf32("ref2"), ToUtf32("Reference"), 1))))),
                new gendoc::parsing::ActionParser(ToUtf32("A5"),
                    new gendoc::parsing::CharParser('\''))))));
    AddRule(new SystemLiteralRule(ToUtf32("SystemLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('\"'),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::KleeneStarParser(
                                    new gendoc::parsing::CharSetParser(ToUtf32("\""), true))))),
                    new gendoc::parsing::CharParser('\"'))),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('\''),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::KleeneStarParser(
                                    new gendoc::parsing::CharSetParser(ToUtf32("\'"), true))))),
                    new gendoc::parsing::CharParser('\''))))));
    AddRule(new PubidLiteralRule(ToUtf32("PubidLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('\"'),
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::KleeneStarParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("PubidChar"), ToUtf32("PubidChar"), 0))))),
                new gendoc::parsing::CharParser('\"')),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('\''),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::KleeneStarParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::DifferenceParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("PubidChar"), ToUtf32("PubidChar"), 0),
                                        new gendoc::parsing::CharParser('\''))))))),
                new gendoc::parsing::CharParser('\'')))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("PubidChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::CharSetParser(ToUtf32(" \r\n")),
                new gendoc::parsing::CharSetParser(ToUtf32("a-zA-Z0-9"))),
            new gendoc::parsing::CharSetParser(ToUtf32("-\'()+,./:=?;!*#@$_%")))));
    AddRule(new CharDataRule(ToUtf32("CharData"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::DifferenceParser(
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::CharSetParser(ToUtf32("<&"), true)),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KleeneStarParser(
                                    new gendoc::parsing::CharSetParser(ToUtf32("<&"), true)),
                                new gendoc::parsing::StringParser(ToUtf32("]]>"))),
                            new gendoc::parsing::KleeneStarParser(
                                new gendoc::parsing::CharSetParser(ToUtf32("<&"), true)))))))));
    AddRule(new CommentRule(ToUtf32("Comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::StringParser(ToUtf32("<!--")),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::DifferenceParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                                            new gendoc::parsing::CharParser('-'))),
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::CharParser('-'),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::DifferenceParser(
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                                                    new gendoc::parsing::CharParser('-'))))))))))),
            new gendoc::parsing::StringParser(ToUtf32("-->")))));
    AddRule(new PIRule(ToUtf32("PI"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::StringParser(ToUtf32("<?")),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("target"), ToUtf32("PITarget"), 0),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::KleeneStarParser(
                                                    new gendoc::parsing::GroupingParser(
                                                        new gendoc::parsing::DifferenceParser(
                                                            new gendoc::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                                                            new gendoc::parsing::StringParser(ToUtf32("?>")))))))))))))),
            new gendoc::parsing::StringParser(ToUtf32("?>")))));
    AddRule(new PITargetRule(ToUtf32("PITarget"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::DifferenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::CharParser('X'),
                                        new gendoc::parsing::CharParser('x'))),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::CharParser('M'),
                                        new gendoc::parsing::CharParser('m')))),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::CharParser('L'),
                                    new gendoc::parsing::CharParser('l'))))))))));
    AddRule(new CDSectRule(ToUtf32("CDSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("CDStart"), ToUtf32("CDStart"), 0),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("CData"), ToUtf32("CData"), 0))),
            new gendoc::parsing::NonterminalParser(ToUtf32("CDEnd"), ToUtf32("CDEnd"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("CDStart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::StringParser(ToUtf32("<![CDATA["))));
    AddRule(new CDataRule(ToUtf32("CData"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::DifferenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                            new gendoc::parsing::StringParser(ToUtf32("]]>")))))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("CDEnd"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::StringParser(ToUtf32("]]>"))));
    AddRule(new PrologRule(ToUtf32("Prolog"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("XMLDecl"), ToUtf32("XMLDecl"), 1)),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("m1"), ToUtf32("Misc"), 1))),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("DocTypeDecl"), ToUtf32("DocTypeDecl"), 1),
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("m2"), ToUtf32("Misc"), 1))))))));
    AddRule(new XMLDeclRule(ToUtf32("XMLDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::StringParser(ToUtf32("<?xml")),
                            new gendoc::parsing::NonterminalParser(ToUtf32("VersionInfo"), ToUtf32("VersionInfo"), 1)),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("EncodingDecl"), ToUtf32("EncodingDecl"), 1))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("SDDecl"), ToUtf32("SDDecl"), 1))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::StringParser(ToUtf32("?>")))));
    AddRule(new VersionInfoRule(ToUtf32("VersionInfo"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                    new gendoc::parsing::StringParser(ToUtf32("version"))),
                new gendoc::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('\''),
                            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("VersionNum"), ToUtf32("VersionNum"), 0))),
                        new gendoc::parsing::CharParser('\'')),
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('\"'),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("VersionNum"), ToUtf32("VersionNum"), 0))),
                        new gendoc::parsing::CharParser('\"')))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Eq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                new gendoc::parsing::CharParser('=')),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("VersionNum"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::StringParser(ToUtf32("1.")),
            new gendoc::parsing::PositiveParser(
                new gendoc::parsing::CharSetParser(ToUtf32("0-9"))))));
    AddRule(new MiscRule(ToUtf32("Misc"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 1),
                new gendoc::parsing::NonterminalParser(ToUtf32("PI"), ToUtf32("PI"), 1)),
            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))));
    AddRule(new DocTypeDeclRule(ToUtf32("DocTypeDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("<!DOCTYPE")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new gendoc::parsing::NonterminalParser(ToUtf32("rootElementName"), ToUtf32("Name"), 0)),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0))))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::CharParser('['),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("IntSubset"), ToUtf32("IntSubset"), 1)),
                                new gendoc::parsing::CharParser(']')),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)))))),
            new gendoc::parsing::CharParser('>'))));
    AddRule(new DeclSepRule(ToUtf32("DeclSep"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("PEReference"), ToUtf32("PEReference"), 1),
            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))));
    AddRule(new IntSubsetRule(ToUtf32("IntSubset"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("MarkupDecl"), ToUtf32("MarkupDecl"), 1),
                    new gendoc::parsing::NonterminalParser(ToUtf32("DeclSep"), ToUtf32("DeclSep"), 1))))));
    AddRule(new MarkupDeclRule(ToUtf32("MarkupDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("ElementDecl"), ToUtf32("ElementDecl"), 1),
                            new gendoc::parsing::NonterminalParser(ToUtf32("AttlistDecl"), ToUtf32("AttlistDecl"), 1)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("EntityDecl"), ToUtf32("EntityDecl"), 1)),
                    new gendoc::parsing::NonterminalParser(ToUtf32("NotationDecl"), ToUtf32("NotationDecl"), 1)),
                new gendoc::parsing::NonterminalParser(ToUtf32("PI"), ToUtf32("PI"), 1)),
            new gendoc::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 1))));
    AddRule(new ExtSubsetRule(ToUtf32("ExtSubset"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("TextDecl"), ToUtf32("TextDecl"), 1)),
            new gendoc::parsing::NonterminalParser(ToUtf32("ExtSubsetDecl"), ToUtf32("ExtSubsetDecl"), 1))));
    AddRule(new ExtSubsetDeclRule(ToUtf32("ExtSubsetDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("MarkupDecl"), ToUtf32("MarkupDecl"), 1),
                        new gendoc::parsing::NonterminalParser(ToUtf32("ConditionalSect"), ToUtf32("ConditionalSect"), 1)),
                    new gendoc::parsing::NonterminalParser(ToUtf32("DeclSep"), ToUtf32("DeclSep"), 1))))));
    AddRule(new SDDeclRule(ToUtf32("SDDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                            new gendoc::parsing::StringParser(ToUtf32("standalone"))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::CharParser('\''),
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                    new gendoc::parsing::StringParser(ToUtf32("yes"))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                    new gendoc::parsing::StringParser(ToUtf32("no")))))),
                                    new gendoc::parsing::CharParser('\''))),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::CharParser('\"'),
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                    new gendoc::parsing::StringParser(ToUtf32("yes"))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                    new gendoc::parsing::StringParser(ToUtf32("no")))))),
                                    new gendoc::parsing::CharParser('\"'))))))))));
    AddRule(new ElementRule(ToUtf32("Element"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('<'),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Attribute"), ToUtf32("Attribute"), 1))))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::StringParser(ToUtf32("/>"))),
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::CharParser('>')),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Content"), ToUtf32("Content"), 1)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("ETag"), ToUtf32("ETag"), 1)))))));
    AddRule(new AttributeRule(ToUtf32("Attribute"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("attName"), ToUtf32("Name"), 0),
                new gendoc::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("attValue"), ToUtf32("AttValue"), 1)))));
    AddRule(new ETagRule(ToUtf32("ETag"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::StringParser(ToUtf32("</")),
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::CharParser('>'))));
    AddRule(new ContentRule(ToUtf32("Content"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("cd1"), ToUtf32("CharData"), 1)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("Element"), ToUtf32("Element"), 1),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("Reference"), ToUtf32("Reference"), 1)),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("CDSect"), ToUtf32("CDSect"), 1)),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("PI"), ToUtf32("PI"), 1)),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 1))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("cd2"), ToUtf32("CharData"), 1))))))));
    AddRule(new ElementDeclRule(ToUtf32("ElementDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("<!ELEMENT")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new gendoc::parsing::NonterminalParser(ToUtf32("elementName"), ToUtf32("Name"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ContentSpec"), ToUtf32("ContentSpec"), 0)),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::CharParser('>'))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ContentSpec"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::StringParser(ToUtf32("EMPTY")),
                    new gendoc::parsing::StringParser(ToUtf32("ANY"))),
                new gendoc::parsing::NonterminalParser(ToUtf32("Mixed"), ToUtf32("Mixed"), 0)),
            new gendoc::parsing::NonterminalParser(ToUtf32("Children"), ToUtf32("Children"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Children"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Choice"), ToUtf32("Choice"), 0),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Seq"), ToUtf32("Seq"), 0))),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::CharParser('?'),
                            new gendoc::parsing::CharParser('*')),
                        new gendoc::parsing::CharParser('+')))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("CP"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Choice"), ToUtf32("Choice"), 0)),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Seq"), ToUtf32("Seq"), 0))),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::CharParser('?'),
                            new gendoc::parsing::CharParser('*')),
                        new gendoc::parsing::CharParser('+')))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Choice"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('('),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0)),
                    new gendoc::parsing::PositiveParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::OptionalParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new gendoc::parsing::CharParser('|')),
                                    new gendoc::parsing::OptionalParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                new gendoc::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0))))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::CharParser(')'))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Seq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('('),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::OptionalParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new gendoc::parsing::CharParser(',')),
                                    new gendoc::parsing::OptionalParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                new gendoc::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0))))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::CharParser(')'))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Mixed"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser('('),
                                new gendoc::parsing::OptionalParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new gendoc::parsing::StringParser(ToUtf32("#PCDATA"))),
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::OptionalParser(
                                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                            new gendoc::parsing::CharParser('|')),
                                        new gendoc::parsing::OptionalParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new gendoc::parsing::StringParser(ToUtf32(")*"))),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('('),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new gendoc::parsing::StringParser(ToUtf32("#PCDATA"))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new gendoc::parsing::CharParser(')')))));
    AddRule(new AttlistDeclRule(ToUtf32("AttlistDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::StringParser(ToUtf32("<!ATTLIST")),
                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("AttDef"), ToUtf32("AttDef"), 1))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::CharParser('>'))));
    AddRule(new AttDefRule(ToUtf32("AttDef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new gendoc::parsing::NonterminalParser(ToUtf32("AttType"), ToUtf32("AttType"), 0)),
                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
            new gendoc::parsing::NonterminalParser(ToUtf32("DefaultDecl"), ToUtf32("DefaultDecl"), 1))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("AttType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("StringType"), ToUtf32("StringType"), 0),
                new gendoc::parsing::NonterminalParser(ToUtf32("TokenizedType"), ToUtf32("TokenizedType"), 0)),
            new gendoc::parsing::NonterminalParser(ToUtf32("EnumeratedType"), ToUtf32("EnumeratedType"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("StringType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::StringParser(ToUtf32("CDATA"))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("TokenizedType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::StringParser(ToUtf32("ID")),
                                new gendoc::parsing::StringParser(ToUtf32("IDREF"))),
                            new gendoc::parsing::StringParser(ToUtf32("IDREFS"))),
                        new gendoc::parsing::StringParser(ToUtf32("ENTITY"))),
                    new gendoc::parsing::StringParser(ToUtf32("ENTITIES"))),
                new gendoc::parsing::StringParser(ToUtf32("NMTOKEN"))),
            new gendoc::parsing::StringParser(ToUtf32("NMTOKENS")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("EnumeratedType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("NotationType"), ToUtf32("NotationType"), 0),
            new gendoc::parsing::NonterminalParser(ToUtf32("Enumeration"), ToUtf32("Enumeration"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("NotationType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::StringParser(ToUtf32("NOTATION")),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                new gendoc::parsing::CharParser('(')),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("f"), ToUtf32("Name"), 0)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::OptionalParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new gendoc::parsing::CharParser('|')),
                                    new gendoc::parsing::OptionalParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                new gendoc::parsing::NonterminalParser(ToUtf32("n"), ToUtf32("Name"), 0))))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::CharParser(')'))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Enumeration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('('),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::OptionalParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new gendoc::parsing::CharParser('|')),
                                    new gendoc::parsing::OptionalParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0))))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::CharParser(')'))));
    AddRule(new DefaultDeclRule(ToUtf32("DefaultDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::StringParser(ToUtf32("#REQUIRED")),
                new gendoc::parsing::StringParser(ToUtf32("#IMPLIED"))),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("#FIXED")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)))),
                    new gendoc::parsing::NonterminalParser(ToUtf32("AttValue"), ToUtf32("AttValue"), 1))))));
    AddRule(new ConditionalSectRule(ToUtf32("ConditionalSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("IncludeSect"), ToUtf32("IncludeSect"), 1),
            new gendoc::parsing::NonterminalParser(ToUtf32("IgnoreSect"), ToUtf32("IgnoreSect"), 0))));
    AddRule(new IncludeSectRule(ToUtf32("IncludeSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("<!")),
                                new gendoc::parsing::OptionalParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new gendoc::parsing::StringParser(ToUtf32("INCLUDE"))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                    new gendoc::parsing::CharParser('[')),
                new gendoc::parsing::NonterminalParser(ToUtf32("ExtSubsetDecl"), ToUtf32("ExtSubsetDecl"), 1)),
            new gendoc::parsing::StringParser(ToUtf32("]]>")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("IgnoreSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("<!")),
                                new gendoc::parsing::OptionalParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new gendoc::parsing::StringParser(ToUtf32("IGNORE"))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                    new gendoc::parsing::CharParser('[')),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("IgnoreSectContents"), ToUtf32("IgnoreSectContents"), 0))),
            new gendoc::parsing::StringParser(ToUtf32("]]>")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("IgnoreSectContents"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("Ignore"), ToUtf32("Ignore"), 0),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("<![")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("IgnoreSectContents"), ToUtf32("IgnoreSectContents"), 0)),
                            new gendoc::parsing::StringParser(ToUtf32("]]>"))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Ignore"), ToUtf32("Ignore"), 0)))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Ignore"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::DifferenceParser(
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::StringParser(ToUtf32("<![")),
                                new gendoc::parsing::StringParser(ToUtf32("]]>"))))),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)))))));
    AddRule(new CharRefRule(ToUtf32("CharRef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::StringParser(ToUtf32("&#")),
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("dcr"), ToUtf32("uint"), 0))),
                new gendoc::parsing::CharParser(';')),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::StringParser(ToUtf32("&#x")),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("hcr"), ToUtf32("hexuint"), 0))),
                new gendoc::parsing::CharParser(';')))));
    AddRule(new ReferenceRule(ToUtf32("Reference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("EntityRef"), ToUtf32("EntityRef"), 1),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("CharRef"), ToUtf32("CharRef"), 0)))));
    AddRule(new EntityRefRule(ToUtf32("EntityRef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('&'),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))),
            new gendoc::parsing::CharParser(';'))));
    AddRule(new PEReferenceRule(ToUtf32("PEReference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('%'),
                new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
            new gendoc::parsing::CharParser(';'))));
    AddRule(new EntityDeclRule(ToUtf32("EntityDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("GEDecl"), ToUtf32("GEDecl"), 1),
            new gendoc::parsing::NonterminalParser(ToUtf32("PEDecl"), ToUtf32("PEDecl"), 1))));
    AddRule(new GEDeclRule(ToUtf32("GEDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::GroupingParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::StringParser(ToUtf32("<!ENTITY")),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                new gendoc::parsing::NonterminalParser(ToUtf32("entityName"), ToUtf32("Name"), 0)),
                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("entityValue"), ToUtf32("EntityDef"), 1)),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new gendoc::parsing::CharParser('>')))));
    AddRule(new PEDeclRule(ToUtf32("PEDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::GroupingParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::StringParser(ToUtf32("<!ENTITY")),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new gendoc::parsing::CharParser('%')),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                new gendoc::parsing::NonterminalParser(ToUtf32("peName"), ToUtf32("Name"), 0)),
                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("peValue"), ToUtf32("PEDef"), 1)),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new gendoc::parsing::CharParser('>')))));
    AddRule(new EntityDefRule(ToUtf32("EntityDef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("EntityValue"), ToUtf32("EntityValue"), 1),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("notation"), ToUtf32("NDataDecl"), 0)))))));
    AddRule(new PEDefRule(ToUtf32("PEDef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("EntityValue"), ToUtf32("EntityValue"), 1),
            new gendoc::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ExternalID"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::StringParser(ToUtf32("SYSTEM")),
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new gendoc::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("SystemLiteral"), 0))),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("PUBLIC")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new gendoc::parsing::NonterminalParser(ToUtf32("p2"), ToUtf32("PubidLiteral"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new gendoc::parsing::NonterminalParser(ToUtf32("s2"), ToUtf32("SystemLiteral"), 0))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("NDataDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                    new gendoc::parsing::StringParser(ToUtf32("NDATA"))),
                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
            new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))));
    AddRule(new TextDeclRule(ToUtf32("TextDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::StringParser(ToUtf32("<?xml")),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("VersionInfo"), ToUtf32("VersionInfo"), 1))),
                    new gendoc::parsing::NonterminalParser(ToUtf32("EncodingDecl"), ToUtf32("EncodingDecl"), 1)),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::StringParser(ToUtf32("?>")))));
    AddRule(new ExtParsedEntRule(ToUtf32("ExtParsedEnt"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("TextDecl"), ToUtf32("TextDecl"), 1)),
            new gendoc::parsing::NonterminalParser(ToUtf32("Content"), ToUtf32("Content"), 1))));
    AddRule(new EncodingDeclRule(ToUtf32("EncodingDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                    new gendoc::parsing::StringParser(ToUtf32("encoding"))),
                new gendoc::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('\"'),
                            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("en1"), ToUtf32("EncName"), 0))),
                        new gendoc::parsing::CharParser('\"')),
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('\''),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("en2"), ToUtf32("EncName"), 0))),
                        new gendoc::parsing::CharParser('\'')))))));
    AddRule(new EncNameRule(ToUtf32("EncName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharSetParser(ToUtf32("A-Za-z")),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::CharSetParser(ToUtf32("A-Za-z0-9._-"))))))));
    AddRule(new NotationDeclRule(ToUtf32("NotationDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("<!NOTATION")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0),
                            new gendoc::parsing::NonterminalParser(ToUtf32("PublicID"), ToUtf32("PublicID"), 0)))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new gendoc::parsing::CharParser('>'))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("PublicID"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::StringParser(ToUtf32("PUBLIC")),
                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
            new gendoc::parsing::NonterminalParser(ToUtf32("PubidLiteral"), ToUtf32("PubidLiteral"), 0))));
}

} } // namespace gendoc.xml
