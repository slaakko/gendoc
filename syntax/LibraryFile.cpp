#include "LibraryFile.hpp"
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
#include <gendoc/parsing/Namespace.hpp>
#include <gendoc/syntax/Element.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

LibraryFile* LibraryFile::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

LibraryFile* LibraryFile::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    LibraryFile* grammar(new LibraryFile(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

LibraryFile::LibraryFile(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("LibraryFile"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.syntax")), parsingDomain_)
{
    SetOwner(0);
}

void LibraryFile::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, gendoc::parsing::ParsingDomain* parsingDomain)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<gendoc::parsing::ParsingDomain*>(parsingDomain)));
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

class LibraryFile::LibraryFileRule : public gendoc::parsing::Rule
{
public:
    LibraryFileRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::ParsingDomain*"), ToUtf32("parsingDomain")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> parsingDomain_value = std::move(stack.top());
        context->parsingDomain = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LibraryFileRule>(this, &LibraryFileRule::PreNamespaceContent));
    }
    void PreNamespaceContent(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::ParsingDomain*>(context->parsingDomain)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): parsingDomain() {}
        gendoc::parsing::ParsingDomain* parsingDomain;
    };
};

class LibraryFile::NamespaceContentRule : public gendoc::parsing::Rule
{
public:
    NamespaceContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::ParsingDomain*"), ToUtf32("parsingDomain")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> parsingDomain_value = std::move(stack.top());
        context->parsingDomain = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A0Action));
        gendoc::parsing::NonterminalParser* grammarNonterminalParser = GetNonterminal(ToUtf32("Grammar"));
        grammarNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreGrammar));
        grammarNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostGrammar));
        gendoc::parsing::NonterminalParser* namespaceNonterminalParser = GetNonterminal(ToUtf32("Namespace"));
        namespaceNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreNamespace));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parsingDomain->AddGrammar(context->fromGrammar);
    }
    void PreGrammar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->parsingDomain->CurrentNamespace()->GetScope())));
    }
    void PostGrammar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromGrammar_value = std::move(stack.top());
            context->fromGrammar = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>*>(fromGrammar_value.get());
            stack.pop();
        }
    }
    void PreNamespace(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::ParsingDomain*>(context->parsingDomain)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): parsingDomain(), fromGrammar() {}
        gendoc::parsing::ParsingDomain* parsingDomain;
        gendoc::parsing::Grammar* fromGrammar;
    };
};

class LibraryFile::NamespaceRule : public gendoc::parsing::Rule
{
public:
    NamespaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::ParsingDomain*"), ToUtf32("parsingDomain")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> parsingDomain_value = std::move(stack.top());
        context->parsingDomain = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A1Action));
        gendoc::parsing::NonterminalParser* nsNonterminalParser = GetNonterminal(ToUtf32("ns"));
        nsNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceRule>(this, &NamespaceRule::Postns));
        gendoc::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NamespaceRule>(this, &NamespaceRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parsingDomain->BeginNamespace(context->fromns);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parsingDomain->EndNamespace();
    }
    void Postns(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromns_value = std::move(stack.top());
            context->fromns = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromns_value.get());
            stack.pop();
        }
    }
    void PreNamespaceContent(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::ParsingDomain*>(context->parsingDomain)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): parsingDomain(), fromns() {}
        gendoc::parsing::ParsingDomain* parsingDomain;
        std::u32string fromns;
    };
};

class LibraryFile::GrammarRule : public gendoc::parsing::Rule
{
public:
    GrammarRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("gendoc::parsing::Grammar*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<GrammarRule>(this, &GrammarRule::A0Action));
        gendoc::parsing::NonterminalParser* grammarNameNonterminalParser = GetNonterminal(ToUtf32("grammarName"));
        grammarNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GrammarRule>(this, &GrammarRule::PostgrammarName));
        gendoc::parsing::NonterminalParser* grammarContentNonterminalParser = GetNonterminal(ToUtf32("GrammarContent"));
        grammarContentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GrammarRule>(this, &GrammarRule::PreGrammarContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Grammar(context->fromgrammarName, context->enclosingScope);
        context->value->SetNs(context->enclosingScope->Ns());
    }
    void PostgrammarName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromgrammarName_value = std::move(stack.top());
            context->fromgrammarName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromgrammarName_value.get());
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
        Context(): enclosingScope(), value(), fromgrammarName() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Grammar* value;
        std::u32string fromgrammarName;
    };
};

class LibraryFile::GrammarContentRule : public gendoc::parsing::Rule
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
        gendoc::parsing::NonterminalParser* ruleNonterminalParser = GetNonterminal(ToUtf32("Rule"));
        ruleNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRule));
        ruleNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GrammarContentRule>(this, &GrammarContentRule::PostRule));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->AddRule(context->fromRule);
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

class LibraryFile::RuleRule : public gendoc::parsing::Rule
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
        gendoc::parsing::NonterminalParser* ruleNameNonterminalParser = GetNonterminal(ToUtf32("ruleName"));
        ruleNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RuleRule>(this, &RuleRule::PostruleName));
        gendoc::parsing::NonterminalParser* signatureNonterminalParser = GetNonterminal(ToUtf32("Signature"));
        signatureNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreSignature));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::parsing::Rule(context->fromruleName, context->enclosingScope);
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
    void PreSignature(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Rule*>(context->value)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enclosingScope(), value(), fromruleName() {}
        gendoc::parsing::Scope* enclosingScope;
        gendoc::parsing::Rule* value;
        std::u32string fromruleName;
    };
};

void LibraryFile::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.syntax.Element"));
    if (!grammar1)
    {
        grammar1 = gendoc::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void LibraryFile::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("gendoc.parsing.stdlib.identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("gendoc.parsing.stdlib.qualified_id")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Element.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("gendoc.parsing.stdlib.spaces_and_comments")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Signature"), this, ToUtf32("Element.Signature")));
    AddRule(new LibraryFileRule(ToUtf32("LibraryFile"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 1)));
    AddRule(new NamespaceContentRule(ToUtf32("NamespaceContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Grammar"), ToUtf32("Grammar"), 1)),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Namespace"), ToUtf32("Namespace"), 1))))));
    AddRule(new NamespaceRule(ToUtf32("Namespace"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("namespace")),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("ns"), ToUtf32("qualified_id"), 0)))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser('{'))),
                new gendoc::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser('}'))))));
    AddRule(new GrammarRule(ToUtf32("Grammar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("grammar")),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("grammarName"), ToUtf32("Identifier"), 0)))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser('{'))),
                new gendoc::parsing::NonterminalParser(ToUtf32("GrammarContent"), ToUtf32("GrammarContent"), 1)),
            new gendoc::parsing::ExpectationParser(
                new gendoc::parsing::CharParser('}')))));
    AddRule(new GrammarContentRule(ToUtf32("GrammarContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Rule"), ToUtf32("Rule"), 1))))));
    AddRule(new RuleRule(ToUtf32("Rule"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ruleName"), ToUtf32("Identifier"), 0)),
                new gendoc::parsing::NonterminalParser(ToUtf32("Signature"), ToUtf32("Signature"), 1)),
            new gendoc::parsing::CharParser(';'))));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace gendoc.syntax
