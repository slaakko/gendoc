#include "SimpleType.hpp"
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

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

SimpleType* SimpleType::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

SimpleType* SimpleType::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    SimpleType* grammar(new SimpleType(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

SimpleType::SimpleType(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("SimpleType"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

SimpleTypeNode* SimpleType::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    SimpleTypeNode* result = *static_cast<gendoc::parsing::ValueObject<SimpleTypeNode*>*>(value.get());
    stack.pop();
    return result;
}

class SimpleType::SimpleTypeRule : public gendoc::parsing::Rule
{
public:
    SimpleTypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("SimpleTypeNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::vector<SimpleTypeSpecifier>"), ToUtf32("specifiers")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<SimpleTypeNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeRule>(this, &SimpleTypeRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeRule>(this, &SimpleTypeRule::A1Action));
        gendoc::parsing::NonterminalParser* sNonterminalParser = GetNonterminal(ToUtf32("s"));
        sNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SimpleTypeRule>(this, &SimpleTypeRule::Posts));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SimpleTypeNode(span, context->specifiers);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->specifiers.push_back(context->froms);
    }
    void Posts(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms_value = std::move(stack.top());
            context->froms = *static_cast<gendoc::parsing::ValueObject<SimpleTypeSpecifier>*>(froms_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), specifiers(), froms() {}
        SimpleTypeNode* value;
        std::vector<SimpleTypeSpecifier> specifiers;
        SimpleTypeSpecifier froms;
    };
};

class SimpleType::SimpleTypeSpecifierRule : public gendoc::parsing::Rule
{
public:
    SimpleTypeSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("SimpleTypeSpecifier"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<SimpleTypeSpecifier>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A9Action));
        gendoc::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A10Action));
        gendoc::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A11Action));
        gendoc::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A12Action));
        gendoc::parsing::ActionParser* a13ActionParser = GetAction(ToUtf32("A13"));
        a13ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A13Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::char_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::char16;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::char32;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::wchar;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::bool_;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::short_;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::int_;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::long_;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::signed_;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::unsigned_;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::float_;
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::double_;
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::void_;
    }
    void A13Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = SimpleTypeSpecifier::auto_;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        SimpleTypeSpecifier value;
    };
};

void SimpleType::GetReferencedGrammars()
{
}

void SimpleType::CreateRules()
{
    AddRule(new SimpleTypeRule(ToUtf32("SimpleType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::PositiveParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("s"), ToUtf32("SimpleTypeSpecifier"), 0))))))));
    AddRule(new SimpleTypeSpecifierRule(ToUtf32("SimpleTypeSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                                                new gendoc::parsing::KeywordParser(ToUtf32("char"))),
                                                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                                new gendoc::parsing::KeywordParser(ToUtf32("char16_t")))),
                                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                            new gendoc::parsing::KeywordParser(ToUtf32("char32_t")))),
                                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                        new gendoc::parsing::KeywordParser(ToUtf32("wchar_t")))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                    new gendoc::parsing::KeywordParser(ToUtf32("bool")))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                                new gendoc::parsing::KeywordParser(ToUtf32("short")))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                            new gendoc::parsing::KeywordParser(ToUtf32("int")))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A7"),
                                        new gendoc::parsing::KeywordParser(ToUtf32("long")))),
                                new gendoc::parsing::ActionParser(ToUtf32("A8"),
                                    new gendoc::parsing::KeywordParser(ToUtf32("signed")))),
                            new gendoc::parsing::ActionParser(ToUtf32("A9"),
                                new gendoc::parsing::KeywordParser(ToUtf32("unsigned")))),
                        new gendoc::parsing::ActionParser(ToUtf32("A10"),
                            new gendoc::parsing::KeywordParser(ToUtf32("float")))),
                    new gendoc::parsing::ActionParser(ToUtf32("A11"),
                        new gendoc::parsing::KeywordParser(ToUtf32("double")))),
                new gendoc::parsing::ActionParser(ToUtf32("A12"),
                    new gendoc::parsing::KeywordParser(ToUtf32("void")))),
            new gendoc::parsing::ActionParser(ToUtf32("A13"),
                new gendoc::parsing::KeywordParser(ToUtf32("auto"))))));
}

} } // namespace gendoc.cppparser
