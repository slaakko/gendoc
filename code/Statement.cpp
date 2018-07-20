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
#include <gendoc/code/Expression.hpp>
#include <gendoc/code/Declaration.hpp>
#include <gendoc/code/Declarator.hpp>
#include <gendoc/code/Identifier.hpp>
#include <gendoc/codedom/Type.hpp>

namespace gendoc { namespace code {

using namespace gendoc::codedom;
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

Statement::Statement(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Statement"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.code")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::codedom::CompoundStatement* Statement::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    gendoc::codedom::CompoundStatement* result = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CompoundStatement*>*>(value.get());
    stack.pop();
    return result;
}

class Statement::StatementRule : public gendoc::parsing::Rule
{
public:
    StatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
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
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A8Action));
        gendoc::parsing::NonterminalParser* labeledStatementNonterminalParser = GetNonterminal(ToUtf32("LabeledStatement"));
        labeledStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostLabeledStatement));
        gendoc::parsing::NonterminalParser* emptyStatementNonterminalParser = GetNonterminal(ToUtf32("EmptyStatement"));
        emptyStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostEmptyStatement));
        gendoc::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostCompoundStatement));
        gendoc::parsing::NonterminalParser* selectionStatementNonterminalParser = GetNonterminal(ToUtf32("SelectionStatement"));
        selectionStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostSelectionStatement));
        gendoc::parsing::NonterminalParser* iterationStatementNonterminalParser = GetNonterminal(ToUtf32("IterationStatement"));
        iterationStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostIterationStatement));
        gendoc::parsing::NonterminalParser* jumpStatementNonterminalParser = GetNonterminal(ToUtf32("JumpStatement"));
        jumpStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostJumpStatement));
        gendoc::parsing::NonterminalParser* declarationStatementNonterminalParser = GetNonterminal(ToUtf32("DeclarationStatement"));
        declarationStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostDeclarationStatement));
        gendoc::parsing::NonterminalParser* tryStatementNonterminalParser = GetNonterminal(ToUtf32("TryStatement"));
        tryStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostTryStatement));
        gendoc::parsing::NonterminalParser* expressionStatementNonterminalParser = GetNonterminal(ToUtf32("ExpressionStatement"));
        expressionStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostExpressionStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLabeledStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEmptyStatement;
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
        context->value = context->fromTryStatement;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionStatement;
    }
    void PostLabeledStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLabeledStatement_value = std::move(stack.top());
            context->fromLabeledStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromLabeledStatement_value.get());
            stack.pop();
        }
    }
    void PostEmptyStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEmptyStatement_value = std::move(stack.top());
            context->fromEmptyStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromEmptyStatement_value.get());
            stack.pop();
        }
    }
    void PostCompoundStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CompoundStatement*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
    void PostSelectionStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSelectionStatement_value = std::move(stack.top());
            context->fromSelectionStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromSelectionStatement_value.get());
            stack.pop();
        }
    }
    void PostIterationStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIterationStatement_value = std::move(stack.top());
            context->fromIterationStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromIterationStatement_value.get());
            stack.pop();
        }
    }
    void PostJumpStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromJumpStatement_value = std::move(stack.top());
            context->fromJumpStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromJumpStatement_value.get());
            stack.pop();
        }
    }
    void PostDeclarationStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclarationStatement_value = std::move(stack.top());
            context->fromDeclarationStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromDeclarationStatement_value.get());
            stack.pop();
        }
    }
    void PostTryStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTryStatement_value = std::move(stack.top());
            context->fromTryStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TryStatement*>*>(fromTryStatement_value.get());
            stack.pop();
        }
    }
    void PostExpressionStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpressionStatement_value = std::move(stack.top());
            context->fromExpressionStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromExpressionStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromLabeledStatement(), fromEmptyStatement(), fromCompoundStatement(), fromSelectionStatement(), fromIterationStatement(), fromJumpStatement(), fromDeclarationStatement(), fromTryStatement(), fromExpressionStatement() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::Statement* fromLabeledStatement;
        gendoc::codedom::Statement* fromEmptyStatement;
        gendoc::codedom::CompoundStatement* fromCompoundStatement;
        gendoc::codedom::Statement* fromSelectionStatement;
        gendoc::codedom::Statement* fromIterationStatement;
        gendoc::codedom::Statement* fromJumpStatement;
        gendoc::codedom::Statement* fromDeclarationStatement;
        gendoc::codedom::TryStatement* fromTryStatement;
        gendoc::codedom::Statement* fromExpressionStatement;
    };
};

class Statement::LabeledStatementRule : public gendoc::parsing::Rule
{
public:
    LabeledStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
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
        gendoc::parsing::NonterminalParser* labelNonterminalParser = GetNonterminal(ToUtf32("Label"));
        labelNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostLabel));
        gendoc::parsing::NonterminalParser* s1NonterminalParser = GetNonterminal(ToUtf32("s1"));
        s1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Posts1));
        gendoc::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostConstantExpression));
        gendoc::parsing::NonterminalParser* s2NonterminalParser = GetNonterminal(ToUtf32("s2"));
        s2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Posts2));
        gendoc::parsing::NonterminalParser* s3NonterminalParser = GetNonterminal(ToUtf32("s3"));
        s3NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Posts3));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LabeledStatement(context->fromLabel, context->froms1);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CaseStatement(context->fromConstantExpression, context->froms2);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DefaultStatement(context->froms3);
    }
    void PostLabel(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLabel_value = std::move(stack.top());
            context->fromLabel = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromLabel_value.get());
            stack.pop();
        }
    }
    void Posts1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms1_value = std::move(stack.top());
            context->froms1 = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(froms1_value.get());
            stack.pop();
        }
    }
    void PostConstantExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromConstantExpression_value = std::move(stack.top());
            context->fromConstantExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromConstantExpression_value.get());
            stack.pop();
        }
    }
    void Posts2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms2_value = std::move(stack.top());
            context->froms2 = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(froms2_value.get());
            stack.pop();
        }
    }
    void Posts3(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms3_value = std::move(stack.top());
            context->froms3 = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(froms3_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromLabel(), froms1(), fromConstantExpression(), froms2(), froms3() {}
        gendoc::codedom::Statement* value;
        std::u32string fromLabel;
        gendoc::codedom::Statement* froms1;
        gendoc::codedom::CppObject* fromConstantExpression;
        gendoc::codedom::Statement* froms2;
        gendoc::codedom::Statement* froms3;
    };
};

class Statement::LabelRule : public gendoc::parsing::Rule
{
public:
    LabelRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LabelRule>(this, &LabelRule::A0Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LabelRule>(this, &LabelRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdentifier;
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
        Context(): value(), fromIdentifier() {}
        std::u32string value;
        std::u32string fromIdentifier;
    };
};

class Statement::EmptyStatementRule : public gendoc::parsing::Rule
{
public:
    EmptyStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EmptyStatementRule>(this, &EmptyStatementRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EmptyStatement;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::codedom::Statement* value;
    };
};

class Statement::ExpressionStatementRule : public gendoc::parsing::Rule
{
public:
    ExpressionStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpressionStatementRule>(this, &ExpressionStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExpressionStatementRule>(this, &ExpressionStatementRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExpressionStatement(context->fromExpression);
    }
    void PostExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromExpression() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::CppObject* fromExpression;
    };
};

class Statement::CompoundStatementRule : public gendoc::parsing::Rule
{
public:
    CompoundStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::CompoundStatement*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CompoundStatement>"), ToUtf32("cs")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::CompoundStatement*>(context->value)));
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
        statementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CompoundStatementRule>(this, &CompoundStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->cs.reset(new CompoundStatement);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->cs->Add(context->fromStatement);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->cs.release();
    }
    void PostStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), cs(), fromStatement() {}
        gendoc::codedom::CompoundStatement* value;
        std::unique_ptr<CompoundStatement> cs;
        gendoc::codedom::Statement* fromStatement;
    };
};

class Statement::SelectionStatementRule : public gendoc::parsing::Rule
{
public:
    SelectionStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SelectionStatementRule>(this, &SelectionStatementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SelectionStatementRule>(this, &SelectionStatementRule::A1Action));
        gendoc::parsing::NonterminalParser* ifStatementNonterminalParser = GetNonterminal(ToUtf32("IfStatement"));
        ifStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::PostIfStatement));
        gendoc::parsing::NonterminalParser* switchStatementNonterminalParser = GetNonterminal(ToUtf32("SwitchStatement"));
        switchStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::PostSwitchStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIfStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSwitchStatement;
    }
    void PostIfStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIfStatement_value = std::move(stack.top());
            context->fromIfStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromIfStatement_value.get());
            stack.pop();
        }
    }
    void PostSwitchStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSwitchStatement_value = std::move(stack.top());
            context->fromSwitchStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromSwitchStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromIfStatement(), fromSwitchStatement() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::Statement* fromIfStatement;
        gendoc::codedom::Statement* fromSwitchStatement;
    };
};

class Statement::IfStatementRule : public gendoc::parsing::Rule
{
public:
    IfStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IfStatementRule>(this, &IfStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal(ToUtf32("Condition"));
        conditionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IfStatementRule>(this, &IfStatementRule::PostCondition));
        gendoc::parsing::NonterminalParser* thenStatementNonterminalParser = GetNonterminal(ToUtf32("thenStatement"));
        thenStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IfStatementRule>(this, &IfStatementRule::PostthenStatement));
        gendoc::parsing::NonterminalParser* elseStatementNonterminalParser = GetNonterminal(ToUtf32("elseStatement"));
        elseStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IfStatementRule>(this, &IfStatementRule::PostelseStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IfStatement(context->fromCondition, context->fromthenStatement, context->fromelseStatement);
    }
    void PostCondition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCondition_value = std::move(stack.top());
            context->fromCondition = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostthenStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromthenStatement_value = std::move(stack.top());
            context->fromthenStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromthenStatement_value.get());
            stack.pop();
        }
    }
    void PostelseStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromelseStatement_value = std::move(stack.top());
            context->fromelseStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromelseStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromCondition(), fromthenStatement(), fromelseStatement() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::CppObject* fromCondition;
        gendoc::codedom::Statement* fromthenStatement;
        gendoc::codedom::Statement* fromelseStatement;
    };
};

class Statement::SwitchStatementRule : public gendoc::parsing::Rule
{
public:
    SwitchStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SwitchStatementRule>(this, &SwitchStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal(ToUtf32("Condition"));
        conditionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SwitchStatementRule>(this, &SwitchStatementRule::PostCondition));
        gendoc::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SwitchStatementRule>(this, &SwitchStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SwitchStatement(context->fromCondition, context->fromStatement);
    }
    void PostCondition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCondition_value = std::move(stack.top());
            context->fromCondition = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromCondition(), fromStatement() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::CppObject* fromCondition;
        gendoc::codedom::Statement* fromStatement;
    };
};

class Statement::IterationStatementRule : public gendoc::parsing::Rule
{
public:
    IterationStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
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
        gendoc::parsing::NonterminalParser* whileStatementNonterminalParser = GetNonterminal(ToUtf32("WhileStatement"));
        whileStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostWhileStatement));
        gendoc::parsing::NonterminalParser* doStatementNonterminalParser = GetNonterminal(ToUtf32("DoStatement"));
        doStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostDoStatement));
        gendoc::parsing::NonterminalParser* forStatementNonterminalParser = GetNonterminal(ToUtf32("ForStatement"));
        forStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostForStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromWhileStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDoStatement;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromForStatement;
    }
    void PostWhileStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromWhileStatement_value = std::move(stack.top());
            context->fromWhileStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromWhileStatement_value.get());
            stack.pop();
        }
    }
    void PostDoStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDoStatement_value = std::move(stack.top());
            context->fromDoStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromDoStatement_value.get());
            stack.pop();
        }
    }
    void PostForStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromForStatement_value = std::move(stack.top());
            context->fromForStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromForStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromWhileStatement(), fromDoStatement(), fromForStatement() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::Statement* fromWhileStatement;
        gendoc::codedom::Statement* fromDoStatement;
        gendoc::codedom::Statement* fromForStatement;
    };
};

class Statement::WhileStatementRule : public gendoc::parsing::Rule
{
public:
    WhileStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<WhileStatementRule>(this, &WhileStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal(ToUtf32("Condition"));
        conditionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<WhileStatementRule>(this, &WhileStatementRule::PostCondition));
        gendoc::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<WhileStatementRule>(this, &WhileStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new WhileStatement(context->fromCondition, context->fromStatement);
    }
    void PostCondition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCondition_value = std::move(stack.top());
            context->fromCondition = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromCondition(), fromStatement() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::CppObject* fromCondition;
        gendoc::codedom::Statement* fromStatement;
    };
};

class Statement::DoStatementRule : public gendoc::parsing::Rule
{
public:
    DoStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DoStatementRule>(this, &DoStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DoStatementRule>(this, &DoStatementRule::PostStatement));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DoStatementRule>(this, &DoStatementRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DoStatement(context->fromStatement, context->fromExpression);
    }
    void PostStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
    void PostExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromStatement(), fromExpression() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::Statement* fromStatement;
        gendoc::codedom::CppObject* fromExpression;
    };
};

class Statement::ForStatementRule : public gendoc::parsing::Rule
{
public:
    ForStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForStatementRule>(this, &ForStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* forInitStatementNonterminalParser = GetNonterminal(ToUtf32("ForInitStatement"));
        forInitStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostForInitStatement));
        gendoc::parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal(ToUtf32("Condition"));
        conditionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostCondition));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostExpression));
        gendoc::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ForStatement(context->fromForInitStatement, context->fromCondition, context->fromExpression, context->fromStatement);
    }
    void PostForInitStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromForInitStatement_value = std::move(stack.top());
            context->fromForInitStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromForInitStatement_value.get());
            stack.pop();
        }
    }
    void PostCondition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCondition_value = std::move(stack.top());
            context->fromCondition = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromForInitStatement(), fromCondition(), fromExpression(), fromStatement() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::CppObject* fromForInitStatement;
        gendoc::codedom::CppObject* fromCondition;
        gendoc::codedom::CppObject* fromExpression;
        gendoc::codedom::Statement* fromStatement;
    };
};

class Statement::ForInitStatementRule : public gendoc::parsing::Rule
{
public:
    ForInitStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A2Action));
        gendoc::parsing::NonterminalParser* emptyStatementNonterminalParser = GetNonterminal(ToUtf32("EmptyStatement"));
        emptyStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostEmptyStatement));
        gendoc::parsing::NonterminalParser* expressionStatementNonterminalParser = GetNonterminal(ToUtf32("ExpressionStatement"));
        expressionStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostExpressionStatement));
        gendoc::parsing::NonterminalParser* simpleDeclarationNonterminalParser = GetNonterminal(ToUtf32("SimpleDeclaration"));
        simpleDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostSimpleDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEmptyStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionStatement;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleDeclaration;
    }
    void PostEmptyStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEmptyStatement_value = std::move(stack.top());
            context->fromEmptyStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromEmptyStatement_value.get());
            stack.pop();
        }
    }
    void PostExpressionStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpressionStatement_value = std::move(stack.top());
            context->fromExpressionStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromExpressionStatement_value.get());
            stack.pop();
        }
    }
    void PostSimpleDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSimpleDeclaration_value = std::move(stack.top());
            context->fromSimpleDeclaration = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::SimpleDeclaration*>*>(fromSimpleDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromEmptyStatement(), fromExpressionStatement(), fromSimpleDeclaration() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::Statement* fromEmptyStatement;
        gendoc::codedom::Statement* fromExpressionStatement;
        gendoc::codedom::SimpleDeclaration* fromSimpleDeclaration;
    };
};

class Statement::JumpStatementRule : public gendoc::parsing::Rule
{
public:
    JumpStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
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
        gendoc::parsing::NonterminalParser* breakStatementNonterminalParser = GetNonterminal(ToUtf32("BreakStatement"));
        breakStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostBreakStatement));
        gendoc::parsing::NonterminalParser* continueStatementNonterminalParser = GetNonterminal(ToUtf32("ContinueStatement"));
        continueStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostContinueStatement));
        gendoc::parsing::NonterminalParser* returnStatementNonterminalParser = GetNonterminal(ToUtf32("ReturnStatement"));
        returnStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostReturnStatement));
        gendoc::parsing::NonterminalParser* gotoStatementNonterminalParser = GetNonterminal(ToUtf32("GotoStatement"));
        gotoStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostGotoStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBreakStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromContinueStatement;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromReturnStatement;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGotoStatement;
    }
    void PostBreakStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBreakStatement_value = std::move(stack.top());
            context->fromBreakStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromBreakStatement_value.get());
            stack.pop();
        }
    }
    void PostContinueStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromContinueStatement_value = std::move(stack.top());
            context->fromContinueStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromContinueStatement_value.get());
            stack.pop();
        }
    }
    void PostReturnStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromReturnStatement_value = std::move(stack.top());
            context->fromReturnStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromReturnStatement_value.get());
            stack.pop();
        }
    }
    void PostGotoStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromGotoStatement_value = std::move(stack.top());
            context->fromGotoStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Statement*>*>(fromGotoStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromBreakStatement(), fromContinueStatement(), fromReturnStatement(), fromGotoStatement() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::Statement* fromBreakStatement;
        gendoc::codedom::Statement* fromContinueStatement;
        gendoc::codedom::Statement* fromReturnStatement;
        gendoc::codedom::Statement* fromGotoStatement;
    };
};

class Statement::BreakStatementRule : public gendoc::parsing::Rule
{
public:
    BreakStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BreakStatementRule>(this, &BreakStatementRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BreakStatement;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::codedom::Statement* value;
    };
};

class Statement::ContinueStatementRule : public gendoc::parsing::Rule
{
public:
    ContinueStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ContinueStatementRule>(this, &ContinueStatementRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ContinueStatement;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::codedom::Statement* value;
    };
};

class Statement::ReturnStatementRule : public gendoc::parsing::Rule
{
public:
    ReturnStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ReturnStatementRule>(this, &ReturnStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ReturnStatementRule>(this, &ReturnStatementRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ReturnStatement(context->fromExpression);
    }
    void PostExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromExpression() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::CppObject* fromExpression;
    };
};

class Statement::GotoStatementRule : public gendoc::parsing::Rule
{
public:
    GotoStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<GotoStatementRule>(this, &GotoStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* gotoTargetNonterminalParser = GetNonterminal(ToUtf32("GotoTarget"));
        gotoTargetNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GotoStatementRule>(this, &GotoStatementRule::PostGotoTarget));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new GotoStatement(context->fromGotoTarget);
    }
    void PostGotoTarget(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromGotoTarget_value = std::move(stack.top());
            context->fromGotoTarget = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromGotoTarget_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromGotoTarget() {}
        gendoc::codedom::Statement* value;
        std::u32string fromGotoTarget;
    };
};

class Statement::GotoTargetRule : public gendoc::parsing::Rule
{
public:
    GotoTargetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<GotoTargetRule>(this, &GotoTargetRule::A0Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GotoTargetRule>(this, &GotoTargetRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdentifier;
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
        Context(): value(), fromIdentifier() {}
        std::u32string value;
        std::u32string fromIdentifier;
    };
};

class Statement::DeclarationStatementRule : public gendoc::parsing::Rule
{
public:
    DeclarationStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Statement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationStatementRule>(this, &DeclarationStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* blockDeclarationNonterminalParser = GetNonterminal(ToUtf32("BlockDeclaration"));
        blockDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationStatementRule>(this, &DeclarationStatementRule::PostBlockDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DeclarationStatement(context->fromBlockDeclaration);
    }
    void PostBlockDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBlockDeclaration_value = std::move(stack.top());
            context->fromBlockDeclaration = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromBlockDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromBlockDeclaration() {}
        gendoc::codedom::Statement* value;
        gendoc::codedom::CppObject* fromBlockDeclaration;
    };
};

class Statement::ConditionRule : public gendoc::parsing::Rule
{
public:
    ConditionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<TypeId>"), ToUtf32("ti")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>(context->value)));
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
        gendoc::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostTypeId));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostDeclarator));
        gendoc::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostAssignmentExpression));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti.reset(context->fromTypeId);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConditionWithDeclarator(context->ti.release(), context->fromDeclarator, context->fromAssignmentExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpression;
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
    void PostAssignmentExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
    void PostExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), ti(), fromTypeId(), fromDeclarator(), fromAssignmentExpression(), fromExpression() {}
        gendoc::codedom::CppObject* value;
        std::unique_ptr<TypeId> ti;
        gendoc::codedom::TypeId* fromTypeId;
        std::u32string fromDeclarator;
        gendoc::codedom::CppObject* fromAssignmentExpression;
        gendoc::codedom::CppObject* fromExpression;
    };
};

class Statement::TryStatementRule : public gendoc::parsing::Rule
{
public:
    TryStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::TryStatement*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::TryStatement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TryStatementRule>(this, &TryStatementRule::A0Action));
        gendoc::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TryStatementRule>(this, &TryStatementRule::PostCompoundStatement));
        gendoc::parsing::NonterminalParser* handlerSeqNonterminalParser = GetNonterminal(ToUtf32("HandlerSeq"));
        handlerSeqNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TryStatementRule>(this, &TryStatementRule::PreHandlerSeq));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TryStatement(context->fromCompoundStatement);
    }
    void PostCompoundStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CompoundStatement*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
    void PreHandlerSeq(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<TryStatement*>(context->value)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromCompoundStatement() {}
        gendoc::codedom::TryStatement* value;
        gendoc::codedom::CompoundStatement* fromCompoundStatement;
    };
};

class Statement::HandlerSeqRule : public gendoc::parsing::Rule
{
public:
    HandlerSeqRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("TryStatement*"), ToUtf32("st")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> st_value = std::move(stack.top());
        context->st = *static_cast<gendoc::parsing::ValueObject<TryStatement*>*>(st_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<HandlerSeqRule>(this, &HandlerSeqRule::A0Action));
        gendoc::parsing::NonterminalParser* handlerNonterminalParser = GetNonterminal(ToUtf32("Handler"));
        handlerNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<HandlerSeqRule>(this, &HandlerSeqRule::PostHandler));
        gendoc::parsing::NonterminalParser* handlerSeqNonterminalParser = GetNonterminal(ToUtf32("HandlerSeq"));
        handlerSeqNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<HandlerSeqRule>(this, &HandlerSeqRule::PreHandlerSeq));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->st->Add(context->fromHandler);
    }
    void PostHandler(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromHandler_value = std::move(stack.top());
            context->fromHandler = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Handler*>*>(fromHandler_value.get());
            stack.pop();
        }
    }
    void PreHandlerSeq(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<TryStatement*>(context->st)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): st(), fromHandler() {}
        TryStatement* st;
        gendoc::codedom::Handler* fromHandler;
    };
};

class Statement::HandlerRule : public gendoc::parsing::Rule
{
public:
    HandlerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Handler*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Handler*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<HandlerRule>(this, &HandlerRule::A0Action));
        gendoc::parsing::NonterminalParser* exceptionDeclarationNonterminalParser = GetNonterminal(ToUtf32("ExceptionDeclaration"));
        exceptionDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostExceptionDeclaration));
        gendoc::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostCompoundStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Handler(context->fromExceptionDeclaration, context->fromCompoundStatement);
    }
    void PostExceptionDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExceptionDeclaration_value = std::move(stack.top());
            context->fromExceptionDeclaration = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::ExceptionDeclaration*>*>(fromExceptionDeclaration_value.get());
            stack.pop();
        }
    }
    void PostCompoundStatement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CompoundStatement*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromExceptionDeclaration(), fromCompoundStatement() {}
        gendoc::codedom::Handler* value;
        gendoc::codedom::ExceptionDeclaration* fromExceptionDeclaration;
        gendoc::codedom::CompoundStatement* fromCompoundStatement;
    };
};

class Statement::ExceptionDeclarationRule : public gendoc::parsing::Rule
{
public:
    ExceptionDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::ExceptionDeclaration*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<ExceptionDeclaration>"), ToUtf32("ed")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::ExceptionDeclaration*>(context->value)));
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
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A4Action));
        gendoc::parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierSeq"));
        typeSpecifierSeqNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PreTypeSpecifierSeq));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PostDeclarator));
        gendoc::parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("AbstractDeclarator"));
        abstractDeclaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PostAbstractDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ed.reset(new ExceptionDeclaration);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ed.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ed->GetTypeId()->Declarator() = context->fromDeclarator;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ed->GetTypeId()->Declarator() = context->fromAbstractDeclarator;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->CatchAll() = true;
    }
    void PreTypeSpecifierSeq(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>(context->ed->GetTypeId())));
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
    void PostAbstractDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAbstractDeclarator_value = std::move(stack.top());
            context->fromAbstractDeclarator = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromAbstractDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), ed(), fromDeclarator(), fromAbstractDeclarator() {}
        gendoc::codedom::ExceptionDeclaration* value;
        std::unique_ptr<ExceptionDeclaration> ed;
        std::u32string fromDeclarator;
        std::u32string fromAbstractDeclarator;
    };
};

void Statement::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.code.Identifier"));
    if (!grammar0)
    {
        grammar0 = gendoc::code::Identifier::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.code.Expression"));
    if (!grammar1)
    {
        grammar1 = gendoc::code::Expression::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.code.Declarator"));
    if (!grammar2)
    {
        grammar2 = gendoc::code::Declarator::Create(pd);
    }
    AddGrammarReference(grammar2);
    gendoc::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("gendoc.code.Declaration"));
    if (!grammar3)
    {
        grammar3 = gendoc::code::Declaration::Create(pd);
    }
    AddGrammarReference(grammar3);
    gendoc::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar4)
    {
        grammar4 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void Statement::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Expression"), this, ToUtf32("Expression.Expression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("AssignmentExpression"), this, ToUtf32("Expression.AssignmentExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ConstantExpression"), this, ToUtf32("Expression.ConstantExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeId"), this, ToUtf32("Declarator.TypeId")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("AbstractDeclarator"), this, ToUtf32("Declarator.AbstractDeclarator")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Declarator"), this, ToUtf32("Declarator.Declarator")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeSpecifierSeq"), this, ToUtf32("Declarator.TypeSpecifierSeq")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("SimpleDeclaration"), this, ToUtf32("Declaration.SimpleDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("BlockDeclaration"), this, ToUtf32("Declaration.BlockDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("gendoc.parsing.stdlib.spaces_and_comments")));
    AddRule(new StatementRule(ToUtf32("Statement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("LabeledStatement"), ToUtf32("LabeledStatement"), 0)),
                                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("EmptyStatement"), ToUtf32("EmptyStatement"), 0))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 0))),
                                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("SelectionStatement"), ToUtf32("SelectionStatement"), 0))),
                            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("IterationStatement"), ToUtf32("IterationStatement"), 0))),
                        new gendoc::parsing::ActionParser(ToUtf32("A5"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("JumpStatement"), ToUtf32("JumpStatement"), 0))),
                    new gendoc::parsing::ActionParser(ToUtf32("A6"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("DeclarationStatement"), ToUtf32("DeclarationStatement"), 0))),
                new gendoc::parsing::ActionParser(ToUtf32("A7"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("TryStatement"), ToUtf32("TryStatement"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A8"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionStatement"), ToUtf32("ExpressionStatement"), 0)))));
    AddRule(new LabeledStatementRule(ToUtf32("LabeledStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("Label"), ToUtf32("Label"), 0),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::DifferenceParser(
                                        new gendoc::parsing::CharParser(':'),
                                        new gendoc::parsing::StringParser(ToUtf32("::"))))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("Statement"), 0))))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("case")),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 0))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser(':'))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("s2"), ToUtf32("Statement"), 0)))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("default")),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::CharParser(':'))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("s3"), ToUtf32("Statement"), 0))))))));
    AddRule(new LabelRule(ToUtf32("Label"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))));
    AddRule(new EmptyStatementRule(ToUtf32("EmptyStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::CharParser(';'))));
    AddRule(new ExpressionStatementRule(ToUtf32("ExpressionStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser(';')))))));
    AddRule(new CompoundStatementRule(ToUtf32("CompoundStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::CharParser('{')),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser('}'))))));
    AddRule(new SelectionStatementRule(ToUtf32("SelectionStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("IfStatement"), ToUtf32("IfStatement"), 0)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("SwitchStatement"), ToUtf32("SwitchStatement"), 0)))));
    AddRule(new IfStatementRule(ToUtf32("IfStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                    new gendoc::parsing::NonterminalParser(ToUtf32("Condition"), ToUtf32("Condition"), 0))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::CharParser(')'))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("thenStatement"), ToUtf32("Statement"), 0))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("else")),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("elseStatement"), ToUtf32("Statement"), 0))))))))));
    AddRule(new SwitchStatementRule(ToUtf32("SwitchStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("switch")),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser('('))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Condition"), ToUtf32("Condition"), 0))),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(')'))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))))));
    AddRule(new IterationStatementRule(ToUtf32("IterationStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("WhileStatement"), ToUtf32("WhileStatement"), 0)),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("DoStatement"), ToUtf32("DoStatement"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ForStatement"), ToUtf32("ForStatement"), 0)))));
    AddRule(new WhileStatementRule(ToUtf32("WhileStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("while")),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser('('))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Condition"), ToUtf32("Condition"), 0))),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(')'))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))))));
    AddRule(new DoStatementRule(ToUtf32("DoStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("do")),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("while")))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser('('))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(')'))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new ForStatementRule(ToUtf32("ForStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("for")),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser('('))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("ForInitStatement"), ToUtf32("ForInitStatement"), 0))),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("Condition"), ToUtf32("Condition"), 0))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser(';'))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(')'))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))))));
    AddRule(new ForInitStatementRule(ToUtf32("ForInitStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("EmptyStatement"), ToUtf32("EmptyStatement"), 0)),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionStatement"), ToUtf32("ExpressionStatement"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("SimpleDeclaration"), ToUtf32("SimpleDeclaration"), 0)))));
    AddRule(new JumpStatementRule(ToUtf32("JumpStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("BreakStatement"), ToUtf32("BreakStatement"), 0)),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("ContinueStatement"), ToUtf32("ContinueStatement"), 0))),
                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ReturnStatement"), ToUtf32("ReturnStatement"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                new gendoc::parsing::NonterminalParser(ToUtf32("GotoStatement"), ToUtf32("GotoStatement"), 0)))));
    AddRule(new BreakStatementRule(ToUtf32("BreakStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::KeywordParser(ToUtf32("break")),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new ContinueStatementRule(ToUtf32("ContinueStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::KeywordParser(ToUtf32("continue")),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new ReturnStatementRule(ToUtf32("ReturnStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::KeywordParser(ToUtf32("return")),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new GotoStatementRule(ToUtf32("GotoStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::KeywordParser(ToUtf32("goto")),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("GotoTarget"), ToUtf32("GotoTarget"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new GotoTargetRule(ToUtf32("GotoTarget"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))));
    AddRule(new DeclarationStatementRule(ToUtf32("DeclarationStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("BlockDeclaration"), ToUtf32("BlockDeclaration"), 0))));
    AddRule(new ConditionRule(ToUtf32("Condition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)),
                    new gendoc::parsing::CharParser('=')),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0)))));
    AddRule(new TryStatementRule(ToUtf32("TryStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::KeywordParser(ToUtf32("try")),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 0)))),
            new gendoc::parsing::ExpectationParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("HandlerSeq"), ToUtf32("HandlerSeq"), 1)))));
    AddRule(new HandlerSeqRule(ToUtf32("HandlerSeq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Handler"), ToUtf32("Handler"), 0))),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("HandlerSeq"), ToUtf32("HandlerSeq"), 1)))));
    AddRule(new HandlerRule(ToUtf32("Handler"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("catch")),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser('('))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("ExceptionDeclaration"), ToUtf32("ExceptionDeclaration"), 0))),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(')'))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 0))))));
    AddRule(new ExceptionDeclarationRule(ToUtf32("ExceptionDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::EmptyParser()),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("TypeSpecifierSeq"), ToUtf32("TypeSpecifierSeq"), 1),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)),
                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("AbstractDeclarator"), ToUtf32("AbstractDeclarator"), 0))),
                                    new gendoc::parsing::EmptyParser()))),
                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                            new gendoc::parsing::StringParser(ToUtf32("...")))))))));
    SetStartRuleName(ToUtf32("CompoundStatement"));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace gendoc.code
