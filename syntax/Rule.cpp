#include "Rule.hpp"
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
#include <gendoc/syntax/Composite.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Rule* Rule::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Rule* Rule::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Rule* grammar(new Rule(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Rule::Rule(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Rule"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.syntax")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::parsing::Rule* Rule::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Scope* enclosingScope)
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
    gendoc::parsing::Rule* result = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Rule*>*>(value.get());
    stack.pop();
    return result;
}

class Rule::RuleRule : public gendoc::parsing::Rule
{
public:
    RuleRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("gendoc::parsing::Rule*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Rule*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RuleRule>(this, &RuleRule::A0Action));
        gendoc::parsing::NonterminalParser* ruleHeaderNonterminalParser = GetNonterminal(ToUtf32("RuleHeader"));
        ruleHeaderNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreRuleHeader));
        ruleHeaderNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleRule>(this, &RuleRule::PostRuleHeader));
        gendoc::parsing::NonterminalParser* ruleBodyNonterminalParser = GetNonterminal(ToUtf32("RuleBody"));
        ruleBodyNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreRuleBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRuleHeader;
    }
    void PreRuleHeader(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->enclosingScope)));
    }
    void PostRuleHeader(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromRuleHeader_value = std::move(stack.top());
            context->fromRuleHeader = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Rule*>*>(fromRuleHeader_value.get());
            stack.pop();
        }
    }
    void PreRuleBody(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Rule*>(context->value)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromRuleHeader() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Rule* value;
        gendoc::parsing::Rule* fromRuleHeader;
    };
};

class Rule::RuleHeaderRule : public gendoc::parsing::Rule
{
public:
    RuleHeaderRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("gendoc::parsing::Rule*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Rule*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RuleHeaderRule>(this, &RuleHeaderRule::A0Action));
        gendoc::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("name"));
        nameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleHeaderRule>(this, &RuleHeaderRule::Postname));
        gendoc::parsing::NonterminalParser* signatureNonterminalParser = GetNonterminal(ToUtf32("Signature"));
        signatureNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<RuleHeaderRule>(this, &RuleHeaderRule::PreSignature));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Rule(context->fromname, context->enclosingScope);
        context->value->SetSpan(span);
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
    void PreSignature(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Rule*>(context->value)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromname() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Rule* value;
        std::u32string fromname;
    };
};

class Rule::RuleBodyRule : public gendoc::parsing::Rule
{
public:
    RuleBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Rule*"), ToUtf32("rule")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> rule_value = std::move(stack.top());
        context->rule = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Rule*>*>(rule_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RuleBodyRule>(this, &RuleBodyRule::A0Action));
        gendoc::parsing::NonterminalParser* definitionNonterminalParser = GetNonterminal(ToUtf32("definition"));
        definitionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<RuleBodyRule>(this, &RuleBodyRule::Predefinition));
        definitionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleBodyRule>(this, &RuleBodyRule::Postdefinition));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->SetDefinition(context->fromdefinition);
    }
    void Predefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->rule->GetScope())));
    }
    void Postdefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromdefinition_value = std::move(stack.top());
            context->fromdefinition = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromdefinition_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): rule(), fromdefinition() {}
        gendoc::parsing::Rule* rule;
        gendoc::parsing::Parser* fromdefinition;
    };
};

void Rule::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.syntax.Element"));
    if (!grammar0)
    {
        grammar0 = gendoc::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.syntax.Composite"));
    if (!grammar1)
    {
        grammar1 = gendoc::syntax::Composite::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar2)
    {
        grammar2 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void Rule::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Element.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Signature"), this, ToUtf32("Element.Signature")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Alternative"), this, ToUtf32("Composite.Alternative")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("gendoc.parsing.stdlib.string")));
    AddRule(new RuleRule(ToUtf32("Rule"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("RuleHeader"), ToUtf32("RuleHeader"), 1)),
                new gendoc::parsing::StringParser(ToUtf32("::="))),
            new gendoc::parsing::NonterminalParser(ToUtf32("RuleBody"), ToUtf32("RuleBody"), 1))));
    AddRule(new RuleHeaderRule(ToUtf32("RuleHeader"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("name"), ToUtf32("Identifier"), 0)),
            new gendoc::parsing::NonterminalParser(ToUtf32("Signature"), ToUtf32("Signature"), 1))));
    AddRule(new RuleBodyRule(ToUtf32("RuleBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("definition"), ToUtf32("Alternative"), 1)),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser(';')))))));
}

} } // namespace gendoc.syntax
