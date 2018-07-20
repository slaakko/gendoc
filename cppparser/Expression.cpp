#include "Expression.hpp"
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
#include <gendoc/cppparser/Literal.hpp>
#include <gendoc/cppparser/Template.hpp>
#include <gendoc/cppparser/TypeExpr.hpp>
#include <gendoc/cppparser/Declarator.hpp>
#include <gendoc/cppparser/Identifier.hpp>
#include <gendoc/cppparser/SimpleType.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Expression* Expression::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Expression* Expression::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Expression* grammar(new Expression(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Expression::Expression(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Expression"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

Node* Expression::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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

class Expression::ExpressionListRule : public gendoc::parsing::Rule
{
public:
    ExpressionListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpressionListRule>(this, &ExpressionListRule::A0Action));
        gendoc::parsing::NonterminalParser* initializerListNonterminalParser = GetNonterminal(ToUtf32("InitializerList"));
        initializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExpressionListRule>(this, &ExpressionListRule::PreInitializerList));
        initializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExpressionListRule>(this, &ExpressionListRule::PostInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromInitializerList;
    }
    void PreInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromInitializerList_value = std::move(stack.top());
            context->fromInitializerList = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromInitializerList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromInitializerList() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromInitializerList;
    };
};

class Expression::PossiblyEmptyArgumentListRule : public gendoc::parsing::Rule
{
public:
    PossiblyEmptyArgumentListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PossiblyEmptyArgumentListRule>(this, &PossiblyEmptyArgumentListRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PossiblyEmptyArgumentListRule>(this, &PossiblyEmptyArgumentListRule::A1Action));
        gendoc::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PossiblyEmptyArgumentListRule>(this, &PossiblyEmptyArgumentListRule::PreExpressionList));
        expressionListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PossiblyEmptyArgumentListRule>(this, &PossiblyEmptyArgumentListRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionList;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = nullptr;
    }
    void PreExpressionList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpressionList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromExpressionList() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExpressionList;
    };
};

class Expression::ExpressionRule : public gendoc::parsing::Rule
{
public:
    ExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExpressionRule>(this, &ExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExpressionRule>(this, &ExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new CommaExpressionNode(context->s, context->expr.release(), context->fromright));
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::ConstantExpressionRule : public gendoc::parsing::Rule
{
public:
    ConstantExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConstantExpressionRule>(this, &ConstantExpressionRule::A0Action));
        gendoc::parsing::NonterminalParser* conditionalExpressionNonterminalParser = GetNonterminal(ToUtf32("ConditionalExpression"));
        conditionalExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConstantExpressionRule>(this, &ConstantExpressionRule::PreConditionalExpression));
        conditionalExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConstantExpressionRule>(this, &ConstantExpressionRule::PostConditionalExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConditionalExpression;
    }
    void PreConditionalExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConditionalExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromConditionalExpression_value = std::move(stack.top());
            context->fromConditionalExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromConditionalExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromConditionalExpression() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromConditionalExpression;
    };
};

class Expression::AssignmentExpressionRule : public gendoc::parsing::Rule
{
public:
    AssignmentExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* concreteAssignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("ConcreteAssignmentExpression"));
        concreteAssignmentExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PreConcreteAssignmentExpression));
        concreteAssignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostConcreteAssignmentExpression));
        gendoc::parsing::NonterminalParser* conditionalExpressionNonterminalParser = GetNonterminal(ToUtf32("ConditionalExpression"));
        conditionalExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PreConditionalExpression));
        conditionalExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostConditionalExpression));
        gendoc::parsing::NonterminalParser* throwExpressionNonterminalParser = GetNonterminal(ToUtf32("ThrowExpression"));
        throwExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PreThrowExpression));
        throwExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostThrowExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConcreteAssignmentExpression;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConditionalExpression;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromThrowExpression;
    }
    void PreConcreteAssignmentExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConcreteAssignmentExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromConcreteAssignmentExpression_value = std::move(stack.top());
            context->fromConcreteAssignmentExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromConcreteAssignmentExpression_value.get());
            stack.pop();
        }
    }
    void PreConditionalExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConditionalExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromConditionalExpression_value = std::move(stack.top());
            context->fromConditionalExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromConditionalExpression_value.get());
            stack.pop();
        }
    }
    void PreThrowExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostThrowExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromThrowExpression_value = std::move(stack.top());
            context->fromThrowExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromThrowExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromConcreteAssignmentExpression(), fromConditionalExpression(), fromThrowExpression() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromConcreteAssignmentExpression;
        Node* fromConditionalExpression;
        Node* fromThrowExpression;
    };
};

class Expression::ConcreteAssignmentExpressionRule : public gendoc::parsing::Rule
{
public:
    ConcreteAssignmentExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("logicalOrExpr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConcreteAssignmentExpressionRule>(this, &ConcreteAssignmentExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConcreteAssignmentExpressionRule>(this, &ConcreteAssignmentExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConcreteAssignmentExpressionRule>(this, &ConcreteAssignmentExpressionRule::A2Action));
        a2ActionParser->SetFailureAction(new gendoc::parsing::MemberFailureAction<ConcreteAssignmentExpressionRule>(this, &ConcreteAssignmentExpressionRule::A2ActionFail));
        gendoc::parsing::NonterminalParser* logicalOrExpressionNonterminalParser = GetNonterminal(ToUtf32("LogicalOrExpression"));
        logicalOrExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConcreteAssignmentExpressionRule>(this, &ConcreteAssignmentExpressionRule::PreLogicalOrExpression));
        logicalOrExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConcreteAssignmentExpressionRule>(this, &ConcreteAssignmentExpressionRule::PostLogicalOrExpression));
        gendoc::parsing::NonterminalParser* assignmentOperatorNonterminalParser = GetNonterminal(ToUtf32("AssignmentOperator"));
        assignmentOperatorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConcreteAssignmentExpressionRule>(this, &ConcreteAssignmentExpressionRule::PostAssignmentOperator));
        gendoc::parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal(ToUtf32("InitializerClause"));
        initializerClauseNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConcreteAssignmentExpressionRule>(this, &ConcreteAssignmentExpressionRule::PreInitializerClause));
        initializerClauseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConcreteAssignmentExpressionRule>(this, &ConcreteAssignmentExpressionRule::PostInitializerClause));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->BeginParsingLvalue();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AssignmentExpressionNode(span, context->logicalOrExpr.release(), context->fromAssignmentOperator, context->fromInitializerClause);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingLvalue();
        context->logicalOrExpr.reset(context->fromLogicalOrExpression);
    }
    void A2ActionFail(ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingLvalue();
    }
    void PreLogicalOrExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostLogicalOrExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLogicalOrExpression_value = std::move(stack.top());
            context->fromLogicalOrExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromLogicalOrExpression_value.get());
            stack.pop();
        }
    }
    void PostAssignmentOperator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAssignmentOperator_value = std::move(stack.top());
            context->fromAssignmentOperator = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromAssignmentOperator_value.get());
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), logicalOrExpr(), fromLogicalOrExpression(), fromAssignmentOperator(), fromInitializerClause() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> logicalOrExpr;
        Node* fromLogicalOrExpression;
        Operator fromAssignmentOperator;
        Node* fromInitializerClause;
    };
};

class Expression::AssignmentOperatorRule : public gendoc::parsing::Rule
{
public:
    AssignmentOperatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A9Action));
        gendoc::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentOperatorRule>(this, &AssignmentOperatorRule::A10Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::assign;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::mulAssign;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::divAssign;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::remAssign;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::addAssign;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::subAssign;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftRightAssign;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftLeftAssign;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::andAssign;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::xorAssign;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::orAssign;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::ConditionalExpressionRule : public gendoc::parsing::Rule
{
public:
    ConditionalExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("qe")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A3Action));
        gendoc::parsing::NonterminalParser* logicalOrExpressionNonterminalParser = GetNonterminal(ToUtf32("LogicalOrExpression"));
        logicalOrExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PreLogicalOrExpression));
        logicalOrExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostLogicalOrExpression));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostExpression));
        gendoc::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PreAssignmentExpression));
        assignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromLogicalOrExpression);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new ConditionalExpressionNode(context->s, context->expr.release(), context->qe.release(), context->fromAssignmentExpression));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->qe.reset(context->fromExpression);
    }
    void PreLogicalOrExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostLogicalOrExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLogicalOrExpression_value = std::move(stack.top());
            context->fromLogicalOrExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromLogicalOrExpression_value.get());
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
    void PreAssignmentExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostAssignmentExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), qe(), s(), fromLogicalOrExpression(), fromExpression(), fromAssignmentExpression() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        std::unique_ptr<Node> qe;
        Span s;
        Node* fromLogicalOrExpression;
        Node* fromExpression;
        Node* fromAssignmentExpression;
    };
};

class Expression::ThrowExpressionRule : public gendoc::parsing::Rule
{
public:
    ThrowExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ThrowExpressionRule>(this, &ThrowExpressionRule::A0Action));
        gendoc::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ThrowExpressionRule>(this, &ThrowExpressionRule::PreAssignmentExpression));
        assignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ThrowExpressionRule>(this, &ThrowExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ThrowExpressionNode(span, context->fromAssignmentExpression);
    }
    void PreAssignmentExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostAssignmentExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromAssignmentExpression() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromAssignmentExpression;
    };
};

class Expression::LogicalOrExpressionRule : public gendoc::parsing::Rule
{
public:
    LogicalOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingTemplateIdOrLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new LogicalOrExpressionNode(context->s, context->expr.release(), context->fromright));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::LogicalAndExpressionRule : public gendoc::parsing::Rule
{
public:
    LogicalAndExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingTemplateIdOrLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new LogicalAndExpressionNode(context->s, context->expr.release(), context->fromright));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::InclusiveOrExpressionRule : public gendoc::parsing::Rule
{
public:
    InclusiveOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new InclusiveOrExpressionNode(context->s, context->expr.release(), context->fromright));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::ExclusiveOrExpressionRule : public gendoc::parsing::Rule
{
public:
    ExclusiveOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new ExclusiveOrExpressionNode(context->s, context->expr.release(), context->fromright));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::AndExpressionRule : public gendoc::parsing::Rule
{
public:
    AndExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AndExpressionRule>(this, &AndExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AndExpressionRule>(this, &AndExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new AndExpressionNode(context->s, context->expr.release(), context->fromright));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::EqualityExpressionRule : public gendoc::parsing::Rule
{
public:
    EqualityExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A4Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::equal;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::notEqual;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new EqualityExpressionNode(context->s, context->expr.release(), context->fromright, context->op));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::RelationalExpressionRule : public gendoc::parsing::Rule
{
public:
    RelationalExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A6Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::lessOrEqual;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::greaterOrEqual;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::less;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::greater;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingTemplateIdOrLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new RelationalExpressionNode(context->s, context->expr.release(), context->fromright, context->op));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::ShiftExpressionRule : public gendoc::parsing::Rule
{
public:
    ShiftExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A4Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::shiftLeft;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::shiftRight;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingTemplateIdOrLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new ShiftExpressionNode(context->s, context->expr.release(), context->fromright, context->op));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::AdditiveExpressionRule : public gendoc::parsing::Rule
{
public:
    AdditiveExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A4Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::add;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::sub;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new AdditiveExpressionNode(context->s, context->expr.release(), context->fromright, context->op));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::MultiplicativeExpressionRule : public gendoc::parsing::Rule
{
public:
    MultiplicativeExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A5Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::mul;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::div;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::rem;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalueAndNotArguments()) pass = false;
        else
        {
            context->s.SetEnd(span.End());
            context->expr.reset(new MultiplicativeExpressionNode(context->s, context->expr.release(), context->fromright, context->op));
        }
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::PMExpressionRule : public gendoc::parsing::Rule
{
public:
    PMExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PMExpressionRule>(this, &PMExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PMExpressionRule>(this, &PMExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PMExpressionRule>(this, &PMExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PMExpressionRule>(this, &PMExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PMExpressionRule>(this, &PMExpressionRule::A4Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PMExpressionRule>(this, &PMExpressionRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PMExpressionRule>(this, &PMExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PMExpressionRule>(this, &PMExpressionRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PMExpressionRule>(this, &PMExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::dotStar;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::arrowStar;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new PMExpressionNode(context->s, context->expr.release(), context->fromright, context->op));
    }
    void Preleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::CastExpressionRule : public gendoc::parsing::Rule
{
public:
    CastExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<CastExpressionRule>(this, &CastExpressionRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostTypeExpr));
        gendoc::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<CastExpressionRule>(this, &CastExpressionRule::PreCastExpression));
        castExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostCastExpression));
        gendoc::parsing::NonterminalParser* unaryExpressionNonterminalParser = GetNonterminal(ToUtf32("UnaryExpression"));
        unaryExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<CastExpressionRule>(this, &CastExpressionRule::PreUnaryExpression));
        unaryExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostUnaryExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CastExpressionNode(span, context->expr.release(), context->fromCastExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromTypeExpr);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUnaryExpression;
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
    void PreCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
    void PreUnaryExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostUnaryExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUnaryExpression_value = std::move(stack.top());
            context->fromUnaryExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromUnaryExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), fromTypeExpr(), fromCastExpression(), fromUnaryExpression() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Node* fromTypeExpr;
        Node* fromCastExpression;
        Node* fromUnaryExpression;
    };
};

class Expression::UnaryExpressionRule : public gendoc::parsing::Rule
{
public:
    UnaryExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A7Action));
        gendoc::parsing::NonterminalParser* c1NonterminalParser = GetNonterminal(ToUtf32("c1"));
        c1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Prec1));
        c1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Postc1));
        gendoc::parsing::NonterminalParser* c2NonterminalParser = GetNonterminal(ToUtf32("c2"));
        c2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Prec2));
        c2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Postc2));
        gendoc::parsing::NonterminalParser* unaryOperatorNonterminalParser = GetNonterminal(ToUtf32("UnaryOperator"));
        unaryOperatorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostUnaryOperator));
        gendoc::parsing::NonterminalParser* c3NonterminalParser = GetNonterminal(ToUtf32("c3"));
        c3NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Prec3));
        c3NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Postc3));
        gendoc::parsing::NonterminalParser* u4NonterminalParser = GetNonterminal(ToUtf32("u4"));
        u4NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Preu4));
        u4NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Postu4));
        gendoc::parsing::NonterminalParser* t5NonterminalParser = GetNonterminal(ToUtf32("t5"));
        t5NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Pret5));
        t5NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Postt5));
        gendoc::parsing::NonterminalParser* newExpressionNonterminalParser = GetNonterminal(ToUtf32("NewExpression"));
        newExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PreNewExpression));
        newExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostNewExpression));
        gendoc::parsing::NonterminalParser* deleteExpressionNonterminalParser = GetNonterminal(ToUtf32("DeleteExpression"));
        deleteExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PreDeleteExpression));
        deleteExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostDeleteExpression));
        gendoc::parsing::NonterminalParser* postfixExpressionNonterminalParser = GetNonterminal(ToUtf32("PostfixExpression"));
        postfixExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PrePostfixExpression));
        postfixExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostPostfixExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UnaryExpressionNode(span, Operator::inc, context->fromc1);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UnaryExpressionNode(span, Operator::dec, context->fromc2);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UnaryExpressionNode(span, context->fromUnaryOperator, context->fromc3);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UnaryExpressionNode(span, Operator::sizeOf, context->fromu4);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UnaryExpressionNode(span, Operator::alignOf, context->fromt5);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNewExpression;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDeleteExpression;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPostfixExpression;
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
    void PostUnaryOperator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUnaryOperator_value = std::move(stack.top());
            context->fromUnaryOperator = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromUnaryOperator_value.get());
            stack.pop();
        }
    }
    void Prec3(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postc3(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromc3_value = std::move(stack.top());
            context->fromc3 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromc3_value.get());
            stack.pop();
        }
    }
    void Preu4(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postu4(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromu4_value = std::move(stack.top());
            context->fromu4 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromu4_value.get());
            stack.pop();
        }
    }
    void Pret5(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postt5(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromt5_value = std::move(stack.top());
            context->fromt5 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromt5_value.get());
            stack.pop();
        }
    }
    void PreNewExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostNewExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNewExpression_value = std::move(stack.top());
            context->fromNewExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromNewExpression_value.get());
            stack.pop();
        }
    }
    void PreDeleteExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDeleteExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeleteExpression_value = std::move(stack.top());
            context->fromDeleteExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromDeleteExpression_value.get());
            stack.pop();
        }
    }
    void PrePostfixExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPostfixExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPostfixExpression_value = std::move(stack.top());
            context->fromPostfixExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromPostfixExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromc1(), fromc2(), fromUnaryOperator(), fromc3(), fromu4(), fromt5(), fromNewExpression(), fromDeleteExpression(), fromPostfixExpression() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromc1;
        Node* fromc2;
        Operator fromUnaryOperator;
        Node* fromc3;
        Node* fromu4;
        Node* fromt5;
        Node* fromNewExpression;
        Node* fromDeleteExpression;
        Node* fromPostfixExpression;
    };
};

class Expression::UnaryOperatorRule : public gendoc::parsing::Rule
{
public:
    UnaryOperatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A5Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::deref;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::addrOf;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::unaryPlus;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::unaryMinus;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::not_;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::complement;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::NewExpressionRule : public gendoc::parsing::Rule
{
public:
    NewExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A0Action));
        gendoc::parsing::NonterminalParser* newPlacementNonterminalParser = GetNonterminal(ToUtf32("NewPlacement"));
        newPlacementNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NewExpressionRule>(this, &NewExpressionRule::PreNewPlacement));
        newPlacementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewPlacement));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NewExpressionRule>(this, &NewExpressionRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostTypeExpr));
        gendoc::parsing::NonterminalParser* newInitializerNonterminalParser = GetNonterminal(ToUtf32("NewInitializer"));
        newInitializerNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NewExpressionRule>(this, &NewExpressionRule::PreNewInitializer));
        newInitializerNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewInitializer));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NewExpressionNode(span, context->fromNewPlacement, context->fromTypeExpr, context->fromNewInitializer);
    }
    void PreNewPlacement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostNewPlacement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNewPlacement_value = std::move(stack.top());
            context->fromNewPlacement = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromNewPlacement_value.get());
            stack.pop();
        }
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
    void PreNewInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostNewInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNewInitializer_value = std::move(stack.top());
            context->fromNewInitializer = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromNewInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromNewPlacement(), fromTypeExpr(), fromNewInitializer() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromNewPlacement;
        Node* fromTypeExpr;
        Node* fromNewInitializer;
    };
};

class Expression::NewPlacementRule : public gendoc::parsing::Rule
{
public:
    NewPlacementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewPlacementRule>(this, &NewPlacementRule::A0Action));
        gendoc::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NewPlacementRule>(this, &NewPlacementRule::PreExpressionList));
        expressionListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewPlacementRule>(this, &NewPlacementRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionList;
    }
    void PreExpressionList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpressionList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromExpressionList() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExpressionList;
    };
};

class Expression::NewInitializerRule : public gendoc::parsing::Rule
{
public:
    NewInitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewInitializerRule>(this, &NewInitializerRule::A0Action));
        gendoc::parsing::NonterminalParser* argsNonterminalParser = GetNonterminal(ToUtf32("args"));
        argsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NewInitializerRule>(this, &NewInitializerRule::Preargs));
        argsNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewInitializerRule>(this, &NewInitializerRule::Postargs));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromargs;
    }
    void Preargs(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postargs(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromargs_value = std::move(stack.top());
            context->fromargs = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromargs_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromargs() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromargs;
    };
};

class Expression::DeleteExpressionRule : public gendoc::parsing::Rule
{
public:
    DeleteExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("array_")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeleteExpressionRule>(this, &DeleteExpressionRule::PreCastExpression));
        castExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeleteExpressionRule>(this, &DeleteExpressionRule::PostCastExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DeleteExpressionNode(span, context->array_, context->fromCastExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->array_ = true;
    }
    void PreCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), array_(), fromCastExpression() {}
        ParsingContext* ctx;
        Node* value;
        bool array_;
        Node* fromCastExpression;
    };
};

class Expression::PostfixExpressionRule : public gendoc::parsing::Rule
{
public:
    PostfixExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("args")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A3Action));
        a3ActionParser->SetFailureAction(new gendoc::parsing::MemberFailureAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A3ActionFail));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A6Action));
        a6ActionParser->SetFailureAction(new gendoc::parsing::MemberFailureAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A6ActionFail));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A9Action));
        gendoc::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A10Action));
        gendoc::parsing::NonterminalParser* primaryExpressionNonterminalParser = GetNonterminal(ToUtf32("PrimaryExpression"));
        primaryExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PrePrimaryExpression));
        primaryExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PostPrimaryExpression));
        gendoc::parsing::NonterminalParser* indexNonterminalParser = GetNonterminal(ToUtf32("index"));
        indexNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Preindex));
        indexNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postindex));
        gendoc::parsing::NonterminalParser* argumentsNonterminalParser = GetNonterminal(ToUtf32("arguments"));
        argumentsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Prearguments));
        argumentsNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postarguments));
        gendoc::parsing::NonterminalParser* id1NonterminalParser = GetNonterminal(ToUtf32("id1"));
        id1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Preid1));
        id1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postid1));
        gendoc::parsing::NonterminalParser* id2NonterminalParser = GetNonterminal(ToUtf32("id2"));
        id2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Preid2));
        id2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postid2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromPrimaryExpression);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->BeginParsingArguments();
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingArguments();
        context->s.SetEnd(span.End());
        context->expr.reset(new SubscriptExpressionNode(context->s, context->expr.release(), context->fromindex));
    }
    void A3ActionFail(ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingArguments();
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->BeginParsingArguments();
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->args.reset(context->fromarguments);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingArguments();
        context->s.SetEnd(span.End());
        context->expr.reset(new InvokeExpressionNode(context->s, context->expr.release(), context->args.release()));
    }
    void A6ActionFail(ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingArguments();
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new DotNode(context->s, context->expr.release(), context->fromid1));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new ArrowNode(context->s, context->expr.release(), context->fromid2));
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new PostfixIncNode(context->s, context->expr.release()));
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new PostfixDecNode(context->s, context->expr.release()));
    }
    void PrePrimaryExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPrimaryExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPrimaryExpression_value = std::move(stack.top());
            context->fromPrimaryExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromPrimaryExpression_value.get());
            stack.pop();
        }
    }
    void Preindex(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postindex(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromindex_value = std::move(stack.top());
            context->fromindex = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromindex_value.get());
            stack.pop();
        }
    }
    void Prearguments(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postarguments(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromarguments_value = std::move(stack.top());
            context->fromarguments = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromarguments_value.get());
            stack.pop();
        }
    }
    void Preid1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postid1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromid1_value = std::move(stack.top());
            context->fromid1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromid1_value.get());
            stack.pop();
        }
    }
    void Preid2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postid2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromid2_value = std::move(stack.top());
            context->fromid2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromid2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), expr(), args(), s(), fromPrimaryExpression(), fromindex(), fromarguments(), fromid1(), fromid2() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        std::unique_ptr<Node> args;
        Span s;
        Node* fromPrimaryExpression;
        Node* fromindex;
        Node* fromarguments;
        Node* fromid1;
        Node* fromid2;
    };
};

class Expression::PrimaryExpressionRule : public gendoc::parsing::Rule
{
public:
    PrimaryExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A9Action));
        gendoc::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A10Action));
        gendoc::parsing::NonterminalParser* literalNonterminalParser = GetNonterminal(ToUtf32("Literal"));
        literalNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostLiteral));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostExpression));
        gendoc::parsing::NonterminalParser* cppCastExpressionNonterminalParser = GetNonterminal(ToUtf32("CppCastExpression"));
        cppCastExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PreCppCastExpression));
        cppCastExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostCppCastExpression));
        gendoc::parsing::NonterminalParser* typeIdExpressionNonterminalParser = GetNonterminal(ToUtf32("TypeIdExpression"));
        typeIdExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PreTypeIdExpression));
        typeIdExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostTypeIdExpression));
        gendoc::parsing::NonterminalParser* idExpressionNonterminalParser = GetNonterminal(ToUtf32("IdExpression"));
        idExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PreIdExpression));
        idExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostIdExpression));
        gendoc::parsing::NonterminalParser* s1NonterminalParser = GetNonterminal(ToUtf32("s1"));
        s1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::Posts1));
        gendoc::parsing::NonterminalParser* argsNonterminalParser = GetNonterminal(ToUtf32("args"));
        argsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::Preargs));
        argsNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::Postargs));
        gendoc::parsing::NonterminalParser* s2NonterminalParser = GetNonterminal(ToUtf32("s2"));
        s2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::Posts2));
        gendoc::parsing::NonterminalParser* bracedInitializerListNonterminalParser = GetNonterminal(ToUtf32("BracedInitializerList"));
        bracedInitializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PreBracedInitializerList));
        bracedInitializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostBracedInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromLiteral);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(new ThisNode(span));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(new ParenthesizedExprNode(span, context->fromExpression));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromCppCastExpression);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromTypeIdExpression);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromIdExpression);
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(new SimpleTypeCastExpressionNode(span, new SimpleTypeNode(span, std::vector < SimpleTypeSpecifier > (1 , context->froms1)), context->expr.release()));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromargs);
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(new SimpleTypeCastExpressionNode(span, new SimpleTypeNode(span, std::vector < SimpleTypeSpecifier > (1 , context->froms2)), context->expr.release()));
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromBracedInitializerList);
    }
    void PostLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLiteral_value = std::move(stack.top());
            context->fromLiteral = *static_cast<gendoc::parsing::ValueObject<LiteralNode*>*>(fromLiteral_value.get());
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
    void PreCppCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCppCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCppCastExpression_value = std::move(stack.top());
            context->fromCppCastExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromCppCastExpression_value.get());
            stack.pop();
        }
    }
    void PreTypeIdExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeIdExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeIdExpression_value = std::move(stack.top());
            context->fromTypeIdExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTypeIdExpression_value.get());
            stack.pop();
        }
    }
    void PreIdExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostIdExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIdExpression_value = std::move(stack.top());
            context->fromIdExpression = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromIdExpression_value.get());
            stack.pop();
        }
    }
    void Posts1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms1_value = std::move(stack.top());
            context->froms1 = *static_cast<gendoc::parsing::ValueObject<SimpleTypeSpecifier>*>(froms1_value.get());
            stack.pop();
        }
    }
    void Preargs(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postargs(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromargs_value = std::move(stack.top());
            context->fromargs = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromargs_value.get());
            stack.pop();
        }
    }
    void Posts2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms2_value = std::move(stack.top());
            context->froms2 = *static_cast<gendoc::parsing::ValueObject<SimpleTypeSpecifier>*>(froms2_value.get());
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
        Context(): ctx(), value(), expr(), fromLiteral(), fromExpression(), fromCppCastExpression(), fromTypeIdExpression(), fromIdExpression(), froms1(), fromargs(), froms2(), fromBracedInitializerList() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        LiteralNode* fromLiteral;
        Node* fromExpression;
        Node* fromCppCastExpression;
        Node* fromTypeIdExpression;
        Node* fromIdExpression;
        SimpleTypeSpecifier froms1;
        Node* fromargs;
        SimpleTypeSpecifier froms2;
        BracedInitializerListNode* fromBracedInitializerList;
    };
};

class Expression::CppCastExpressionRule : public gendoc::parsing::Rule
{
public:
    CppCastExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CppCastExpressionRule>(this, &CppCastExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CppCastExpressionRule>(this, &CppCastExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CppCastExpressionRule>(this, &CppCastExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CppCastExpressionRule>(this, &CppCastExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CppCastExpressionRule>(this, &CppCastExpressionRule::A4Action));
        gendoc::parsing::NonterminalParser* t1NonterminalParser = GetNonterminal(ToUtf32("t1"));
        t1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<CppCastExpressionRule>(this, &CppCastExpressionRule::Pret1));
        t1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CppCastExpressionRule>(this, &CppCastExpressionRule::Postt1));
        gendoc::parsing::NonterminalParser* e1NonterminalParser = GetNonterminal(ToUtf32("e1"));
        e1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<CppCastExpressionRule>(this, &CppCastExpressionRule::Pree1));
        e1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CppCastExpressionRule>(this, &CppCastExpressionRule::Poste1));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CppCastExpressionNode(span, context->fromt1, context->frome1, context->op);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::dynamicCast;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::staticCast;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::reinterpretCast;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::constCast;
    }
    void Pret1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postt1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromt1_value = std::move(stack.top());
            context->fromt1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromt1_value.get());
            stack.pop();
        }
    }
    void Pree1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Poste1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frome1_value = std::move(stack.top());
            context->frome1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(frome1_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), op(), fromt1(), frome1() {}
        ParsingContext* ctx;
        Node* value;
        Operator op;
        Node* fromt1;
        Node* frome1;
    };
};

class Expression::TypeIdExpressionRule : public gendoc::parsing::Rule
{
public:
    TypeIdExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeIdExpressionRule>(this, &TypeIdExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeIdExpressionRule>(this, &TypeIdExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* t1NonterminalParser = GetNonterminal(ToUtf32("t1"));
        t1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TypeIdExpressionRule>(this, &TypeIdExpressionRule::Pret1));
        t1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeIdExpressionRule>(this, &TypeIdExpressionRule::Postt1));
        gendoc::parsing::NonterminalParser* e1NonterminalParser = GetNonterminal(ToUtf32("e1"));
        e1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TypeIdExpressionRule>(this, &TypeIdExpressionRule::Pree1));
        e1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeIdExpressionRule>(this, &TypeIdExpressionRule::Poste1));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeIdExpressionNode(span, context->fromt1);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeIdExpressionNode(span, context->frome1);
    }
    void Pret1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postt1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromt1_value = std::move(stack.top());
            context->fromt1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromt1_value.get());
            stack.pop();
        }
    }
    void Pree1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Poste1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frome1_value = std::move(stack.top());
            context->frome1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(frome1_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromt1(), frome1() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromt1;
        Node* frome1;
    };
};

class Expression::IdExpressionRule : public gendoc::parsing::Rule
{
public:
    IdExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IdExpressionRule>(this, &IdExpressionRule::PreQualifiedId));
        qualifiedIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::PostQualifiedId));
        gendoc::parsing::NonterminalParser* unqualifiedIdNonterminalParser = GetNonterminal(ToUtf32("UnqualifiedId"));
        unqualifiedIdNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IdExpressionRule>(this, &IdExpressionRule::PreUnqualifiedId));
        unqualifiedIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::PostUnqualifiedId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromQualifiedId;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUnqualifiedId;
    }
    void PreQualifiedId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostQualifiedId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context->fromQualifiedId = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
    void PreUnqualifiedId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostUnqualifiedId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUnqualifiedId_value = std::move(stack.top());
            context->fromUnqualifiedId = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromUnqualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromQualifiedId(), fromUnqualifiedId() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromQualifiedId;
        Node* fromUnqualifiedId;
    };
};

class Expression::UnqualifiedIdRule : public gendoc::parsing::Rule
{
public:
    UnqualifiedIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnqualifiedIdRule>(this, &UnqualifiedIdRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnqualifiedIdRule>(this, &UnqualifiedIdRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnqualifiedIdRule>(this, &UnqualifiedIdRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnqualifiedIdRule>(this, &UnqualifiedIdRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnqualifiedIdRule>(this, &UnqualifiedIdRule::A4Action));
        gendoc::parsing::NonterminalParser* templateIdNonterminalParser = GetNonterminal(ToUtf32("TemplateId"));
        templateIdNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnqualifiedIdRule>(this, &UnqualifiedIdRule::PreTemplateId));
        templateIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnqualifiedIdRule>(this, &UnqualifiedIdRule::PostTemplateId));
        gendoc::parsing::NonterminalParser* id1NonterminalParser = GetNonterminal(ToUtf32("id1"));
        id1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnqualifiedIdRule>(this, &UnqualifiedIdRule::Postid1));
        gendoc::parsing::NonterminalParser* operatorFunctionIdNonterminalParser = GetNonterminal(ToUtf32("OperatorFunctionId"));
        operatorFunctionIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnqualifiedIdRule>(this, &UnqualifiedIdRule::PostOperatorFunctionId));
        gendoc::parsing::NonterminalParser* conversionFunctionIdNonterminalParser = GetNonterminal(ToUtf32("ConversionFunctionId"));
        conversionFunctionIdNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnqualifiedIdRule>(this, &UnqualifiedIdRule::PreConversionFunctionId));
        conversionFunctionIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnqualifiedIdRule>(this, &UnqualifiedIdRule::PostConversionFunctionId));
        gendoc::parsing::NonterminalParser* id2NonterminalParser = GetNonterminal(ToUtf32("id2"));
        id2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnqualifiedIdRule>(this, &UnqualifiedIdRule::Postid2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTemplateId;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdentifierNode(span, context->fromid1);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOperatorFunctionId;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConversionFunctionId;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DtorIdNode(span, context->fromid2);
    }
    void PreTemplateId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTemplateId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTemplateId_value = std::move(stack.top());
            context->fromTemplateId = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTemplateId_value.get());
            stack.pop();
        }
    }
    void Postid1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromid1_value = std::move(stack.top());
            context->fromid1 = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromid1_value.get());
            stack.pop();
        }
    }
    void PostOperatorFunctionId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOperatorFunctionId_value = std::move(stack.top());
            context->fromOperatorFunctionId = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromOperatorFunctionId_value.get());
            stack.pop();
        }
    }
    void PreConversionFunctionId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConversionFunctionId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromConversionFunctionId_value = std::move(stack.top());
            context->fromConversionFunctionId = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromConversionFunctionId_value.get());
            stack.pop();
        }
    }
    void Postid2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromid2_value = std::move(stack.top());
            context->fromid2 = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromid2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromTemplateId(), fromid1(), fromOperatorFunctionId(), fromConversionFunctionId(), fromid2() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTemplateId;
        std::u32string fromid1;
        Node* fromOperatorFunctionId;
        Node* fromConversionFunctionId;
        std::u32string fromid2;
    };
};

class Expression::QualifiedIdRule : public gendoc::parsing::Rule
{
public:
    QualifiedIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("node")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A6Action));
        gendoc::parsing::NonterminalParser* templateIdNonterminalParser = GetNonterminal(ToUtf32("TemplateId"));
        templateIdNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdRule>(this, &QualifiedIdRule::PreTemplateId));
        templateIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::PostTemplateId));
        gendoc::parsing::NonterminalParser* ns1NonterminalParser = GetNonterminal(ToUtf32("ns1"));
        ns1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdRule>(this, &QualifiedIdRule::Prens1));
        ns1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postns1));
        gendoc::parsing::NonterminalParser* uid1NonterminalParser = GetNonterminal(ToUtf32("uid1"));
        uid1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdRule>(this, &QualifiedIdRule::Preuid1));
        uid1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postuid1));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::PostIdentifier));
        gendoc::parsing::NonterminalParser* operatorFunctionIdNonterminalParser = GetNonterminal(ToUtf32("OperatorFunctionId"));
        operatorFunctionIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::PostOperatorFunctionId));
        gendoc::parsing::NonterminalParser* ns2NonterminalParser = GetNonterminal(ToUtf32("ns2"));
        ns2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdRule>(this, &QualifiedIdRule::Prens2));
        ns2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postns2));
        gendoc::parsing::NonterminalParser* uid2NonterminalParser = GetNonterminal(ToUtf32("uid2"));
        uid2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<QualifiedIdRule>(this, &QualifiedIdRule::Preuid2));
        uid2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postuid2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NestedIdNode(span, nullptr, context->fromTemplateId);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NestedIdNode(span, new NestedIdNode(span, nullptr, context->node.release()), context->fromuid1);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(context->fromns1);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NestedIdNode(span, nullptr, new IdentifierNode(span, context->fromIdentifier));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NestedIdNode(span, nullptr, context->fromOperatorFunctionId);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NestedIdNode(span, context->node.release(), context->fromuid2);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(context->fromns2);
    }
    void PreTemplateId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTemplateId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTemplateId_value = std::move(stack.top());
            context->fromTemplateId = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTemplateId_value.get());
            stack.pop();
        }
    }
    void Prens1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postns1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromns1_value = std::move(stack.top());
            context->fromns1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromns1_value.get());
            stack.pop();
        }
    }
    void Preuid1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postuid1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromuid1_value = std::move(stack.top());
            context->fromuid1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromuid1_value.get());
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
    void PostOperatorFunctionId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOperatorFunctionId_value = std::move(stack.top());
            context->fromOperatorFunctionId = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromOperatorFunctionId_value.get());
            stack.pop();
        }
    }
    void Prens2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postns2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromns2_value = std::move(stack.top());
            context->fromns2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromns2_value.get());
            stack.pop();
        }
    }
    void Preuid2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postuid2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromuid2_value = std::move(stack.top());
            context->fromuid2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromuid2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), node(), fromTemplateId(), fromns1(), fromuid1(), fromIdentifier(), fromOperatorFunctionId(), fromns2(), fromuid2() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> node;
        Node* fromTemplateId;
        Node* fromns1;
        Node* fromuid1;
        std::u32string fromIdentifier;
        Node* fromOperatorFunctionId;
        Node* fromns2;
        Node* fromuid2;
    };
};

class Expression::OperatorFunctionIdRule : public gendoc::parsing::Rule
{
public:
    OperatorFunctionIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Node*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorFunctionIdRule>(this, &OperatorFunctionIdRule::A0Action));
        gendoc::parsing::NonterminalParser* operatorNonterminalParser = GetNonterminal(ToUtf32("Operator"));
        operatorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<OperatorFunctionIdRule>(this, &OperatorFunctionIdRule::PostOperator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OperatorFunctionIdNode(span, context->fromOperator);
    }
    void PostOperator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOperator_value = std::move(stack.top());
            context->fromOperator = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromOperator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromOperator() {}
        Node* value;
        Operator fromOperator;
    };
};

class Expression::OperatorRule : public gendoc::parsing::Rule
{
public:
    OperatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A9Action));
        gendoc::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A10Action));
        gendoc::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A11Action));
        gendoc::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A12Action));
        gendoc::parsing::ActionParser* a13ActionParser = GetAction(ToUtf32("A13"));
        a13ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A13Action));
        gendoc::parsing::ActionParser* a14ActionParser = GetAction(ToUtf32("A14"));
        a14ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A14Action));
        gendoc::parsing::ActionParser* a15ActionParser = GetAction(ToUtf32("A15"));
        a15ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A15Action));
        gendoc::parsing::ActionParser* a16ActionParser = GetAction(ToUtf32("A16"));
        a16ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A16Action));
        gendoc::parsing::ActionParser* a17ActionParser = GetAction(ToUtf32("A17"));
        a17ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A17Action));
        gendoc::parsing::ActionParser* a18ActionParser = GetAction(ToUtf32("A18"));
        a18ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A18Action));
        gendoc::parsing::ActionParser* a19ActionParser = GetAction(ToUtf32("A19"));
        a19ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A19Action));
        gendoc::parsing::ActionParser* a20ActionParser = GetAction(ToUtf32("A20"));
        a20ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A20Action));
        gendoc::parsing::ActionParser* a21ActionParser = GetAction(ToUtf32("A21"));
        a21ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A21Action));
        gendoc::parsing::ActionParser* a22ActionParser = GetAction(ToUtf32("A22"));
        a22ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A22Action));
        gendoc::parsing::ActionParser* a23ActionParser = GetAction(ToUtf32("A23"));
        a23ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A23Action));
        gendoc::parsing::ActionParser* a24ActionParser = GetAction(ToUtf32("A24"));
        a24ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A24Action));
        gendoc::parsing::ActionParser* a25ActionParser = GetAction(ToUtf32("A25"));
        a25ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A25Action));
        gendoc::parsing::ActionParser* a26ActionParser = GetAction(ToUtf32("A26"));
        a26ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A26Action));
        gendoc::parsing::ActionParser* a27ActionParser = GetAction(ToUtf32("A27"));
        a27ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A27Action));
        gendoc::parsing::ActionParser* a28ActionParser = GetAction(ToUtf32("A28"));
        a28ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A28Action));
        gendoc::parsing::ActionParser* a29ActionParser = GetAction(ToUtf32("A29"));
        a29ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A29Action));
        gendoc::parsing::ActionParser* a30ActionParser = GetAction(ToUtf32("A30"));
        a30ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A30Action));
        gendoc::parsing::ActionParser* a31ActionParser = GetAction(ToUtf32("A31"));
        a31ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A31Action));
        gendoc::parsing::ActionParser* a32ActionParser = GetAction(ToUtf32("A32"));
        a32ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OperatorRule>(this, &OperatorRule::A32Action));
        gendoc::parsing::NonterminalParser* assignmentOperatorNonterminalParser = GetNonterminal(ToUtf32("AssignmentOperator"));
        assignmentOperatorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<OperatorRule>(this, &OperatorRule::PostAssignmentOperator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::newArray;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::deleteArray;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::new_;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::delete_;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAssignmentOperator;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::inc;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::dec;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftLeft;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftRight;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::logicalAnd;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::logicalOr;
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::equal;
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::notEqual;
    }
    void A13Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::lessOrEqual;
    }
    void A14Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::greaterOrEqual;
    }
    void A15Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::dotStar;
    }
    void A16Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::arrowStar;
    }
    void A17Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::arrow;
    }
    void A18Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::apply;
    }
    void A19Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::subscript;
    }
    void A20Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::less;
    }
    void A21Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::greater;
    }
    void A22Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::add;
    }
    void A23Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::sub;
    }
    void A24Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::mul;
    }
    void A25Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::div;
    }
    void A26Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::rem;
    }
    void A27Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::xor_;
    }
    void A28Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::and_;
    }
    void A29Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::or_;
    }
    void A30Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::complement;
    }
    void A31Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::not_;
    }
    void A32Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::comma;
    }
    void PostAssignmentOperator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAssignmentOperator_value = std::move(stack.top());
            context->fromAssignmentOperator = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromAssignmentOperator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromAssignmentOperator() {}
        Operator value;
        Operator fromAssignmentOperator;
    };
};

class Expression::ConversionFunctionIdRule : public gendoc::parsing::Rule
{
public:
    ConversionFunctionIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConversionFunctionIdRule>(this, &ConversionFunctionIdRule::A0Action));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ConversionFunctionIdRule>(this, &ConversionFunctionIdRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConversionFunctionIdRule>(this, &ConversionFunctionIdRule::PostTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConversionFunctionIdNode(span, context->fromTypeExpr);
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromTypeExpr() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTypeExpr;
    };
};

void Expression::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Literal"));
    if (!grammar0)
    {
        grammar0 = gendoc::cppparser::Literal::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Identifier"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.cppparser.SimpleType"));
    if (!grammar2)
    {
        grammar2 = gendoc::cppparser::SimpleType::Create(pd);
    }
    AddGrammarReference(grammar2);
    gendoc::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Declarator"));
    if (!grammar3)
    {
        grammar3 = gendoc::cppparser::Declarator::Create(pd);
    }
    AddGrammarReference(grammar3);
    gendoc::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Template"));
    if (!grammar4)
    {
        grammar4 = gendoc::cppparser::Template::Create(pd);
    }
    AddGrammarReference(grammar4);
    gendoc::parsing::Grammar* grammar5 = pd->GetGrammar(ToUtf32("gendoc.cppparser.TypeExpr"));
    if (!grammar5)
    {
        grammar5 = gendoc::cppparser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar5);
}

void Expression::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Literal"), this, ToUtf32("Literal.Literal")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("BracedInitializerList"), this, ToUtf32("Declarator.BracedInitializerList")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("InitializerList"), this, ToUtf32("Declarator.InitializerList")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TemplateId"), this, ToUtf32("Template.TemplateId")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("InitializerClause"), this, ToUtf32("Declarator.InitializerClause")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("SimpleTypeSpecifier"), this, ToUtf32("SimpleType.SimpleTypeSpecifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("SimpleTemplateId"), this, ToUtf32("Template.SimpleTemplateId")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("NestedNameSpecifier"), this, ToUtf32("Identifier.NestedNameSpecifier")));
    AddRule(new ExpressionListRule(ToUtf32("ExpressionList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("InitializerList"), ToUtf32("InitializerList"), 1))));
    AddRule(new PossiblyEmptyArgumentListRule(ToUtf32("PossiblyEmptyArgumentList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::EmptyParser()))));
    AddRule(new ExpressionRule(ToUtf32("Expression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AssignmentExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser(','),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AssignmentExpression"), 1))))))))));
    AddRule(new ConstantExpressionRule(ToUtf32("ConstantExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("ConditionalExpression"), ToUtf32("ConditionalExpression"), 1))));
    AddRule(new AssignmentExpressionRule(ToUtf32("AssignmentExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ConcreteAssignmentExpression"), ToUtf32("ConcreteAssignmentExpression"), 1)),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ConditionalExpression"), ToUtf32("ConditionalExpression"), 1))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ThrowExpression"), ToUtf32("ThrowExpression"), 1)))));
    AddRule(new ConcreteAssignmentExpressionRule(ToUtf32("ConcreteAssignmentExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::EmptyParser()),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("LogicalOrExpression"), ToUtf32("LogicalOrExpression"), 1)),
                            new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentOperator"), ToUtf32("AssignmentOperator"), 0)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("InitializerClause"), ToUtf32("InitializerClause"), 1)))))));
    AddRule(new AssignmentOperatorRule(ToUtf32("AssignmentOperator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                    new gendoc::parsing::GroupingParser(
                                                        new gendoc::parsing::DifferenceParser(
                                                            new gendoc::parsing::StringParser(ToUtf32("=")),
                                                            new gendoc::parsing::StringParser(ToUtf32("=="))))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                    new gendoc::parsing::StringParser(ToUtf32("*=")))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                new gendoc::parsing::StringParser(ToUtf32("/=")))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                            new gendoc::parsing::StringParser(ToUtf32("%=")))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                        new gendoc::parsing::StringParser(ToUtf32("+=")))),
                                new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                    new gendoc::parsing::StringParser(ToUtf32("-=")))),
                            new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                new gendoc::parsing::StringParser(ToUtf32(">>=")))),
                        new gendoc::parsing::ActionParser(ToUtf32("A7"),
                            new gendoc::parsing::StringParser(ToUtf32("<<=")))),
                    new gendoc::parsing::ActionParser(ToUtf32("A8"),
                        new gendoc::parsing::StringParser(ToUtf32("&=")))),
                new gendoc::parsing::ActionParser(ToUtf32("A9"),
                    new gendoc::parsing::StringParser(ToUtf32("^=")))),
            new gendoc::parsing::ActionParser(ToUtf32("A10"),
                new gendoc::parsing::StringParser(ToUtf32("|="))))));
    AddRule(new ConditionalExpressionRule(ToUtf32("ConditionalExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("LogicalOrExpression"), ToUtf32("LogicalOrExpression"), 1)),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::CharParser('?'),
                                                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1))),
                                            new gendoc::parsing::CharParser(':')),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 1)))))))))));
    AddRule(new ThrowExpressionRule(ToUtf32("ThrowExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::KeywordParser(ToUtf32("throw")),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 1)))))));
    AddRule(new LogicalOrExpressionRule(ToUtf32("LogicalOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("LogicalAndExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("||")),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("LogicalAndExpression"), 1))))))))));
    AddRule(new LogicalAndExpressionRule(ToUtf32("LogicalAndExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("InclusiveOrExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("&&")),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("InclusiveOrExpression"), 1))))))))));
    AddRule(new InclusiveOrExpressionRule(ToUtf32("InclusiveOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ExclusiveOrExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::DifferenceParser(
                                        new gendoc::parsing::CharParser('|'),
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::StringParser(ToUtf32("|=")),
                                                new gendoc::parsing::StringParser(ToUtf32("||")))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ExclusiveOrExpression"), 1))))))))));
    AddRule(new ExclusiveOrExpressionRule(ToUtf32("ExclusiveOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AndExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::DifferenceParser(
                                        new gendoc::parsing::CharParser('^'),
                                        new gendoc::parsing::StringParser(ToUtf32("^=")))),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AndExpression"), 1))))))))));
    AddRule(new AndExpressionRule(ToUtf32("AndExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("EqualityExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::DifferenceParser(
                                        new gendoc::parsing::CharParser('&'),
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::StringParser(ToUtf32("&=")),
                                                new gendoc::parsing::StringParser(ToUtf32("&&")))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("EqualityExpression"), 1))))))))));
    AddRule(new EqualityExpressionRule(ToUtf32("EqualityExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("RelationalExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::StringParser(ToUtf32("=="))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                            new gendoc::parsing::StringParser(ToUtf32("!="))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelationalExpression"), 1))))))))));
    AddRule(new RelationalExpressionRule(ToUtf32("RelationalExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ShiftExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                    new gendoc::parsing::StringParser(ToUtf32("<="))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                    new gendoc::parsing::StringParser(ToUtf32(">=")))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                new gendoc::parsing::GroupingParser(
                                                    new gendoc::parsing::DifferenceParser(
                                                        new gendoc::parsing::CharParser('<'),
                                                        new gendoc::parsing::GroupingParser(
                                                            new gendoc::parsing::AlternativeParser(
                                                                new gendoc::parsing::StringParser(ToUtf32("<<")),
                                                                new gendoc::parsing::StringParser(ToUtf32("<<=")))))))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::DifferenceParser(
                                                    new gendoc::parsing::CharParser('>'),
                                                    new gendoc::parsing::GroupingParser(
                                                        new gendoc::parsing::AlternativeParser(
                                                            new gendoc::parsing::StringParser(ToUtf32(">>")),
                                                            new gendoc::parsing::StringParser(ToUtf32(">>="))))))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ShiftExpression"), 1))))))))));
    AddRule(new ShiftExpressionRule(ToUtf32("ShiftExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AdditiveExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::DifferenceParser(
                                                    new gendoc::parsing::StringParser(ToUtf32("<<")),
                                                    new gendoc::parsing::StringParser(ToUtf32("<<="))))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::DifferenceParser(
                                                    new gendoc::parsing::StringParser(ToUtf32(">>")),
                                                    new gendoc::parsing::StringParser(ToUtf32(">>="))))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AdditiveExpression"), 1))))))))));
    AddRule(new AdditiveExpressionRule(ToUtf32("AdditiveExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("MultiplicativeExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::DifferenceParser(
                                                    new gendoc::parsing::CharParser('+'),
                                                    new gendoc::parsing::StringParser(ToUtf32("+="))))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::DifferenceParser(
                                                    new gendoc::parsing::CharParser('-'),
                                                    new gendoc::parsing::StringParser(ToUtf32("-="))))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("MultiplicativeExpression"), 1))))))))));
    AddRule(new MultiplicativeExpressionRule(ToUtf32("MultiplicativeExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("PMExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                new gendoc::parsing::GroupingParser(
                                                    new gendoc::parsing::DifferenceParser(
                                                        new gendoc::parsing::CharParser('*'),
                                                        new gendoc::parsing::StringParser(ToUtf32("*="))))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                new gendoc::parsing::GroupingParser(
                                                    new gendoc::parsing::DifferenceParser(
                                                        new gendoc::parsing::CharParser('/'),
                                                        new gendoc::parsing::StringParser(ToUtf32("/=")))))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::DifferenceParser(
                                                    new gendoc::parsing::CharParser('%'),
                                                    new gendoc::parsing::StringParser(ToUtf32("%="))))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("PMExpression"), 1))))))))));
    AddRule(new PMExpressionRule(ToUtf32("PMExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("CastExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::StringParser(ToUtf32(".*"))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                            new gendoc::parsing::StringParser(ToUtf32("->*"))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("CastExpression"), 1))))))))));
    AddRule(new CastExpressionRule(ToUtf32("CastExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser('('),
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))),
                            new gendoc::parsing::CharParser(')')),
                        new gendoc::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 1)))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("UnaryExpression"), ToUtf32("UnaryExpression"), 1)))));
    AddRule(new UnaryExpressionRule(ToUtf32("UnaryExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::StringParser(ToUtf32("++")),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("c1"), ToUtf32("CastExpression"), 1)))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::StringParser(ToUtf32("--")),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("c2"), ToUtf32("CastExpression"), 1))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("UnaryOperator"), ToUtf32("UnaryOperator"), 0),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("c3"), ToUtf32("CastExpression"), 1))))),
                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::KeywordParser(ToUtf32("sizeof")),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("u4"), ToUtf32("UnaryExpression"), 1))))),
                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::KeywordParser(ToUtf32("alignof")),
                                            new gendoc::parsing::CharParser('(')),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("t5"), ToUtf32("TypeExpr"), 1)),
                                    new gendoc::parsing::CharParser(')'))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A5"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("NewExpression"), ToUtf32("NewExpression"), 1))),
                new gendoc::parsing::ActionParser(ToUtf32("A6"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("DeleteExpression"), ToUtf32("DeleteExpression"), 1))),
            new gendoc::parsing::ActionParser(ToUtf32("A7"),
                new gendoc::parsing::NonterminalParser(ToUtf32("PostfixExpression"), ToUtf32("PostfixExpression"), 1)))));
    AddRule(new UnaryOperatorRule(ToUtf32("UnaryOperator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::DifferenceParser(
                                        new gendoc::parsing::CharParser('*'),
                                        new gendoc::parsing::StringParser(ToUtf32("*="))))),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::DifferenceParser(
                                        new gendoc::parsing::CharParser('&'),
                                        new gendoc::parsing::StringParser(ToUtf32("&=")))))),
                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::DifferenceParser(
                                    new gendoc::parsing::CharParser('+'),
                                    new gendoc::parsing::StringParser(ToUtf32("+=")))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::DifferenceParser(
                                new gendoc::parsing::CharParser('-'),
                                new gendoc::parsing::StringParser(ToUtf32("-=")))))),
                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                    new gendoc::parsing::CharParser('!'))),
            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                new gendoc::parsing::CharParser('~')))));
    AddRule(new NewExpressionRule(ToUtf32("NewExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("new")),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("NewPlacement"), ToUtf32("NewPlacement"), 1))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("NewInitializer"), ToUtf32("NewInitializer"), 1)))))));
    AddRule(new NewPlacementRule(ToUtf32("NewPlacement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('('),
                        new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 1)),
                    new gendoc::parsing::CharParser(')'))))));
    AddRule(new NewInitializerRule(ToUtf32("NewInitializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('('),
                        new gendoc::parsing::NonterminalParser(ToUtf32("args"), ToUtf32("PossiblyEmptyArgumentList"), 1)),
                    new gendoc::parsing::CharParser(')'))))));
    AddRule(new DeleteExpressionRule(ToUtf32("DeleteExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("delete")),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::CharParser('['),
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::CharParser(']')))))),
                    new gendoc::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 1))))));
    AddRule(new PostfixExpressionRule(ToUtf32("PostfixExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("PrimaryExpression"), ToUtf32("PrimaryExpression"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                            new gendoc::parsing::CharParser('[')),
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("index"), ToUtf32("Expression"), 1)),
                                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                        new gendoc::parsing::CharParser(']'))),
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                            new gendoc::parsing::CharParser('(')),
                                                        new gendoc::parsing::GroupingParser(
                                                            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                                                new gendoc::parsing::NonterminalParser(ToUtf32("arguments"), ToUtf32("PossiblyEmptyArgumentList"), 1)))),
                                                    new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                                        new gendoc::parsing::CharParser(')')))),
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::GroupingParser(
                                                    new gendoc::parsing::DifferenceParser(
                                                        new gendoc::parsing::CharParser('.'),
                                                        new gendoc::parsing::StringParser(ToUtf32(".*")))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A7"),
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("id1"), ToUtf32("IdExpression"), 1)))),
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::DifferenceParser(
                                                    new gendoc::parsing::StringParser(ToUtf32("->")),
                                                    new gendoc::parsing::StringParser(ToUtf32("->*")))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A8"),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("id2"), ToUtf32("IdExpression"), 1)))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A9"),
                                        new gendoc::parsing::StringParser(ToUtf32("++")))),
                                new gendoc::parsing::ActionParser(ToUtf32("A10"),
                                    new gendoc::parsing::StringParser(ToUtf32("--")))))))))));
    AddRule(new PrimaryExpressionRule(ToUtf32("PrimaryExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("Literal"), ToUtf32("Literal"), 0)),
                                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                new gendoc::parsing::KeywordParser(ToUtf32("this")))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::CharParser('('),
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1)),
                                                    new gendoc::parsing::CharParser(')'))))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("CppCastExpression"), ToUtf32("CppCastExpression"), 1))),
                                new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("TypeIdExpression"), ToUtf32("TypeIdExpression"), 1))),
                            new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("IdExpression"), ToUtf32("IdExpression"), 1))),
                        new gendoc::parsing::ActionParser(ToUtf32("A7"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("SimpleTypeSpecifier"), 0),
                                            new gendoc::parsing::CharParser('(')),
                                        new gendoc::parsing::ActionParser(ToUtf32("A8"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("args"), ToUtf32("PossiblyEmptyArgumentList"), 1))),
                                    new gendoc::parsing::CharParser(')'))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A9"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("s2"), ToUtf32("SimpleTypeSpecifier"), 0),
                                new gendoc::parsing::ActionParser(ToUtf32("A10"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("BracedInitializerList"), ToUtf32("BracedInitializerList"), 1))))))))));
    AddRule(new CppCastExpressionRule(ToUtf32("CppCastExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::AlternativeParser(
                                                    new gendoc::parsing::AlternativeParser(
                                                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                            new gendoc::parsing::KeywordParser(ToUtf32("dynamic_cast"))),
                                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                            new gendoc::parsing::KeywordParser(ToUtf32("static_cast")))),
                                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                        new gendoc::parsing::KeywordParser(ToUtf32("reinterpret_cast")))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                    new gendoc::parsing::KeywordParser(ToUtf32("const_cast"))))),
                                        new gendoc::parsing::CharParser('<')),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("t1"), ToUtf32("TypeExpr"), 1)),
                                new gendoc::parsing::CharParser('>')),
                            new gendoc::parsing::CharParser('(')),
                        new gendoc::parsing::NonterminalParser(ToUtf32("e1"), ToUtf32("Expression"), 1)),
                    new gendoc::parsing::CharParser(')'))))));
    AddRule(new TypeIdExpressionRule(ToUtf32("TypeIdExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("typeid")),
                                new gendoc::parsing::CharParser('(')),
                            new gendoc::parsing::NonterminalParser(ToUtf32("t1"), ToUtf32("TypeExpr"), 1)),
                        new gendoc::parsing::CharParser(')')))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("typeid")),
                                new gendoc::parsing::CharParser('(')),
                            new gendoc::parsing::NonterminalParser(ToUtf32("e1"), ToUtf32("Expression"), 1)),
                        new gendoc::parsing::CharParser(')')))))));
    AddRule(new IdExpressionRule(ToUtf32("IdExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("UnqualifiedId"), ToUtf32("UnqualifiedId"), 1)))));
    AddRule(new UnqualifiedIdRule(ToUtf32("UnqualifiedId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("TemplateId"), ToUtf32("TemplateId"), 1)),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("id1"), ToUtf32("Identifier"), 0))),
                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("OperatorFunctionId"), ToUtf32("OperatorFunctionId"), 0))),
                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ConversionFunctionId"), ToUtf32("ConversionFunctionId"), 1))),
            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('~'),
                        new gendoc::parsing::NonterminalParser(ToUtf32("id2"), ToUtf32("Identifier"), 0)))))));
    AddRule(new QualifiedIdRule(ToUtf32("QualifiedId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::StringParser(ToUtf32("::")),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("TemplateId"), ToUtf32("TemplateId"), 1)))),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::StringParser(ToUtf32("::")),
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("ns1"), ToUtf32("NestedNameSpecifier"), 1))),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("uid1"), ToUtf32("UnqualifiedId"), 1))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::StringParser(ToUtf32("::")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))))),
                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::StringParser(ToUtf32("::")),
                            new gendoc::parsing::NonterminalParser(ToUtf32("OperatorFunctionId"), ToUtf32("OperatorFunctionId"), 0))))),
            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A6"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("ns2"), ToUtf32("NestedNameSpecifier"), 1)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("uid2"), ToUtf32("UnqualifiedId"), 1)))))));
    AddRule(new OperatorFunctionIdRule(ToUtf32("OperatorFunctionId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::KeywordParser(ToUtf32("operator")),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Operator"), ToUtf32("Operator"), 0))))));
    AddRule(new OperatorRule(ToUtf32("Operator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                                                                                                                            new gendoc::parsing::GroupingParser(
                                                                                                                                                new gendoc::parsing::SequenceParser(
                                                                                                                                                    new gendoc::parsing::KeywordParser(ToUtf32("new")),
                                                                                                                                                    new gendoc::parsing::StringParser(ToUtf32("[]"))))),
                                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                                                                                                            new gendoc::parsing::GroupingParser(
                                                                                                                                                new gendoc::parsing::SequenceParser(
                                                                                                                                                    new gendoc::parsing::KeywordParser(ToUtf32("delete")),
                                                                                                                                                    new gendoc::parsing::StringParser(ToUtf32("[]")))))),
                                                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                                                                                                        new gendoc::parsing::KeywordParser(ToUtf32("new")))),
                                                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                                                                                                    new gendoc::parsing::KeywordParser(ToUtf32("delete")))),
                                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentOperator"), ToUtf32("AssignmentOperator"), 0))),
                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                                                                                                            new gendoc::parsing::StringParser(ToUtf32("++")))),
                                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                                                                                                        new gendoc::parsing::StringParser(ToUtf32("--")))),
                                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A7"),
                                                                                                                    new gendoc::parsing::StringParser(ToUtf32("<<")))),
                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A8"),
                                                                                                                new gendoc::parsing::StringParser(ToUtf32(">>")))),
                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A9"),
                                                                                                            new gendoc::parsing::StringParser(ToUtf32("&&")))),
                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A10"),
                                                                                                        new gendoc::parsing::StringParser(ToUtf32("||")))),
                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A11"),
                                                                                                    new gendoc::parsing::StringParser(ToUtf32("==")))),
                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A12"),
                                                                                                new gendoc::parsing::StringParser(ToUtf32("!=")))),
                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A13"),
                                                                                            new gendoc::parsing::StringParser(ToUtf32("<=")))),
                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A14"),
                                                                                        new gendoc::parsing::StringParser(ToUtf32(">=")))),
                                                                                new gendoc::parsing::ActionParser(ToUtf32("A15"),
                                                                                    new gendoc::parsing::StringParser(ToUtf32(".*")))),
                                                                            new gendoc::parsing::ActionParser(ToUtf32("A16"),
                                                                                new gendoc::parsing::StringParser(ToUtf32("->*")))),
                                                                        new gendoc::parsing::ActionParser(ToUtf32("A17"),
                                                                            new gendoc::parsing::StringParser(ToUtf32("->")))),
                                                                    new gendoc::parsing::ActionParser(ToUtf32("A18"),
                                                                        new gendoc::parsing::StringParser(ToUtf32("()")))),
                                                                new gendoc::parsing::ActionParser(ToUtf32("A19"),
                                                                    new gendoc::parsing::StringParser(ToUtf32("[]")))),
                                                            new gendoc::parsing::ActionParser(ToUtf32("A20"),
                                                                new gendoc::parsing::CharParser('<'))),
                                                        new gendoc::parsing::ActionParser(ToUtf32("A21"),
                                                            new gendoc::parsing::CharParser('>'))),
                                                    new gendoc::parsing::ActionParser(ToUtf32("A22"),
                                                        new gendoc::parsing::CharParser('+'))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A23"),
                                                    new gendoc::parsing::CharParser('-'))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A24"),
                                                new gendoc::parsing::CharParser('*'))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A25"),
                                            new gendoc::parsing::CharParser('/'))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A26"),
                                        new gendoc::parsing::CharParser('%'))),
                                new gendoc::parsing::ActionParser(ToUtf32("A27"),
                                    new gendoc::parsing::CharParser('^'))),
                            new gendoc::parsing::ActionParser(ToUtf32("A28"),
                                new gendoc::parsing::CharParser('&'))),
                        new gendoc::parsing::ActionParser(ToUtf32("A29"),
                            new gendoc::parsing::CharParser('|'))),
                    new gendoc::parsing::ActionParser(ToUtf32("A30"),
                        new gendoc::parsing::CharParser('~'))),
                new gendoc::parsing::ActionParser(ToUtf32("A31"),
                    new gendoc::parsing::CharParser('!'))),
            new gendoc::parsing::ActionParser(ToUtf32("A32"),
                new gendoc::parsing::CharParser(',')))));
    AddRule(new ConversionFunctionIdRule(ToUtf32("ConversionFunctionId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::KeywordParser(ToUtf32("operator")),
                    new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))))));
}

} } // namespace gendoc.cppparser
