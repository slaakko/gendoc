#include "SourceLines.hpp"
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

SourceLines* SourceLines::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

SourceLines* SourceLines::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    SourceLines* grammar(new SourceLines(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

SourceLines::SourceLines(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("SourceLines"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

int SourceLines::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    int result = *static_cast<gendoc::parsing::ValueObject<int>*>(value.get());
    stack.pop();
    return result;
}

class SourceLines::SourceFileRule : public gendoc::parsing::Rule
{
public:
    SourceFileRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("int"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<int>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceFileRule>(this, &SourceFileRule::A0Action));
        gendoc::parsing::NonterminalParser* sourceLineNonterminalParser = GetNonterminal(ToUtf32("SourceLine"));
        sourceLineNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SourceFileRule>(this, &SourceFileRule::PostSourceLine));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value + context->fromSourceLine;
    }
    void PostSourceLine(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSourceLine_value = std::move(stack.top());
            context->fromSourceLine = *static_cast<gendoc::parsing::ValueObject<int>*>(fromSourceLine_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromSourceLine() {}
        int value;
        int fromSourceLine;
    };
};

class SourceLines::SourceLineRule : public gendoc::parsing::Rule
{
public:
    SourceLineRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("int"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<int>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceLineRule>(this, &SourceLineRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = 1;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        int value;
    };
};

void SourceLines::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void SourceLines::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("comment"), this, ToUtf32("gendoc.parsing.stdlib.comment")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("newline"), this, ToUtf32("gendoc.parsing.stdlib.newline")));
    AddRule(new SourceFileRule(ToUtf32("SourceFile"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("SourceLine"), ToUtf32("SourceLine"), 0))))));
    AddRule(new SourceLineRule(ToUtf32("SourceLine"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("PPLine"), ToUtf32("PPLine"), 0),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0)))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::PositiveParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::DifferenceParser(
                                    new gendoc::parsing::AnyCharParser(),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0)))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("PPLine"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new gendoc::parsing::CharParser('#')),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::DifferenceParser(
                                new gendoc::parsing::AnyCharParser(),
                                new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))))),
                new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("S"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::CharParser(' '),
                    new gendoc::parsing::CharParser('\t'))))));
    SetSkipRuleName(ToUtf32("comment"));
}

} } // namespace gendoc.cppparser
