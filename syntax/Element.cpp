#include "Element.hpp"
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
#include <gendoc/parsing/Rule.hpp>
#include <gendoc/codedom/Type.hpp>
#include <gendoc/code/Declarator.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Element* Element::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Element* Element::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Element* grammar(new Element(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Element::Element(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Element"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.syntax")), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back(ToUtf32("alphabetic"));
    keywords0.push_back(ToUtf32("anychar"));
    keywords0.push_back(ToUtf32("basechar"));
    keywords0.push_back(ToUtf32("cased_letter"));
    keywords0.push_back(ToUtf32("close_punctuation"));
    keywords0.push_back(ToUtf32("connector_punctuation"));
    keywords0.push_back(ToUtf32("control"));
    keywords0.push_back(ToUtf32("currency_symbol"));
    keywords0.push_back(ToUtf32("dash_punctuation"));
    keywords0.push_back(ToUtf32("decimal_number"));
    keywords0.push_back(ToUtf32("digit"));
    keywords0.push_back(ToUtf32("empty"));
    keywords0.push_back(ToUtf32("enclosing_mark"));
    keywords0.push_back(ToUtf32("end"));
    keywords0.push_back(ToUtf32("final_punctuation"));
    keywords0.push_back(ToUtf32("format"));
    keywords0.push_back(ToUtf32("grammar"));
    keywords0.push_back(ToUtf32("graphic"));
    keywords0.push_back(ToUtf32("hexdigit"));
    keywords0.push_back(ToUtf32("idcont"));
    keywords0.push_back(ToUtf32("idstart"));
    keywords0.push_back(ToUtf32("initial_punctuation"));
    keywords0.push_back(ToUtf32("keyword"));
    keywords0.push_back(ToUtf32("keyword_list"));
    keywords0.push_back(ToUtf32("letter"));
    keywords0.push_back(ToUtf32("letter_number"));
    keywords0.push_back(ToUtf32("line_separator"));
    keywords0.push_back(ToUtf32("lower_letter"));
    keywords0.push_back(ToUtf32("mark"));
    keywords0.push_back(ToUtf32("math_symbol"));
    keywords0.push_back(ToUtf32("modifier_letter"));
    keywords0.push_back(ToUtf32("modifier_symbol"));
    keywords0.push_back(ToUtf32("nonspacing_mark"));
    keywords0.push_back(ToUtf32("number"));
    keywords0.push_back(ToUtf32("open_punctuation"));
    keywords0.push_back(ToUtf32("other"));
    keywords0.push_back(ToUtf32("other_letter"));
    keywords0.push_back(ToUtf32("other_number"));
    keywords0.push_back(ToUtf32("other_punctuation"));
    keywords0.push_back(ToUtf32("other_symbol"));
    keywords0.push_back(ToUtf32("paragraph_separator"));
    keywords0.push_back(ToUtf32("private_use"));
    keywords0.push_back(ToUtf32("punctuation"));
    keywords0.push_back(ToUtf32("range"));
    keywords0.push_back(ToUtf32("separator"));
    keywords0.push_back(ToUtf32("skip"));
    keywords0.push_back(ToUtf32("space"));
    keywords0.push_back(ToUtf32("space_separator"));
    keywords0.push_back(ToUtf32("spacing_mark"));
    keywords0.push_back(ToUtf32("start"));
    keywords0.push_back(ToUtf32("surrogate"));
    keywords0.push_back(ToUtf32("symbol"));
    keywords0.push_back(ToUtf32("title_letter"));
    keywords0.push_back(ToUtf32("token"));
    keywords0.push_back(ToUtf32("unassigned"));
    keywords0.push_back(ToUtf32("upper_letter"));
    keywords0.push_back(ToUtf32("using"));
    keywords0.push_back(ToUtf32("value"));
    keywords0.push_back(ToUtf32("var"));
}

void Element::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::Grammar* grammar)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<gendoc::parsing::Grammar*>(grammar)));
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

class Element::RuleLinkRule : public gendoc::parsing::Rule
{
public:
    RuleLinkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RuleLinkRule>(this, &RuleLinkRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RuleLinkRule>(this, &RuleLinkRule::A1Action));
        gendoc::parsing::NonterminalParser* aliasNameNonterminalParser = GetNonterminal(ToUtf32("aliasName"));
        aliasNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::PostaliasName));
        gendoc::parsing::NonterminalParser* ruleNameNonterminalParser = GetNonterminal(ToUtf32("ruleName"));
        ruleNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::PostruleName));
        gendoc::parsing::NonterminalParser* qualified_idNonterminalParser = GetNonterminal(ToUtf32("qualified_id"));
        qualified_idNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::Postqualified_id));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        RuleLink * link(new RuleLink(context->fromaliasName, context->grammar, context->fromruleName));
        link->SetSpan(span);
        context->grammar->AddRuleLink(link);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        RuleLink * link(new RuleLink(context->grammar, context->fromqualified_id));
        link->SetSpan(span);
        context->grammar->AddRuleLink(link);
    }
    void PostaliasName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromaliasName_value = std::move(stack.top());
            context->fromaliasName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromaliasName_value.get());
            stack.pop();
        }
    }
    void PostruleName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromruleName_value = std::move(stack.top());
            context->fromruleName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromruleName_value.get());
            stack.pop();
        }
    }
    void Postqualified_id(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromqualified_id_value = std::move(stack.top());
            context->fromqualified_id = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromqualified_id_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): grammar(), fromaliasName(), fromruleName(), fromqualified_id() {}
        gendoc::parsing::Grammar* grammar;
        std::u32string fromaliasName;
        std::u32string fromruleName;
        std::u32string fromqualified_id;
    };
};

class Element::SignatureRule : public gendoc::parsing::Rule
{
public:
    SignatureRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        gendoc::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SignatureRule>(this, &SignatureRule::PreParameterList));
        gendoc::parsing::NonterminalParser* returnTypeNonterminalParser = GetNonterminal(ToUtf32("ReturnType"));
        returnTypeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SignatureRule>(this, &SignatureRule::PreReturnType));
    }
    void PreParameterList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Rule*>(context->rule)));
    }
    void PreReturnType(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Rule*>(context->rule)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): rule() {}
        gendoc::parsing::Rule* rule;
    };
};

class Element::ParameterListRule : public gendoc::parsing::Rule
{
public:
    ParameterListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        gendoc::parsing::NonterminalParser* variableNonterminalParser = GetNonterminal(ToUtf32("Variable"));
        variableNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ParameterListRule>(this, &ParameterListRule::PreVariable));
        gendoc::parsing::NonterminalParser* parameterNonterminalParser = GetNonterminal(ToUtf32("Parameter"));
        parameterNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ParameterListRule>(this, &ParameterListRule::PreParameter));
    }
    void PreVariable(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Rule*>(context->rule)));
    }
    void PreParameter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Rule*>(context->rule)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): rule() {}
        gendoc::parsing::Rule* rule;
    };
};

class Element::VariableRule : public gendoc::parsing::Rule
{
public:
    VariableRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<VariableRule>(this, &VariableRule::A0Action));
        gendoc::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<VariableRule>(this, &VariableRule::PostTypeId));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<VariableRule>(this, &VariableRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->AddLocalVariable(AttrOrVariable(context->fromTypeId->ToString(), context->fromDeclarator));
        delete context->fromTypeId;
    }
    void PostTypeId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): rule(), fromTypeId(), fromDeclarator() {}
        gendoc::parsing::Rule* rule;
        gendoc::codedom::TypeId* fromTypeId;
        std::u32string fromDeclarator;
    };
};

class Element::ParameterRule : public gendoc::parsing::Rule
{
public:
    ParameterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ParameterRule>(this, &ParameterRule::A0Action));
        gendoc::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ParameterRule>(this, &ParameterRule::PostTypeId));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ParameterRule>(this, &ParameterRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->AddInheritedAttribute(AttrOrVariable(context->fromTypeId->ToString(), context->fromDeclarator));
        delete context->fromTypeId;
    }
    void PostTypeId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): rule(), fromTypeId(), fromDeclarator() {}
        gendoc::parsing::Rule* rule;
        gendoc::codedom::TypeId* fromTypeId;
        std::u32string fromDeclarator;
    };
};

class Element::ReturnTypeRule : public gendoc::parsing::Rule
{
public:
    ReturnTypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ReturnTypeRule>(this, &ReturnTypeRule::A0Action));
        gendoc::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ReturnTypeRule>(this, &ReturnTypeRule::PostTypeId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->SetValueTypeName(context->fromTypeId->ToString());
        delete context->fromTypeId;
    }
    void PostTypeId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): rule(), fromTypeId() {}
        gendoc::parsing::Rule* rule;
        gendoc::codedom::TypeId* fromTypeId;
    };
};

class Element::IdentifierRule : public gendoc::parsing::Rule
{
public:
    IdentifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdentifierRule>(this, &IdentifierRule::A0Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IdentifierRule>(this, &IdentifierRule::Postidentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromidentifier() {}
        std::u32string value;
        std::u32string fromidentifier;
    };
};

class Element::QualifiedIdRule : public gendoc::parsing::Rule
{
public:
    QualifiedIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A0Action));
        gendoc::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postfirst));
        gendoc::parsing::NonterminalParser* restNonterminalParser = GetNonterminal(ToUtf32("rest"));
        restNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postrest));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void Postfirst(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postrest(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromrest_value = std::move(stack.top());
            context->fromrest = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromrest_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromfirst(), fromrest() {}
        std::u32string value;
        std::u32string fromfirst;
        std::u32string fromrest;
    };
};

class Element::StringArrayRule : public gendoc::parsing::Rule
{
public:
    StringArrayRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("std::vector<std::u32string>*"), ToUtf32("array")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> array_value = std::move(stack.top());
        context->array = *static_cast<gendoc::parsing::ValueObject<std::vector<std::u32string>*>*>(array_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StringArrayRule>(this, &StringArrayRule::A0Action));
        gendoc::parsing::NonterminalParser* strNonterminalParser = GetNonterminal(ToUtf32("str"));
        strNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StringArrayRule>(this, &StringArrayRule::Poststr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->array->push_back(context->fromstr);
    }
    void Poststr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromstr_value = std::move(stack.top());
            context->fromstr = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromstr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): array(), fromstr() {}
        std::vector<std::u32string>* array;
        std::u32string fromstr;
    };
};

void Element::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.code.Declarator"));
    if (!grammar1)
    {
        grammar1 = gendoc::code::Declarator::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void Element::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("gendoc.parsing.stdlib.qualified_id")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("gendoc.parsing.stdlib.identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("gendoc.parsing.stdlib.string")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeId"), this, ToUtf32("gendoc.code.Declarator.TypeId")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Declarator"), this, ToUtf32("gendoc.code.Declarator.Declarator")));
    AddRule(new RuleLinkRule(ToUtf32("RuleLink"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("using")),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("aliasName"), ToUtf32("identifier"), 0)),
                                new gendoc::parsing::CharParser('=')),
                            new gendoc::parsing::NonterminalParser(ToUtf32("ruleName"), ToUtf32("qualified_id"), 0)),
                        new gendoc::parsing::CharParser(';')))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("using")),
                            new gendoc::parsing::NonterminalParser(ToUtf32("qualified_id"), ToUtf32("qualified_id"), 0)),
                        new gendoc::parsing::CharParser(';')))))));
    AddRule(new SignatureRule(ToUtf32("Signature"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 1)),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("ReturnType"), ToUtf32("ReturnType"), 1)))));
    AddRule(new ParameterListRule(ToUtf32("ParameterList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('('),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::ListParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("Variable"), ToUtf32("Variable"), 1),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Parameter"), ToUtf32("Parameter"), 1))),
                        new gendoc::parsing::CharParser(',')))),
            new gendoc::parsing::ExpectationParser(
                new gendoc::parsing::CharParser(')')))));
    AddRule(new VariableRule(ToUtf32("Variable"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::KeywordParser(ToUtf32("var")),
                new gendoc::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0)),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)))));
    AddRule(new ParameterRule(ToUtf32("Parameter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)))));
    AddRule(new ReturnTypeRule(ToUtf32("ReturnType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::DifferenceParser(
                    new gendoc::parsing::CharParser(':'),
                    new gendoc::parsing::StringParser(ToUtf32("::")))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Keyword"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KeywordListParser(ToUtf32("identifier"), keywords0)));
    AddRule(new IdentifierRule(ToUtf32("Identifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::DifferenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("identifier"), ToUtf32("identifier"), 0),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Keyword"), ToUtf32("Keyword"), 0))))));
    AddRule(new QualifiedIdRule(ToUtf32("QualifiedId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("Identifier"), 0),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser('.'),
                                new gendoc::parsing::NonterminalParser(ToUtf32("rest"), ToUtf32("Identifier"), 0)))))))));
    AddRule(new StringArrayRule(ToUtf32("StringArray"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('['),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::ListParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("str"), ToUtf32("string"), 0)),
                        new gendoc::parsing::CharParser(',')))),
            new gendoc::parsing::ExpectationParser(
                new gendoc::parsing::CharParser(']')))));
}

} } // namespace gendoc.syntax
