#include "Statement.hpp"
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
#include <gendoc/cppparser/Identifier.hpp>
#include <gendoc/cppparser/Expression.hpp>
#include <gendoc/cppparser/TypeExpr.hpp>
#include <gendoc/cppparser/Declarator.hpp>
#include <gendoc/cppparser/Declaration.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Statement* Statement::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Statement* Statement::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Statement* grammar(new Statement(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Statement::Statement(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Statement"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

Node* Statement::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<ParsingContext*>(ctx)));
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
    Node* result = *static_cast<gendoc::parsing::ValueObject<Node*>*>(value.get());
    stack.pop();
    return result;
}

class Statement::StatementRule : public gendoc::parsing::Rule
{
public:
    StatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A7Action));
        gendoc::parsing::NonterminalParser* labeledStatementNonterminalParser = GetNonterminal(ToUtf32("LabeledStatement"));
        labeledStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreLabeledStatement));
        labeledStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostLabeledStatement));
        gendoc::parsing::NonterminalParser* expressionStatementNonterminalParser = GetNonterminal(ToUtf32("ExpressionStatement"));
        expressionStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreExpressionStatement));
        expressionStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostExpressionStatement));
        gendoc::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostCompoundStatement));
        gendoc::parsing::NonterminalParser* selectionStatementNonterminalParser = GetNonterminal(ToUtf32("SelectionStatement"));
        selectionStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreSelectionStatement));
        selectionStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostSelectionStatement));
        gendoc::parsing::NonterminalParser* iterationStatementNonterminalParser = GetNonterminal(ToUtf32("IterationStatement"));
        iterationStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreIterationStatement));
        iterationStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostIterationStatement));
        gendoc::parsing::NonterminalParser* jumpStatementNonterminalParser = GetNonterminal(ToUtf32("JumpStatement"));
        jumpStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreJumpStatement));
        jumpStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostJumpStatement));
        gendoc::parsing::NonterminalParser* declarationStatementNonterminalParser = GetNonterminal(ToUtf32("DeclarationStatement"));
        declarationStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreDeclarationStatement));
        declarationStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostDeclarationStatement));
        gendoc::parsing::NonterminalParser* tryBlockNonterminalParser = GetNonterminal(ToUtf32("TryBlock"));
        tryBlockNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<StatementRule>(this, &StatementRule::PreTryBlock));
        tryBlockNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostTryBlock));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLabeledStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionStatement;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCompoundStatement;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSelectionStatement;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIterationStatement;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromJumpStatement;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDeclarationStatement;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTryBlock;
    }
    void PreLabeledStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostLabeledStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLabeledStatement_value = std::move(stack.top());
            context->fromLabeledStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromLabeledStatement_value.get());
            stack.pop();
        }
    }
    void PreExpressionStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpressionStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpressionStatement_value = std::move(stack.top());
            context->fromExpressionStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExpressionStatement_value.get());
            stack.pop();
        }
    }
    void PreCompoundStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCompoundStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<gendoc::parsing::ValueObject<CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
    void PreSelectionStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostSelectionStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSelectionStatement_value = std::move(stack.top());
            context->fromSelectionStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromSelectionStatement_value.get());
            stack.pop();
        }
    }
    void PreIterationStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostIterationStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIterationStatement_value = std::move(stack.top());
            context->fromIterationStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromIterationStatement_value.get());
            stack.pop();
        }
    }
    void PreJumpStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostJumpStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromJumpStatement_value = std::move(stack.top());
            context->fromJumpStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromJumpStatement_value.get());
            stack.pop();
        }
    }
    void PreDeclarationStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDeclarationStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclarationStatement_value = std::move(stack.top());
            context->fromDeclarationStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromDeclarationStatement_value.get());
            stack.pop();
        }
    }
    void PreTryBlock(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTryBlock(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTryBlock_value = std::move(stack.top());
            context->fromTryBlock = *static_cast<gendoc::parsing::ValueObject<TryStatementNode*>*>(fromTryBlock_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromLabeledStatement(), fromExpressionStatement(), fromCompoundStatement(), fromSelectionStatement(), fromIterationStatement(), fromJumpStatement(), fromDeclarationStatement(), fromTryBlock() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromLabeledStatement;
        Node* fromExpressionStatement;
        CompoundStatementNode* fromCompoundStatement;
        Node* fromSelectionStatement;
        Node* fromIterationStatement;
        Node* fromJumpStatement;
        Node* fromDeclarationStatement;
        TryStatementNode* fromTryBlock;
    };
};

class Statement::LabeledStatementRule : public gendoc::parsing::Rule
{
public:
    LabeledStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A2Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostIdentifier));
        gendoc::parsing::NonterminalParser* lsNonterminalParser = GetNonterminal(ToUtf32("ls"));
        lsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LabeledStatementRule>(this, &LabeledStatementRule::Prels));
        lsNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Postls));
        gendoc::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LabeledStatementRule>(this, &LabeledStatementRule::PreConstantExpression));
        constantExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostConstantExpression));
        gendoc::parsing::NonterminalParser* csNonterminalParser = GetNonterminal(ToUtf32("cs"));
        csNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LabeledStatementRule>(this, &LabeledStatementRule::Precs));
        csNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Postcs));
        gendoc::parsing::NonterminalParser* dsNonterminalParser = GetNonterminal(ToUtf32("ds"));
        dsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LabeledStatementRule>(this, &LabeledStatementRule::Preds));
        dsNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Postds));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LabeledStatementNode(span, context->fromIdentifier, context->fromls);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CaseStatementNode(span, context->fromConstantExpression, context->fromcs);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DefaultStatementNode(span, context->fromds);
    }
    void PostIdentifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void Prels(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postls(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromls_value = std::move(stack.top());
            context->fromls = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromls_value.get());
            stack.pop();
        }
    }
    void PreConstantExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConstantExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromConstantExpression_value = std::move(stack.top());
            context->fromConstantExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromConstantExpression_value.get());
            stack.pop();
        }
    }
    void Precs(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postcs(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromcs_value = std::move(stack.top());
            context->fromcs = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromcs_value.get());
            stack.pop();
        }
    }
    void Preds(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postds(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromds_value = std::move(stack.top());
            context->fromds = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromds_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromIdentifier(), fromls(), fromConstantExpression(), fromcs(), fromds() {}
        ParsingContext* ctx;
        Node* value;
        std::u32string fromIdentifier;
        Node* fromls;
        Node* fromConstantExpression;
        Node* fromcs;
        Node* fromds;
    };
};

class Statement::ExpressionStatementRule : public gendoc::parsing::Rule
{
public:
    ExpressionStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpressionStatementRule>(this, &ExpressionStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExpressionStatementRule>(this, &ExpressionStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExpressionStatementRule>(this, &ExpressionStatementRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExpressionStatementNode(span, context->fromExpression);
    }
    void PreExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromExpression() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExpression;
    };
};

class Statement::CompoundStatementRule : public gendoc::parsing::Rule
{
public:
    CompoundStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("CompoundStatementNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CompoundStatementNode>"), ToUtf32("node")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<CompoundStatementNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A2Action));
        gendoc::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<CompoundStatementRule>(this, &CompoundStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CompoundStatementRule>(this, &CompoundStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(new CompoundStatementNode(span));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node->AddStatement(span, context->fromStatement);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node->SetEndBraceSpan(span);
        context->value = context->node.release();
    }
    void PreStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), node(), fromStatement() {}
        ParsingContext* ctx;
        CompoundStatementNode* value;
        std::unique_ptr<CompoundStatementNode> node;
        Node* fromStatement;
    };
};

class Statement::SelectionStatementRule : public gendoc::parsing::Rule
{
public:
    SelectionStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("elseSpan")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SelectionStatementRule>(this, &SelectionStatementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SelectionStatementRule>(this, &SelectionStatementRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SelectionStatementRule>(this, &SelectionStatementRule::A2Action));
        gendoc::parsing::NonterminalParser* c1NonterminalParser = GetNonterminal(ToUtf32("c1"));
        c1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SelectionStatementRule>(this, &SelectionStatementRule::Prec1));
        c1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::Postc1));
        gendoc::parsing::NonterminalParser* thenSNonterminalParser = GetNonterminal(ToUtf32("thenS"));
        thenSNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SelectionStatementRule>(this, &SelectionStatementRule::PrethenS));
        thenSNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::PostthenS));
        gendoc::parsing::NonterminalParser* elseSNonterminalParser = GetNonterminal(ToUtf32("elseS"));
        elseSNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SelectionStatementRule>(this, &SelectionStatementRule::PreelseS));
        elseSNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::PostelseS));
        gendoc::parsing::NonterminalParser* c2NonterminalParser = GetNonterminal(ToUtf32("c2"));
        c2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SelectionStatementRule>(this, &SelectionStatementRule::Prec2));
        c2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::Postc2));
        gendoc::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SelectionStatementRule>(this, &SelectionStatementRule::PreStatement));
        statementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IfStatementNode(span, context->fromc1, context->fromthenS, context->fromelseS, context->elseSpan);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->elseSpan = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SwitchStatementNode(span, context->fromc2, context->fromStatement);
    }
    void Prec1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postc1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromc1_value = std::move(stack.top());
            context->fromc1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromc1_value.get());
            stack.pop();
        }
    }
    void PrethenS(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostthenS(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromthenS_value = std::move(stack.top());
            context->fromthenS = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromthenS_value.get());
            stack.pop();
        }
    }
    void PreelseS(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostelseS(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromelseS_value = std::move(stack.top());
            context->fromelseS = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromelseS_value.get());
            stack.pop();
        }
    }
    void Prec2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postc2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromc2_value = std::move(stack.top());
            context->fromc2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromc2_value.get());
            stack.pop();
        }
    }
    void PreStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), elseSpan(), fromc1(), fromthenS(), fromelseS(), fromc2(), fromStatement() {}
        ParsingContext* ctx;
        Node* value;
        Span elseSpan;
        Node* fromc1;
        Node* fromthenS;
        Node* fromelseS;
        Node* fromc2;
        Node* fromStatement;
    };
};

class Statement::ConditionRule : public gendoc::parsing::Rule
{
public:
    ConditionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("typeExpr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("declarator")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A5Action));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConditionRule>(this, &ConditionRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostTypeExpr));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConditionRule>(this, &ConditionRule::PreDeclarator));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostDeclarator));
        gendoc::parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal(ToUtf32("InitializerClause"));
        initializerClauseNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConditionRule>(this, &ConditionRule::PreInitializerClause));
        initializerClauseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostInitializerClause));
        gendoc::parsing::NonterminalParser* bracedInitializerListNonterminalParser = GetNonterminal(ToUtf32("BracedInitializerList"));
        bracedInitializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConditionRule>(this, &ConditionRule::PreBracedInitializerList));
        bracedInitializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostBracedInitializerList));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConditionRule>(this, &ConditionRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeExpr.reset(context->fromTypeExpr);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->declarator.reset(context->fromDeclarator);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->value = new SimpleDeclarationNode(context->s, Specifier::none, context->typeExpr.release(), new InitDeclaratorNode(context->s, context->declarator.release(), new AssignmentInitializerNode(context->s, context->fromInitializerClause)));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->value = new SimpleDeclarationNode(context->s, Specifier::none, context->typeExpr.release(), new InitDeclaratorNode(context->s, context->declarator.release(), context->fromBracedInitializerList));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpression;
    }
    void PreTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context->fromTypeExpr = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PreDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
    void PreInitializerClause(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInitializerClause(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromInitializerClause_value = std::move(stack.top());
            context->fromInitializerClause = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromInitializerClause_value.get());
            stack.pop();
        }
    }
    void PreBracedInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostBracedInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBracedInitializerList_value = std::move(stack.top());
            context->fromBracedInitializerList = *static_cast<gendoc::parsing::ValueObject<BracedInitializerListNode*>*>(fromBracedInitializerList_value.get());
            stack.pop();
        }
    }
    void PreExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), typeExpr(), declarator(), s(), fromTypeExpr(), fromDeclarator(), fromInitializerClause(), fromBracedInitializerList(), fromExpression() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> typeExpr;
        std::unique_ptr<Node> declarator;
        Span s;
        Node* fromTypeExpr;
        Node* fromDeclarator;
        Node* fromInitializerClause;
        BracedInitializerListNode* fromBracedInitializerList;
        Node* fromExpression;
    };
};

class Statement::IterationStatementRule : public gendoc::parsing::Rule
{
public:
    IterationStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("forRange")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("whileSpan")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A5Action));
        gendoc::parsing::NonterminalParser* c1NonterminalParser = GetNonterminal(ToUtf32("c1"));
        c1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::Prec1));
        c1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::Postc1));
        gendoc::parsing::NonterminalParser* s1NonterminalParser = GetNonterminal(ToUtf32("s1"));
        s1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::Pres1));
        s1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::Posts1));
        gendoc::parsing::NonterminalParser* s2NonterminalParser = GetNonterminal(ToUtf32("s2"));
        s2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::Pres2));
        s2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::Posts2));
        gendoc::parsing::NonterminalParser* c2NonterminalParser = GetNonterminal(ToUtf32("c2"));
        c2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::Prec2));
        c2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::Postc2));
        gendoc::parsing::NonterminalParser* forRangeDeclarationNonterminalParser = GetNonterminal(ToUtf32("ForRangeDeclaration"));
        forRangeDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::PreForRangeDeclaration));
        forRangeDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostForRangeDeclaration));
        gendoc::parsing::NonterminalParser* forRangeInitializerNonterminalParser = GetNonterminal(ToUtf32("ForRangeInitializer"));
        forRangeInitializerNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::PreForRangeInitializer));
        forRangeInitializerNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostForRangeInitializer));
        gendoc::parsing::NonterminalParser* s3NonterminalParser = GetNonterminal(ToUtf32("s3"));
        s3NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::Pres3));
        s3NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::Posts3));
        gendoc::parsing::NonterminalParser* forInitStatementNonterminalParser = GetNonterminal(ToUtf32("ForInitStatement"));
        forInitStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::PreForInitStatement));
        forInitStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostForInitStatement));
        gendoc::parsing::NonterminalParser* c4NonterminalParser = GetNonterminal(ToUtf32("c4"));
        c4NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::Prec4));
        c4NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::Postc4));
        gendoc::parsing::NonterminalParser* loopExprNonterminalParser = GetNonterminal(ToUtf32("loopExpr"));
        loopExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::PreloopExpr));
        loopExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostloopExpr));
        gendoc::parsing::NonterminalParser* s4NonterminalParser = GetNonterminal(ToUtf32("s4"));
        s4NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IterationStatementRule>(this, &IterationStatementRule::Pres4));
        s4NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::Posts4));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new WhileStatementNode(span, context->fromc1, context->froms1);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DoStatementNode(span, context->froms2, context->fromc2, context->whileSpan);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->whileSpan = span;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new RangeForStatementNode(span, context->forRange.release(), context->fromForRangeInitializer, context->froms3);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->forRange.reset(context->fromForRangeDeclaration);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ForStatementNode(span, context->fromForInitStatement, context->fromc4, context->fromloopExpr, context->froms4);
    }
    void Prec1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postc1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromc1_value = std::move(stack.top());
            context->fromc1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromc1_value.get());
            stack.pop();
        }
    }
    void Pres1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Posts1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms1_value = std::move(stack.top());
            context->froms1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(froms1_value.get());
            stack.pop();
        }
    }
    void Pres2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Posts2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms2_value = std::move(stack.top());
            context->froms2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(froms2_value.get());
            stack.pop();
        }
    }
    void Prec2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postc2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromc2_value = std::move(stack.top());
            context->fromc2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromc2_value.get());
            stack.pop();
        }
    }
    void PreForRangeDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostForRangeDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromForRangeDeclaration_value = std::move(stack.top());
            context->fromForRangeDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromForRangeDeclaration_value.get());
            stack.pop();
        }
    }
    void PreForRangeInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostForRangeInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromForRangeInitializer_value = std::move(stack.top());
            context->fromForRangeInitializer = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromForRangeInitializer_value.get());
            stack.pop();
        }
    }
    void Pres3(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Posts3(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms3_value = std::move(stack.top());
            context->froms3 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(froms3_value.get());
            stack.pop();
        }
    }
    void PreForInitStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostForInitStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromForInitStatement_value = std::move(stack.top());
            context->fromForInitStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromForInitStatement_value.get());
            stack.pop();
        }
    }
    void Prec4(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postc4(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromc4_value = std::move(stack.top());
            context->fromc4 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromc4_value.get());
            stack.pop();
        }
    }
    void PreloopExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostloopExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromloopExpr_value = std::move(stack.top());
            context->fromloopExpr = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromloopExpr_value.get());
            stack.pop();
        }
    }
    void Pres4(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Posts4(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms4_value = std::move(stack.top());
            context->froms4 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(froms4_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), forRange(), whileSpan(), fromc1(), froms1(), froms2(), fromc2(), fromForRangeDeclaration(), fromForRangeInitializer(), froms3(), fromForInitStatement(), fromc4(), fromloopExpr(), froms4() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> forRange;
        Span whileSpan;
        Node* fromc1;
        Node* froms1;
        Node* froms2;
        Node* fromc2;
        Node* fromForRangeDeclaration;
        Node* fromForRangeInitializer;
        Node* froms3;
        Node* fromForInitStatement;
        Node* fromc4;
        Node* fromloopExpr;
        Node* froms4;
    };
};

class Statement::ForInitStatementRule : public gendoc::parsing::Rule
{
public:
    ForInitStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A1Action));
        gendoc::parsing::NonterminalParser* expressionStatementNonterminalParser = GetNonterminal(ToUtf32("ExpressionStatement"));
        expressionStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ForInitStatementRule>(this, &ForInitStatementRule::PreExpressionStatement));
        expressionStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostExpressionStatement));
        gendoc::parsing::NonterminalParser* simpleDeclarationNonterminalParser = GetNonterminal(ToUtf32("SimpleDeclaration"));
        simpleDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ForInitStatementRule>(this, &ForInitStatementRule::PreSimpleDeclaration));
        simpleDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostSimpleDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleDeclaration;
    }
    void PreExpressionStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpressionStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpressionStatement_value = std::move(stack.top());
            context->fromExpressionStatement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExpressionStatement_value.get());
            stack.pop();
        }
    }
    void PreSimpleDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostSimpleDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSimpleDeclaration_value = std::move(stack.top());
            context->fromSimpleDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromSimpleDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromExpressionStatement(), fromSimpleDeclaration() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExpressionStatement;
        Node* fromSimpleDeclaration;
    };
};

class Statement::ForRangeDeclarationRule : public gendoc::parsing::Rule
{
public:
    ForRangeDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("typeExpr")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForRangeDeclarationRule>(this, &ForRangeDeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForRangeDeclarationRule>(this, &ForRangeDeclarationRule::A1Action));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ForRangeDeclarationRule>(this, &ForRangeDeclarationRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForRangeDeclarationRule>(this, &ForRangeDeclarationRule::PostTypeExpr));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ForRangeDeclarationRule>(this, &ForRangeDeclarationRule::PreDeclarator));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForRangeDeclarationRule>(this, &ForRangeDeclarationRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SimpleDeclarationNode(span, Specifier::none, context->typeExpr.release(), context->fromDeclarator);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeExpr.reset(context->fromTypeExpr);
    }
    void PreTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context->fromTypeExpr = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PreDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), typeExpr(), fromTypeExpr(), fromDeclarator() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> typeExpr;
        Node* fromTypeExpr;
        Node* fromDeclarator;
    };
};

class Statement::ForRangeInitializerRule : public gendoc::parsing::Rule
{
public:
    ForRangeInitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForRangeInitializerRule>(this, &ForRangeInitializerRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForRangeInitializerRule>(this, &ForRangeInitializerRule::A1Action));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ForRangeInitializerRule>(this, &ForRangeInitializerRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForRangeInitializerRule>(this, &ForRangeInitializerRule::PostExpression));
        gendoc::parsing::NonterminalParser* bracedInitializerListNonterminalParser = GetNonterminal(ToUtf32("BracedInitializerList"));
        bracedInitializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ForRangeInitializerRule>(this, &ForRangeInitializerRule::PreBracedInitializerList));
        bracedInitializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForRangeInitializerRule>(this, &ForRangeInitializerRule::PostBracedInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpression;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBracedInitializerList;
    }
    void PreExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PreBracedInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostBracedInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBracedInitializerList_value = std::move(stack.top());
            context->fromBracedInitializerList = *static_cast<gendoc::parsing::ValueObject<BracedInitializerListNode*>*>(fromBracedInitializerList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromExpression(), fromBracedInitializerList() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExpression;
        BracedInitializerListNode* fromBracedInitializerList;
    };
};

class Statement::JumpStatementRule : public gendoc::parsing::Rule
{
public:
    JumpStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A4Action));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<JumpStatementRule>(this, &JumpStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostExpression));
        gendoc::parsing::NonterminalParser* bracedInitializerListNonterminalParser = GetNonterminal(ToUtf32("BracedInitializerList"));
        bracedInitializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<JumpStatementRule>(this, &JumpStatementRule::PreBracedInitializerList));
        bracedInitializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostBracedInitializerList));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BreakStatementNode(span);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ContinueStatementNode(span);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ReturnStatementNode(span, context->fromExpression);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ReturnStatementNode(span, context->fromBracedInitializerList);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new GotoStatementNode(span, context->fromIdentifier);
    }
    void PreExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PreBracedInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostBracedInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBracedInitializerList_value = std::move(stack.top());
            context->fromBracedInitializerList = *static_cast<gendoc::parsing::ValueObject<BracedInitializerListNode*>*>(fromBracedInitializerList_value.get());
            stack.pop();
        }
    }
    void PostIdentifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromExpression(), fromBracedInitializerList(), fromIdentifier() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExpression;
        BracedInitializerListNode* fromBracedInitializerList;
        std::u32string fromIdentifier;
    };
};

class Statement::DeclarationStatementRule : public gendoc::parsing::Rule
{
public:
    DeclarationStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationStatementRule>(this, &DeclarationStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* blockDeclarationNonterminalParser = GetNonterminal(ToUtf32("BlockDeclaration"));
        blockDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationStatementRule>(this, &DeclarationStatementRule::PreBlockDeclaration));
        blockDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationStatementRule>(this, &DeclarationStatementRule::PostBlockDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DeclarationStatementNode(span, context->fromBlockDeclaration);
    }
    void PreBlockDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostBlockDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBlockDeclaration_value = std::move(stack.top());
            context->fromBlockDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromBlockDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromBlockDeclaration() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromBlockDeclaration;
    };
};

class Statement::TryBlockRule : public gendoc::parsing::Rule
{
public:
    TryBlockRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("TryStatementNode*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<TryStatementNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TryBlockRule>(this, &TryBlockRule::A0Action));
        gendoc::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TryBlockRule>(this, &TryBlockRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TryBlockRule>(this, &TryBlockRule::PostCompoundStatement));
        gendoc::parsing::NonterminalParser* handlerSequenceNonterminalParser = GetNonterminal(ToUtf32("HandlerSequence"));
        handlerSequenceNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TryBlockRule>(this, &TryBlockRule::PreHandlerSequence));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TryStatementNode(span, context->fromCompoundStatement);
    }
    void PreCompoundStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCompoundStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<gendoc::parsing::ValueObject<CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
    void PreHandlerSequence(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<TryStatementNode*>(context->value)));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromCompoundStatement() {}
        ParsingContext* ctx;
        TryStatementNode* value;
        CompoundStatementNode* fromCompoundStatement;
    };
};

class Statement::HandlerSequenceRule : public gendoc::parsing::Rule
{
public:
    HandlerSequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("TryStatementNode*"), ToUtf32("tryStatementNode")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> tryStatementNode_value = std::move(stack.top());
        context->tryStatementNode = *static_cast<gendoc::parsing::ValueObject<TryStatementNode*>*>(tryStatementNode_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<HandlerSequenceRule>(this, &HandlerSequenceRule::A0Action));
        gendoc::parsing::NonterminalParser* handlerNonterminalParser = GetNonterminal(ToUtf32("Handler"));
        handlerNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<HandlerSequenceRule>(this, &HandlerSequenceRule::PreHandler));
        handlerNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<HandlerSequenceRule>(this, &HandlerSequenceRule::PostHandler));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->tryStatementNode->AddHandler(span, context->fromHandler);
    }
    void PreHandler(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostHandler(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromHandler_value = std::move(stack.top());
            context->fromHandler = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromHandler_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): tryStatementNode(), ctx(), fromHandler() {}
        TryStatementNode* tryStatementNode;
        ParsingContext* ctx;
        Node* fromHandler;
    };
};

class Statement::HandlerRule : public gendoc::parsing::Rule
{
public:
    HandlerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<HandlerRule>(this, &HandlerRule::A0Action));
        gendoc::parsing::NonterminalParser* exceptionDeclarationNonterminalParser = GetNonterminal(ToUtf32("ExceptionDeclaration"));
        exceptionDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<HandlerRule>(this, &HandlerRule::PreExceptionDeclaration));
        exceptionDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostExceptionDeclaration));
        gendoc::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<HandlerRule>(this, &HandlerRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostCompoundStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new HandlerNode(span, context->fromExceptionDeclaration, context->fromCompoundStatement);
    }
    void PreExceptionDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExceptionDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExceptionDeclaration_value = std::move(stack.top());
            context->fromExceptionDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExceptionDeclaration_value.get());
            stack.pop();
        }
    }
    void PreCompoundStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCompoundStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<gendoc::parsing::ValueObject<CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromExceptionDeclaration(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExceptionDeclaration;
        CompoundStatementNode* fromCompoundStatement;
    };
};

class Statement::ExceptionDeclarationRule : public gendoc::parsing::Rule
{
public:
    ExceptionDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("typeExpr")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A2Action));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PostTypeExpr));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PreDeclarator));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SimpleDeclarationNode(span, Specifier::none, context->typeExpr.release(), context->fromDeclarator);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeExpr.reset(context->fromTypeExpr);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CatchAllNode(span);
    }
    void PreTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context->fromTypeExpr = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PreDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), typeExpr(), fromTypeExpr(), fromDeclarator() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> typeExpr;
        Node* fromTypeExpr;
        Node* fromDeclarator;
    };
};

void Statement::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Expression"));
    if (!grammar0)
    {
        grammar0 = gendoc::cppparser::Expression::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Declarator"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::Declarator::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Identifier"));
    if (!grammar2)
    {
        grammar2 = gendoc::cppparser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar2);
    gendoc::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("gendoc.cppparser.TypeExpr"));
    if (!grammar3)
    {
        grammar3 = gendoc::cppparser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar3);
    gendoc::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Declaration"));
    if (!grammar4)
    {
        grammar4 = gendoc::cppparser::Declaration::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void Statement::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ConstantExpression"), this, ToUtf32("Expression.ConstantExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Expression"), this, ToUtf32("Expression.Expression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Declarator"), this, ToUtf32("Declarator.Declarator")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("InitializerClause"), this, ToUtf32("Declarator.InitializerClause")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("BlockDeclaration"), this, ToUtf32("Declaration.BlockDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("BracedInitializerList"), this, ToUtf32("Declarator.BracedInitializerList")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("SimpleDeclaration"), this, ToUtf32("Declaration.SimpleDeclaration")));
    AddRule(new StatementRule(ToUtf32("Statement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("LabeledStatement"), ToUtf32("LabeledStatement"), 1)),
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionStatement"), ToUtf32("ExpressionStatement"), 1))),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 1))),
                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("SelectionStatement"), ToUtf32("SelectionStatement"), 1))),
                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("IterationStatement"), ToUtf32("IterationStatement"), 1))),
                    new gendoc::parsing::ActionParser(ToUtf32("A5"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("JumpStatement"), ToUtf32("JumpStatement"), 1))),
                new gendoc::parsing::ActionParser(ToUtf32("A6"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("DeclarationStatement"), ToUtf32("DeclarationStatement"), 1))),
            new gendoc::parsing::ActionParser(ToUtf32("A7"),
                new gendoc::parsing::NonterminalParser(ToUtf32("TryBlock"), ToUtf32("TryBlock"), 1)))));
    AddRule(new LabeledStatementRule(ToUtf32("LabeledStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::DifferenceParser(
                                        new gendoc::parsing::CharParser(':'),
                                        new gendoc::parsing::StringParser(ToUtf32("::"))))),
                            new gendoc::parsing::NonterminalParser(ToUtf32("ls"), ToUtf32("Statement"), 1)))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("case")),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 1))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser(':'))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("cs"), ToUtf32("Statement"), 1)))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("default")),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::CharParser(':'))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("ds"), ToUtf32("Statement"), 1))))))));
    AddRule(new ExpressionStatementRule(ToUtf32("ExpressionStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1)),
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new CompoundStatementRule(ToUtf32("CompoundStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::CharParser('{')),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 1))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser('}'))))));
    AddRule(new SelectionStatementRule(ToUtf32("SelectionStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::KeywordParser(ToUtf32("if")),
                                        new gendoc::parsing::ExpectationParser(
                                            new gendoc::parsing::CharParser('('))),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("c1"), ToUtf32("Condition"), 1))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser(')'))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("thenS"), ToUtf32("Statement"), 1))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::KeywordParser(ToUtf32("else"))),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("elseS"), ToUtf32("Statement"), 1)))))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("switch")),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser('('))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("c2"), ToUtf32("Condition"), 1))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::CharParser(')'))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 1))))))));
    AddRule(new ConditionRule(ToUtf32("Condition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 1))))),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('='),
                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("InitializerClause"), ToUtf32("InitializerClause"), 1))),
                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("BracedInitializerList"), ToUtf32("BracedInitializerList"), 1))))),
            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1)))));
    AddRule(new IterationStatementRule(ToUtf32("IterationStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::KeywordParser(ToUtf32("while")),
                                            new gendoc::parsing::ExpectationParser(
                                                new gendoc::parsing::CharParser('('))),
                                        new gendoc::parsing::ExpectationParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("c1"), ToUtf32("Condition"), 1))),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser(')'))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("Statement"), 1))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::KeywordParser(ToUtf32("do")),
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("s2"), ToUtf32("Statement"), 1)),
                                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                    new gendoc::parsing::ExpectationParser(
                                                        new gendoc::parsing::KeywordParser(ToUtf32("while"))))),
                                            new gendoc::parsing::ExpectationParser(
                                                new gendoc::parsing::CharParser('('))),
                                        new gendoc::parsing::ExpectationParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("c2"), ToUtf32("Expression"), 1))),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser(')'))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser(';')))))),
                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::KeywordParser(ToUtf32("for")),
                                                new gendoc::parsing::CharParser('(')),
                                            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("ForRangeDeclaration"), ToUtf32("ForRangeDeclaration"), 1))),
                                        new gendoc::parsing::CharParser(':')),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("ForRangeInitializer"), ToUtf32("ForRangeInitializer"), 1))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser(')'))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("s3"), ToUtf32("Statement"), 1)))))),
            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::KeywordParser(ToUtf32("for")),
                                                new gendoc::parsing::CharParser('(')),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("ForInitStatement"), ToUtf32("ForInitStatement"), 1)),
                                        new gendoc::parsing::OptionalParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("c4"), ToUtf32("Condition"), 1))),
                                    new gendoc::parsing::CharParser(';')),
                                new gendoc::parsing::OptionalParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("loopExpr"), ToUtf32("Expression"), 1))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::CharParser(')'))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("s4"), ToUtf32("Statement"), 1))))))));
    AddRule(new ForInitStatementRule(ToUtf32("ForInitStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionStatement"), ToUtf32("ExpressionStatement"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("SimpleDeclaration"), ToUtf32("SimpleDeclaration"), 1)))));
    AddRule(new ForRangeDeclarationRule(ToUtf32("ForRangeDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 1))))));
    AddRule(new ForRangeInitializerRule(ToUtf32("ForRangeInitializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("BracedInitializerList"), ToUtf32("BracedInitializerList"), 1)))));
    AddRule(new JumpStatementRule(ToUtf32("JumpStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("break")),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser(';'))))),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("continue")),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser(';')))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("return")),
                                    new gendoc::parsing::OptionalParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser(';')))))),
                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("return")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("BracedInitializerList"), ToUtf32("BracedInitializerList"), 1)),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::CharParser(';')))))),
            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("goto")),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser(';'))))))));
    AddRule(new DeclarationStatementRule(ToUtf32("DeclarationStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("BlockDeclaration"), ToUtf32("BlockDeclaration"), 1))));
    AddRule(new TryBlockRule(ToUtf32("TryBlock"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("try")),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 1))))),
            new gendoc::parsing::ExpectationParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("HandlerSequence"), ToUtf32("HandlerSequence"), 2)))));
    AddRule(new HandlerSequenceRule(ToUtf32("HandlerSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Handler"), ToUtf32("Handler"), 1))))));
    AddRule(new HandlerRule(ToUtf32("Handler"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("catch")),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser('('))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("ExceptionDeclaration"), ToUtf32("ExceptionDeclaration"), 1))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser(')'))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 1)))))));
    AddRule(new ExceptionDeclarationRule(ToUtf32("ExceptionDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 1))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::StringParser(ToUtf32("...")))))));
}

} } // namespace gendoc.cppparser
