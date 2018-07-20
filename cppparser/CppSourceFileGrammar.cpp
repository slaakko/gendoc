#include "CppSourceFileGrammar.hpp"
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
#include <gendoc/cppparser/Declaration.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

CppSourceFile* CppSourceFile::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

CppSourceFile* CppSourceFile::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    CppSourceFile* grammar(new CppSourceFile(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

CppSourceFile::CppSourceFile(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("CppSourceFile"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

void CppSourceFile::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, SourceFileNode* sourceFile)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<SourceFileNode*>(sourceFile)));
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

class CppSourceFile::SourceFileRule : public gendoc::parsing::Rule
{
public:
    SourceFileRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("SourceFileNode*"), ToUtf32("sourceFile")));
        AddLocalVariable(AttrOrVariable(ToUtf32("ParsingContext"), ToUtf32("ctx")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SourceFileRule>(this, &SourceFileRule::A0Action));
        gendoc::parsing::NonterminalParser* declarationsNonterminalParser = GetNonterminal(ToUtf32("Declarations"));
        declarationsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SourceFileRule>(this, &SourceFileRule::PreDeclarations));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx.SetSourceFileNode(context->sourceFile);
    }
    void PreDeclarations(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<NamespaceNode*>(context->sourceFile->GlobalNs())));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(&context->ctx)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): sourceFile(), ctx() {}
        SourceFileNode* sourceFile;
        ParsingContext ctx;
    };
};

class CppSourceFile::DeclarationsRule : public gendoc::parsing::Rule
{
public:
    DeclarationsRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("NamespaceNode*"), ToUtf32("ns")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> ns_value = std::move(stack.top());
        context->ns = *static_cast<gendoc::parsing::ValueObject<NamespaceNode*>*>(ns_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationsRule>(this, &DeclarationsRule::A0Action));
        gendoc::parsing::NonterminalParser* declarationNonterminalParser = GetNonterminal(ToUtf32("Declaration"));
        declarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationsRule>(this, &DeclarationsRule::PreDeclaration));
        declarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationsRule>(this, &DeclarationsRule::PostDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ns->AddMember(span, context->fromDeclaration);
    }
    void PreDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclaration_value = std::move(stack.top());
            context->fromDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ns(), ctx(), fromDeclaration() {}
        NamespaceNode* ns;
        ParsingContext* ctx;
        Node* fromDeclaration;
    };
};

void CppSourceFile::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Declaration"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::Declaration::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void CppSourceFile::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("comment"), this, ToUtf32("gendoc.parsing.stdlib.comment")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Declaration"), this, ToUtf32("Declaration.Declaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("newline"), this, ToUtf32("gendoc.parsing.stdlib.newline")));
    AddRule(new SourceFileRule(ToUtf32("SourceFile"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::EmptyParser()),
            new gendoc::parsing::NonterminalParser(ToUtf32("Declarations"), ToUtf32("Declarations"), 2))));
    AddRule(new DeclarationsRule(ToUtf32("Declarations"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Declaration"), ToUtf32("Declaration"), 1))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("CommentsAndSpacesAndPPLines"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("PPLine"), ToUtf32("PPLine"), 0),
                        new gendoc::parsing::NonterminalParser(ToUtf32("comment"), ToUtf32("comment"), 0)),
                    new gendoc::parsing::SpaceParser())))));
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
    SetSkipRuleName(ToUtf32("CommentsAndSpacesAndPPLines"));
}

} } // namespace gendoc.cppparser
