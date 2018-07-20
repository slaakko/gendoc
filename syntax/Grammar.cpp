#include "Grammar.hpp"
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
#include <gendoc/syntax/Element.hpp>
#include <gendoc/syntax/Rule.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

GrammarGrammar* GrammarGrammar::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

GrammarGrammar* GrammarGrammar::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    GrammarGrammar* grammar(new GrammarGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

GrammarGrammar::GrammarGrammar(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("GrammarGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.syntax")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::parsing::Grammar* GrammarGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Scope* enclosingScope, ParsingDomain* parsingDomain)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<ParsingDomain*>(parsingDomain)));
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
    gendoc::parsing::Grammar* result = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>*>(value.get());
    stack.pop();
    return result;
}

class GrammarGrammar::GrammarRule : public gendoc::parsing::Rule
{
public:
    GrammarRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Scope*"), ToUtf32("enclosingScope")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingDomain*"), ToUtf32("parsingDomain")));
        SetValueTypeName(ToUtf32("gendoc::parsing::Grammar*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> parsingDomain_value = std::move(stack.top());
        context->parsingDomain = *static_cast<gendoc::parsing::ValueObject<ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<GrammarRule>(this, &GrammarRule::A0Action));
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GrammarRule>(this, &GrammarRule::Postname));
        gendoc::parsing::NonterminalParser* grammarContentNonterminalParser = GetNonterminal(ToUtf32("GrammarContent"));
        grammarContentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GrammarRule>(this, &GrammarRule::PreGrammarContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Grammar(context->fromname, context->enclosingScope, context->parsingDomain);
        context->value->SetSpan(span);
        context->value->SetNs(context->enclosingScope->Ns());
    }
    void Postname(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromname_value = std::move(stack.top());
            context->fromname = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromname_value.get());
            stack.pop();
        }
    }
    void PreGrammarContent(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>(context->value)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), parsingDomain(), value(), fromname() {}
        gendoc::parsing::Scope* enclosingScope;
        ParsingDomain* parsingDomain;
        gendoc::parsing::Grammar* value;
        std::u32string fromname;
    };
};

class GrammarGrammar::GrammarContentRule : public gendoc::parsing::Rule
{
public:
    GrammarContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Grammar*"), ToUtf32("grammar")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> grammar_value = std::move(stack.top());
        context->grammar = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<GrammarContentRule>(this, &GrammarContentRule::A0Action));
        gendoc::parsing::NonterminalParser* startClauseNonterminalParser = GetNonterminal(ToUtf32("StartClause"));
        startClauseNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreStartClause));
        gendoc::parsing::NonterminalParser* skipClauseNonterminalParser = GetNonterminal(ToUtf32("SkipClause"));
        skipClauseNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreSkipClause));
        gendoc::parsing::NonterminalParser* ruleLinkNonterminalParser = GetNonterminal(ToUtf32("RuleLink"));
        ruleLinkNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRuleLink));
        gendoc::parsing::NonterminalParser* ruleNonterminalParser = GetNonterminal(ToUtf32("Rule"));
        ruleNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRule));
        ruleNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GrammarContentRule>(this, &GrammarContentRule::PostRule));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->AddRule(context->fromRule);
    }
    void PreStartClause(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>(context->grammar)));
    }
    void PreSkipClause(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>(context->grammar)));
    }
    void PreRuleLink(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>(context->grammar)));
    }
    void PreRule(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->grammar->GetScope())));
    }
    void PostRule(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromRule_value = std::move(stack.top());
            context->fromRule = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Rule*>*>(fromRule_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): grammar(), fromRule() {}
        gendoc::parsing::Grammar* grammar;
        gendoc::parsing::Rule* fromRule;
    };
};

class GrammarGrammar::StartClauseRule : public gendoc::parsing::Rule
{
public:
    StartClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Grammar*"), ToUtf32("grammar")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> grammar_value = std::move(stack.top());
        context->grammar = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StartClauseRule>(this, &StartClauseRule::A0Action));
        gendoc::parsing::NonterminalParser* strtNonterminalParser = GetNonterminal(ToUtf32("strt"));
        strtNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StartClauseRule>(this, &StartClauseRule::Poststrt));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->SetStartRuleName(context->fromstrt);
    }
    void Poststrt(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromstrt_value = std::move(stack.top());
            context->fromstrt = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromstrt_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): grammar(), fromstrt() {}
        gendoc::parsing::Grammar* grammar;
        std::u32string fromstrt;
    };
};

class GrammarGrammar::SkipClauseRule : public gendoc::parsing::Rule
{
public:
    SkipClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Grammar*"), ToUtf32("grammar")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> grammar_value = std::move(stack.top());
        context->grammar = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SkipClauseRule>(this, &SkipClauseRule::A0Action));
        gendoc::parsing::NonterminalParser* skpNonterminalParser = GetNonterminal(ToUtf32("skp"));
        skpNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SkipClauseRule>(this, &SkipClauseRule::Postskp));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->SetSkipRuleName(context->fromskp);
    }
    void Postskp(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromskp_value = std::move(stack.top());
            context->fromskp = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromskp_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): grammar(), fromskp() {}
        gendoc::parsing::Grammar* grammar;
        std::u32string fromskp;
    };
};

void GrammarGrammar::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.syntax.Element"));
    if (!grammar0)
    {
        grammar0 = gendoc::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.syntax.Rule"));
    if (!grammar1)
    {
        grammar1 = gendoc::syntax::Rule::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar2)
    {
        grammar2 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void GrammarGrammar::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Element.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Element.QualifiedId")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("RuleLink"), this, ToUtf32("Element.RuleLink")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Signature"), this, ToUtf32("Element.Signature")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Rule"), this, ToUtf32("Rule.Rule")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("char"), this, ToUtf32("gendoc.parsing.stdlib.char")));
    AddRule(new GrammarRule(ToUtf32("Grammar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("grammar")),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("name"), ToUtf32("Identifier"), 0)))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser('{'))),
                new gendoc::parsing::NonterminalParser(ToUtf32("GrammarContent"), ToUtf32("GrammarContent"), 1)),
            new gendoc::parsing::ExpectationParser(
                new gendoc::parsing::CharParser('}')))));
    AddRule(new GrammarContentRule(ToUtf32("GrammarContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("StartClause"), ToUtf32("StartClause"), 1),
                            new gendoc::parsing::NonterminalParser(ToUtf32("SkipClause"), ToUtf32("SkipClause"), 1)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("RuleLink"), ToUtf32("RuleLink"), 1)),
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Rule"), ToUtf32("Rule"), 1)))))));
    AddRule(new StartClauseRule(ToUtf32("StartClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::KeywordParser(ToUtf32("start")),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("strt"), ToUtf32("Identifier"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::CharParser(';')))));
    AddRule(new SkipClauseRule(ToUtf32("SkipClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::KeywordParser(ToUtf32("skip")),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("skp"), ToUtf32("QualifiedId"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::CharParser(';')))));
}

} } // namespace gendoc.syntax
