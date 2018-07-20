#include "ParserTokenGrammar.hpp"
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
#include <gendoc/syntax/Element.hpp>
#include <gendoc/syntax/Primitive.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

ParserToken* ParserToken::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

ParserToken* ParserToken::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ParserToken* grammar(new ParserToken(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ParserToken::ParserToken(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("ParserToken"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

void ParserToken::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, SourceCodeWriter* writer)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<SourceCodeWriter*>(writer)));
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

class ParserToken::SourceLineRule : public gendoc::parsing::Rule
{
public:
    SourceLineRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("SourceCodeWriter*"), ToUtf32("writer")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> writer_value = std::move(stack.top());
        context->writer = *static_cast<gendoc::parsing::ValueObject<SourceCodeWriter*>*>(writer_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* sourceTokenNonterminalParser = GetNonterminal(ToUtf32("SourceToken"));
        sourceTokenNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SourceLineRule>(this, &SourceLineRule::PreSourceToken));
    }
    void PreSourceToken(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<SourceCodeWriter*>(context->writer)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): writer() {}
        SourceCodeWriter* writer;
    };
};

class ParserToken::SourceTokenRule : public gendoc::parsing::Rule
{
public:
    SourceTokenRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("SourceCodeWriter*"), ToUtf32("writer")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> writer_value = std::move(stack.top());
        context->writer = *static_cast<gendoc::parsing::ValueObject<SourceCodeWriter*>*>(writer_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A9Action));
        gendoc::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A10Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::Postidentifier));
        gendoc::parsing::NonterminalParser* charSetNonterminalParser = GetNonterminal(ToUtf32("CharSet"));
        gendoc::parsing::NonterminalParser* stringNonterminalParser = GetNonterminal(ToUtf32("string"));
        stringNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::Poststring));
        gendoc::parsing::NonterminalParser* charNonterminalParser = GetNonterminal(ToUtf32("char"));
        charNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::Postchar));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteComment(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteComment(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WritePP(std::u32string(matchBegin, matchEnd));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteKeyword(std::u32string(matchBegin, matchEnd));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteKeyword(std::u32string(matchBegin, matchEnd));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteIdentifier(std::u32string(matchBegin, matchEnd));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteString(std::u32string(matchBegin, matchEnd));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteString(std::u32string(matchBegin, matchEnd));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteString(std::u32string(matchBegin, matchEnd));
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteSpace(matchEnd - matchBegin);
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->writer->WriteOther(std::u32string(matchBegin, matchEnd));
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
    void Poststring(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromstring_value = std::move(stack.top());
            context->fromstring = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromstring_value.get());
            stack.pop();
        }
    }
    void Postchar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromchar_value = std::move(stack.top());
            context->fromchar = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromchar_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): writer(), fromidentifier(), fromstring(), fromchar() {}
        SourceCodeWriter* writer;
        std::u32string fromidentifier;
        std::u32string fromstring;
        char32_t fromchar;
    };
};

class ParserToken::CharSetRule : public gendoc::parsing::Rule
{
public:
    CharSetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("inverse")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("s")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A1Action));
        gendoc::parsing::NonterminalParser* charSetRangeNonterminalParser = GetNonterminal(ToUtf32("CharSetRange"));
        charSetRangeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharSetRule>(this, &CharSetRule::PostCharSetRange));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->inverse = true;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(context->fromCharSetRange);
    }
    void PostCharSetRange(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCharSetRange_value = std::move(stack.top());
            context->fromCharSetRange = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromCharSetRange_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): inverse(), s(), fromCharSetRange() {}
        bool inverse;
        std::u32string s;
        std::u32string fromCharSetRange;
    };
};

class ParserToken::CharSetRangeRule : public gendoc::parsing::Rule
{
public:
    CharSetRangeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A1Action));
        gendoc::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postfirst));
        gendoc::parsing::NonterminalParser* lastNonterminalParser = GetNonterminal(ToUtf32("last"));
        lastNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postlast));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromfirst);
        context->value.append(1, '-');
        context->value.append(1, context->fromlast);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromfirst);
    }
    void Postfirst(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postlast(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromlast_value = std::move(stack.top());
            context->fromlast = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromlast_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromfirst(), fromlast() {}
        std::u32string value;
        char32_t fromfirst;
        char32_t fromlast;
    };
};

class ParserToken::CharSetCharRule : public gendoc::parsing::Rule
{
public:
    CharSetCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A1Action));
        gendoc::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharSetCharRule>(this, &CharSetCharRule::Postescape));
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromescape() {}
        char32_t value;
        char32_t fromescape;
    };
};

void ParserToken::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Keyword"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::Keyword::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.syntax.Element"));
    if (!grammar2)
    {
        grammar2 = gendoc::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void ParserToken::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("char"), this, ToUtf32("gendoc.parsing.stdlib.char")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("gendoc.parsing.stdlib.identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("CppKeyword"), this, ToUtf32("Keyword.Keyword")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ParserKeyword"), this, ToUtf32("gendoc.syntax.Element.Keyword")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("gendoc.parsing.stdlib.string")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("escape"), this, ToUtf32("gendoc.parsing.stdlib.escape")));
    AddRule(new SourceLineRule(ToUtf32("SourceLine"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("SourceToken"), ToUtf32("SourceToken"), 1))));
    AddRule(new SourceTokenRule(ToUtf32("SourceToken"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("LineComment"), ToUtf32("LineComment"), 0)),
                                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("BlockComment"), ToUtf32("BlockComment"), 0))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("PPLine"), ToUtf32("PPLine"), 0))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("CppKeyword"), ToUtf32("CppKeyword"), 0))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("ParserKeyword"), ToUtf32("ParserKeyword"), 0))),
                                new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("identifier"), ToUtf32("identifier"), 0))),
                            new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("CharSet"), ToUtf32("CharSet"), 0))),
                        new gendoc::parsing::ActionParser(ToUtf32("A7"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("string"), ToUtf32("string"), 0))),
                    new gendoc::parsing::ActionParser(ToUtf32("A8"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("char"), ToUtf32("char"), 0))),
                new gendoc::parsing::ActionParser(ToUtf32("A9"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::PositiveParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Space"), ToUtf32("Space"), 0))))),
            new gendoc::parsing::ActionParser(ToUtf32("A10"),
                new gendoc::parsing::AnyCharParser()))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("LineComment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::StringParser(ToUtf32("//")),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::CharSetParser(ToUtf32("\r\n"), true)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("BlockComment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::StringParser(ToUtf32("/*")),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::DifferenceParser(
                            new gendoc::parsing::AnyCharParser(),
                            new gendoc::parsing::StringParser(ToUtf32("*/")))))),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::StringParser(ToUtf32("*/"))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("PPLine"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Space"), ToUtf32("Space"), 0)),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::StringParser(ToUtf32("[cpp]")),
                                    new gendoc::parsing::StringParser(ToUtf32("[hpp]")))))),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Space"), ToUtf32("Space"), 0))),
                new gendoc::parsing::CharParser('#')),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::CharSetParser(ToUtf32("\r\n"), true)))));
    AddRule(new CharSetRule(ToUtf32("CharSet"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('['),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                    new gendoc::parsing::CharParser('^'))))),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("CharSetRange"), ToUtf32("CharSetRange"), 0))))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::CharParser(']'))))));
    AddRule(new CharSetRangeRule(ToUtf32("CharSetRange"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("CharSetChar"), 0),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('-'),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("last"), ToUtf32("CharSetChar"), 0))),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::EmptyParser()))))));
    AddRule(new CharSetCharRule(ToUtf32("CharSetChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::CharSetParser(ToUtf32("\\]"), true)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Space"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::CharSetParser(ToUtf32("\t "))));
}

} } // namespace gendoc.cppparser
