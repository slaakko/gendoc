#include "IncludeDirectiveGrammar.hpp"
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

IncludeDirective* IncludeDirective::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

IncludeDirective* IncludeDirective::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    IncludeDirective* grammar(new IncludeDirective(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

IncludeDirective::IncludeDirective(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("IncludeDirective"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

void IncludeDirective::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, SourceFileNode* sourceFileNode)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<SourceFileNode*>(sourceFileNode)));
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

class IncludeDirective::SourceFileRule : public gendoc::parsing::Rule
{
public:
    SourceFileRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("SourceFileNode*"), ToUtf32("sourceFileNode")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> sourceFileNode_value = std::move(stack.top());
        context->sourceFileNode = *static_cast<gendoc::parsing::ValueObject<SourceFileNode*>*>(sourceFileNode_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* sourceLineNonterminalParser = GetNonterminal(ToUtf32("SourceLine"));
        sourceLineNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SourceFileRule>(this, &SourceFileRule::PreSourceLine));
    }
    void PreSourceLine(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<SourceFileNode*>(context->sourceFileNode)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): sourceFileNode() {}
        SourceFileNode* sourceFileNode;
    };
};

class IncludeDirective::SourceLineRule : public gendoc::parsing::Rule
{
public:
    SourceLineRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("SourceFileNode*"), ToUtf32("sourceFile")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> sourceFile_value = std::move(stack.top());
        context->sourceFile = *static_cast<gendoc::parsing::ValueObject<SourceFileNode*>*>(sourceFile_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* includeDirectiveLineNonterminalParser = GetNonterminal(ToUtf32("IncludeDirectiveLine"));
        includeDirectiveLineNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SourceLineRule>(this, &SourceLineRule::PreIncludeDirectiveLine));
    }
    void PreIncludeDirectiveLine(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<SourceFileNode*>(context->sourceFile)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): sourceFile() {}
        SourceFileNode* sourceFile;
    };
};

class IncludeDirective::IncludeDirectiveLineRule : public gendoc::parsing::Rule
{
public:
    IncludeDirectiveLineRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("SourceFileNode*"), ToUtf32("sourceFile")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> sourceFile_value = std::move(stack.top());
        context->sourceFile = *static_cast<gendoc::parsing::ValueObject<SourceFileNode*>*>(sourceFile_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IncludeDirectiveLineRule>(this, &IncludeDirectiveLineRule::A0Action));
        gendoc::parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal(ToUtf32("FilePath"));
        filePathNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IncludeDirectiveLineRule>(this, &IncludeDirectiveLineRule::PostFilePath));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->sourceFile->AddIncludeDirective(new IncludeDirectiveNode(span, context->fromFilePath));
    }
    void PostFilePath(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFilePath_value = std::move(stack.top());
            context->fromFilePath = *static_cast<gendoc::parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): sourceFile(), fromFilePath() {}
        SourceFileNode* sourceFile;
        std::string fromFilePath;
    };
};

class IncludeDirective::FilePathRule : public gendoc::parsing::Rule
{
public:
    FilePathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::string"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FilePathRule>(this, &FilePathRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FilePathRule>(this, &FilePathRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf8(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf8(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        std::string value;
    };
};

void IncludeDirective::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void IncludeDirective::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("newline"), this, ToUtf32("gendoc.parsing.stdlib.newline")));
    AddRule(new SourceFileRule(ToUtf32("SourceFile"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("SourceLine"), ToUtf32("SourceLine"), 1))));
    AddRule(new SourceLineRule(ToUtf32("SourceLine"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("IncludeDirectiveLine"), ToUtf32("IncludeDirectiveLine"), 1),
            new gendoc::parsing::NonterminalParser(ToUtf32("OtherLine"), ToUtf32("OtherLine"), 0))));
    AddRule(new IncludeDirectiveLineRule(ToUtf32("IncludeDirectiveLine"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::KleeneStarParser(
                                                new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                            new gendoc::parsing::CharParser('#')),
                                        new gendoc::parsing::KleeneStarParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                    new gendoc::parsing::KeywordParser(ToUtf32("include"))),
                                new gendoc::parsing::KleeneStarParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new gendoc::parsing::NonterminalParser(ToUtf32("FilePath"), ToUtf32("FilePath"), 0)),
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::DifferenceParser(
                                    new gendoc::parsing::AnyCharParser(),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))))),
                    new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("OtherLine"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::DifferenceParser(
                        new gendoc::parsing::AnyCharParser(),
                        new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0)))),
            new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))));
    AddRule(new FilePathRule(ToUtf32("FilePath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('<'),
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::KleeneStarParser(
                                new gendoc::parsing::CharSetParser(ToUtf32(">"), true))))),
                new gendoc::parsing::CharParser('>')),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('\"'),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::KleeneStarParser(
                                new gendoc::parsing::CharSetParser(ToUtf32("\""), true))))),
                new gendoc::parsing::CharParser('\"')))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("S"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::CharParser(' '),
                    new gendoc::parsing::CharParser('\t'))))));
}

} } // namespace gendoc.cppparser
