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
#include <gendoc/code/Declaration.hpp>
#include <gendoc/code/Declarator.hpp>
#include <gendoc/code/Literal.hpp>
#include <gendoc/code/Identifier.hpp>
#include <gendoc/util/TextUtils.hpp>
#include <gendoc/codedom/Type.hpp>

namespace gendoc { namespace code {

using namespace gendoc::codedom;
using gendoc::util::Trim;
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

Expression::Expression(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Expression"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.code")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::codedom::CppObject* Expression::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    gendoc::codedom::CppObject* result = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(value.get());
    stack.pop();
    return result;
}

class Expression::ExpressionRule : public gendoc::parsing::Rule
{
public:
    ExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::comma, 0, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::ConstantExpressionRule : public gendoc::parsing::Rule
{
public:
    ConstantExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConstantExpressionRule>(this, &ConstantExpressionRule::A0Action));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConstantExpressionRule>(this, &ConstantExpressionRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpression;
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
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromExpression;
    };
};

class Expression::AssignmentExpressionRule : public gendoc::parsing::Rule
{
public:
    AssignmentExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("lor")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A3Action));
        gendoc::parsing::NonterminalParser* logicalOrExpressionNonterminalParser = GetNonterminal(ToUtf32("LogicalOrExpression"));
        logicalOrExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostLogicalOrExpression));
        gendoc::parsing::NonterminalParser* assingmentOpNonterminalParser = GetNonterminal(ToUtf32("AssingmentOp"));
        assingmentOpNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostAssingmentOp));
        gendoc::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostAssignmentExpression));
        gendoc::parsing::NonterminalParser* conditionalExpressionNonterminalParser = GetNonterminal(ToUtf32("ConditionalExpression"));
        conditionalExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostConditionalExpression));
        gendoc::parsing::NonterminalParser* throwExpressionNonterminalParser = GetNonterminal(ToUtf32("ThrowExpression"));
        throwExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostThrowExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->lor.release(), context->fromAssingmentOp, 1, context->fromAssignmentExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->lor.reset(context->fromLogicalOrExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConditionalExpression;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromThrowExpression;
    }
    void PostLogicalOrExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLogicalOrExpression_value = std::move(stack.top());
            context->fromLogicalOrExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromLogicalOrExpression_value.get());
            stack.pop();
        }
    }
    void PostAssingmentOp(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAssingmentOp_value = std::move(stack.top());
            context->fromAssingmentOp = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromAssingmentOp_value.get());
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
    void PostConditionalExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromConditionalExpression_value = std::move(stack.top());
            context->fromConditionalExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromConditionalExpression_value.get());
            stack.pop();
        }
    }
    void PostThrowExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromThrowExpression_value = std::move(stack.top());
            context->fromThrowExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromThrowExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), lor(), fromLogicalOrExpression(), fromAssingmentOp(), fromAssignmentExpression(), fromConditionalExpression(), fromThrowExpression() {}
        gendoc::codedom::CppObject* value;
        std::unique_ptr<CppObject> lor;
        gendoc::codedom::CppObject* fromLogicalOrExpression;
        Operator fromAssingmentOp;
        gendoc::codedom::CppObject* fromAssignmentExpression;
        gendoc::codedom::CppObject* fromConditionalExpression;
        gendoc::codedom::CppObject* fromThrowExpression;
    };
};

class Expression::AssingmentOpRule : public gendoc::parsing::Rule
{
public:
    AssingmentOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AssingmentOpRule>(this, &AssingmentOpRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = GetOperator(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::ThrowExpressionRule : public gendoc::parsing::Rule
{
public:
    ThrowExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ThrowExpressionRule>(this, &ThrowExpressionRule::A0Action));
        gendoc::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ThrowExpressionRule>(this, &ThrowExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ThrowExpr(context->fromAssignmentExpression);
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromAssignmentExpression() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromAssignmentExpression;
    };
};

class Expression::ConditionalExpressionRule : public gendoc::parsing::Rule
{
public:
    ConditionalExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostExpression));
        gendoc::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConditionalExpr(context->value, context->fromExpression, context->fromAssignmentExpression);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromExpression(), fromAssignmentExpression() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        gendoc::codedom::CppObject* fromExpression;
        gendoc::codedom::CppObject* fromAssignmentExpression;
    };
};

class Expression::LogicalOrExpressionRule : public gendoc::parsing::Rule
{
public:
    LogicalOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::or_, 2, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::LogicalAndExpressionRule : public gendoc::parsing::Rule
{
public:
    LogicalAndExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::and_, 3, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::InclusiveOrExpressionRule : public gendoc::parsing::Rule
{
public:
    InclusiveOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::bitor_, 4, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::ExclusiveOrExpressionRule : public gendoc::parsing::Rule
{
public:
    ExclusiveOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::bitxor, 5, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::AndExpressionRule : public gendoc::parsing::Rule
{
public:
    AndExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::bitand_, 6, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::EqualityExpressionRule : public gendoc::parsing::Rule
{
public:
    EqualityExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* eqOpNonterminalParser = GetNonterminal(ToUtf32("EqOp"));
        eqOpNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::PostEqOp));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromEqOp, 7, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostEqOp(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEqOp_value = std::move(stack.top());
            context->fromEqOp = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromEqOp_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromEqOp(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        Operator fromEqOp;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::EqOpRule : public gendoc::parsing::Rule
{
public:
    EqOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqOpRule>(this, &EqOpRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqOpRule>(this, &EqOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::eq;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::notEq;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::RelationalExpressionRule : public gendoc::parsing::Rule
{
public:
    RelationalExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* relOpNonterminalParser = GetNonterminal(ToUtf32("RelOp"));
        relOpNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::PostRelOp));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromRelOp, 8, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostRelOp(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromRelOp_value = std::move(stack.top());
            context->fromRelOp = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromRelOp_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromRelOp(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        Operator fromRelOp;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::RelOpRule : public gendoc::parsing::Rule
{
public:
    RelOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A3Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::lessOrEq;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::greaterOrEq;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::less;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::greater;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::ShiftExpressionRule : public gendoc::parsing::Rule
{
public:
    ShiftExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* shiftOpNonterminalParser = GetNonterminal(ToUtf32("ShiftOp"));
        shiftOpNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::PostShiftOp));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromShiftOp, 9, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostShiftOp(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromShiftOp_value = std::move(stack.top());
            context->fromShiftOp = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromShiftOp_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromShiftOp(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        Operator fromShiftOp;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::ShiftOpRule : public gendoc::parsing::Rule
{
public:
    ShiftOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ShiftOpRule>(this, &ShiftOpRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ShiftOpRule>(this, &ShiftOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftLeft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftRight;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::AdditiveExpressionRule : public gendoc::parsing::Rule
{
public:
    AdditiveExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* addOpNonterminalParser = GetNonterminal(ToUtf32("AddOp"));
        addOpNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::PostAddOp));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromAddOp, 10, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostAddOp(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAddOp_value = std::move(stack.top());
            context->fromAddOp = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromAddOp_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromAddOp(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        Operator fromAddOp;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::AddOpRule : public gendoc::parsing::Rule
{
public:
    AddOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AddOpRule>(this, &AddOpRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AddOpRule>(this, &AddOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::plus;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::minus;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::MultiplicativeExpressionRule : public gendoc::parsing::Rule
{
public:
    MultiplicativeExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* mulOpNonterminalParser = GetNonterminal(ToUtf32("MulOp"));
        mulOpNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::PostMulOp));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromMulOp, 11, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostMulOp(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromMulOp_value = std::move(stack.top());
            context->fromMulOp = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromMulOp_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromMulOp(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        Operator fromMulOp;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::MulOpRule : public gendoc::parsing::Rule
{
public:
    MulOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::mul;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::div;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::rem;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::PmExpressionRule : public gendoc::parsing::Rule
{
public:
    PmExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PmExpressionRule>(this, &PmExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PmExpressionRule>(this, &PmExpressionRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::Postleft));
        gendoc::parsing::NonterminalParser* pmOpNonterminalParser = GetNonterminal(ToUtf32("PmOp"));
        pmOpNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::PostPmOp));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromPmOp, 12, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostPmOp(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPmOp_value = std::move(stack.top());
            context->fromPmOp = *static_cast<gendoc::parsing::ValueObject<Operator>*>(fromPmOp_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromPmOp(), fromright() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::CppObject* fromleft;
        Operator fromPmOp;
        gendoc::codedom::CppObject* fromright;
    };
};

class Expression::PmOpRule : public gendoc::parsing::Rule
{
public:
    PmOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PmOpRule>(this, &PmOpRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PmOpRule>(this, &PmOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::dotStar;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::arrowStar;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::CastExpressionRule : public gendoc::parsing::Rule
{
public:
    CastExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("ce")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("ti")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A3Action));
        gendoc::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostTypeId));
        gendoc::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostCastExpression));
        gendoc::parsing::NonterminalParser* unaryExpressionNonterminalParser = GetNonterminal(ToUtf32("UnaryExpression"));
        unaryExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostUnaryExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ce.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ce.reset(new CastExpr(context->ti.release(), context->fromCastExpression));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti.reset(context->fromTypeId);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ce.reset(context->fromUnaryExpression);
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
    void PostCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
    void PostUnaryExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUnaryExpression_value = std::move(stack.top());
            context->fromUnaryExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromUnaryExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), ce(), ti(), fromTypeId(), fromCastExpression(), fromUnaryExpression() {}
        gendoc::codedom::CppObject* value;
        std::unique_ptr<CppObject> ce;
        std::unique_ptr<CppObject> ti;
        gendoc::codedom::TypeId* fromTypeId;
        gendoc::codedom::CppObject* fromCastExpression;
        gendoc::codedom::CppObject* fromUnaryExpression;
    };
};

class Expression::UnaryExpressionRule : public gendoc::parsing::Rule
{
public:
    UnaryExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("ue")));
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
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A9Action));
        gendoc::parsing::NonterminalParser* postfixExpressionNonterminalParser = GetNonterminal(ToUtf32("PostfixExpression"));
        postfixExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostPostfixExpression));
        gendoc::parsing::NonterminalParser* postCastExpressionNonterminalParser = GetNonterminal(ToUtf32("PostCastExpression"));
        postCastExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostPostCastExpression));
        gendoc::parsing::NonterminalParser* e1NonterminalParser = GetNonterminal(ToUtf32("e1"));
        e1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste1));
        gendoc::parsing::NonterminalParser* e2NonterminalParser = GetNonterminal(ToUtf32("e2"));
        e2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste2));
        gendoc::parsing::NonterminalParser* unaryOperatorNonterminalParser = GetNonterminal(ToUtf32("UnaryOperator"));
        unaryOperatorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostUnaryOperator));
        gendoc::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostCastExpression));
        gendoc::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostTypeId));
        gendoc::parsing::NonterminalParser* e3NonterminalParser = GetNonterminal(ToUtf32("e3"));
        e3NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste3));
        gendoc::parsing::NonterminalParser* newExpressionNonterminalParser = GetNonterminal(ToUtf32("NewExpression"));
        newExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostNewExpression));
        gendoc::parsing::NonterminalParser* deleteExpressionNonterminalParser = GetNonterminal(ToUtf32("DeleteExpression"));
        deleteExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostDeleteExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ue.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromPostfixExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromPostCastExpression);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new PreIncrementExpr(context->frome1));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new PreDecrementExpr(context->frome2));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new UnaryOpExpr(context->fromUnaryOperator, context->fromCastExpression));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new SizeOfExpr(context->fromTypeId, true));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new SizeOfExpr(context->frome3, false));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromNewExpression);
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromDeleteExpression);
    }
    void PostPostfixExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPostfixExpression_value = std::move(stack.top());
            context->fromPostfixExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromPostfixExpression_value.get());
            stack.pop();
        }
    }
    void PostPostCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPostCastExpression_value = std::move(stack.top());
            context->fromPostCastExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromPostCastExpression_value.get());
            stack.pop();
        }
    }
    void Poste1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frome1_value = std::move(stack.top());
            context->frome1 = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(frome1_value.get());
            stack.pop();
        }
    }
    void Poste2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frome2_value = std::move(stack.top());
            context->frome2 = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(frome2_value.get());
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
    void PostCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
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
    void Poste3(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frome3_value = std::move(stack.top());
            context->frome3 = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(frome3_value.get());
            stack.pop();
        }
    }
    void PostNewExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNewExpression_value = std::move(stack.top());
            context->fromNewExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromNewExpression_value.get());
            stack.pop();
        }
    }
    void PostDeleteExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeleteExpression_value = std::move(stack.top());
            context->fromDeleteExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromDeleteExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), ue(), fromPostfixExpression(), fromPostCastExpression(), frome1(), frome2(), fromUnaryOperator(), fromCastExpression(), fromTypeId(), frome3(), fromNewExpression(), fromDeleteExpression() {}
        gendoc::codedom::CppObject* value;
        std::unique_ptr<CppObject> ue;
        gendoc::codedom::CppObject* fromPostfixExpression;
        gendoc::codedom::CppObject* fromPostCastExpression;
        gendoc::codedom::CppObject* frome1;
        gendoc::codedom::CppObject* frome2;
        Operator fromUnaryOperator;
        gendoc::codedom::CppObject* fromCastExpression;
        gendoc::codedom::TypeId* fromTypeId;
        gendoc::codedom::CppObject* frome3;
        gendoc::codedom::CppObject* fromNewExpression;
        gendoc::codedom::CppObject* fromDeleteExpression;
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
        context->value = Operator::plus;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::minus;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::not_;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::neg;
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
        SetValueTypeName(ToUtf32("gendoc::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("global")));
        AddLocalVariable(AttrOrVariable(ToUtf32("TypeId*"), ToUtf32("typeId")));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("parens")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A3Action));
        gendoc::parsing::NonterminalParser* newPlacementNonterminalParser = GetNonterminal(ToUtf32("NewPlacement"));
        newPlacementNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewPlacement));
        gendoc::parsing::NonterminalParser* newTypeIdNonterminalParser = GetNonterminal(ToUtf32("NewTypeId"));
        newTypeIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewTypeId));
        gendoc::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostTypeId));
        gendoc::parsing::NonterminalParser* newInitializerNonterminalParser = GetNonterminal(ToUtf32("NewInitializer"));
        newInitializerNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewInitializer));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NewExpr(context->global, context->fromNewPlacement, context->typeId, context->parens, context->fromNewInitializer);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->global = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeId = context->fromNewTypeId;
        context->parens = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeId = context->fromTypeId;
        context->parens = true;
    }
    void PostNewPlacement(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNewPlacement_value = std::move(stack.top());
            context->fromNewPlacement = *static_cast<gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>*>(fromNewPlacement_value.get());
            stack.pop();
        }
    }
    void PostNewTypeId(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNewTypeId_value = std::move(stack.top());
            context->fromNewTypeId = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>*>(fromNewTypeId_value.get());
            stack.pop();
        }
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
    void PostNewInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNewInitializer_value = std::move(stack.top());
            context->fromNewInitializer = *static_cast<gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>*>(fromNewInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), global(), typeId(), parens(), fromNewPlacement(), fromNewTypeId(), fromTypeId(), fromNewInitializer() {}
        gendoc::codedom::CppObject* value;
        bool global;
        TypeId* typeId;
        bool parens;
        std::vector<gendoc::codedom::CppObject*> fromNewPlacement;
        gendoc::codedom::TypeId* fromNewTypeId;
        gendoc::codedom::TypeId* fromTypeId;
        std::vector<gendoc::codedom::CppObject*> fromNewInitializer;
    };
};

class Expression::NewTypeIdRule : public gendoc::parsing::Rule
{
public:
    NewTypeIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::TypeId*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A3Action));
        gendoc::parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierSeq"));
        typeSpecifierSeqNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NewTypeIdRule>(this, &NewTypeIdRule::PreTypeSpecifierSeq));
        gendoc::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewTypeIdRule>(this, &NewTypeIdRule::PostTypeName));
        gendoc::parsing::NonterminalParser* newDeclaratorNonterminalParser = GetNonterminal(ToUtf32("NewDeclarator"));
        newDeclaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewTypeIdRule>(this, &NewTypeIdRule::PostNewDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti.reset(new TypeId);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ti.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti->Add(context->fromTypeName);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti->Declarator() = context->fromNewDeclarator;
    }
    void PreTypeSpecifierSeq(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>(context->ti.get())));
    }
    void PostTypeName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeName_value = std::move(stack.top());
            context->fromTypeName = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeName*>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
    void PostNewDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNewDeclarator_value = std::move(stack.top());
            context->fromNewDeclarator = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromNewDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), ti(), fromTypeName(), fromNewDeclarator() {}
        gendoc::codedom::TypeId* value;
        std::unique_ptr<TypeId> ti;
        gendoc::codedom::TypeName* fromTypeName;
        std::u32string fromNewDeclarator;
    };
};

class Expression::NewDeclaratorRule : public gendoc::parsing::Rule
{
public:
    NewDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewDeclaratorRule>(this, &NewDeclaratorRule::A0Action));
        gendoc::parsing::NonterminalParser* newDeclaratorNonterminalParser = GetNonterminal(ToUtf32("NewDeclarator"));
        newDeclaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewDeclaratorRule>(this, &NewDeclaratorRule::PostNewDeclarator));
        gendoc::parsing::NonterminalParser* directNewDeclaratorNonterminalParser = GetNonterminal(ToUtf32("DirectNewDeclarator"));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Trim(std::u32string(matchBegin, matchEnd));
    }
    void PostNewDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNewDeclarator_value = std::move(stack.top());
            context->fromNewDeclarator = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromNewDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromNewDeclarator() {}
        std::u32string value;
        std::u32string fromNewDeclarator;
    };
};

class Expression::DirectNewDeclaratorRule : public gendoc::parsing::Rule
{
public:
    DirectNewDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("e")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::A1Action));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::PostExpression));
        gendoc::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::PostConstantExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->e.reset(context->fromExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->e.reset(context->fromConstantExpression);
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): e(), fromExpression(), fromConstantExpression() {}
        std::unique_ptr<CppObject> e;
        gendoc::codedom::CppObject* fromExpression;
        gendoc::codedom::CppObject* fromConstantExpression;
    };
};

class Expression::NewPlacementRule : public gendoc::parsing::Rule
{
public:
    NewPlacementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<gendoc::codedom::CppObject*>"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewPlacementRule>(this, &NewPlacementRule::A0Action));
        gendoc::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewPlacementRule>(this, &NewPlacementRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionList;
    }
    void PostExpressionList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromExpressionList() {}
        std::vector<gendoc::codedom::CppObject*> value;
        std::vector<gendoc::codedom::CppObject*> fromExpressionList;
    };
};

class Expression::NewInitializerRule : public gendoc::parsing::Rule
{
public:
    NewInitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<gendoc::codedom::CppObject*>"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NewInitializerRule>(this, &NewInitializerRule::A0Action));
        gendoc::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NewInitializerRule>(this, &NewInitializerRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionList;
    }
    void PostExpressionList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromExpressionList() {}
        std::vector<gendoc::codedom::CppObject*> value;
        std::vector<gendoc::codedom::CppObject*> fromExpressionList;
    };
};

class Expression::DeleteExpressionRule : public gendoc::parsing::Rule
{
public:
    DeleteExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("global")));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("array")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeleteExpressionRule>(this, &DeleteExpressionRule::PostCastExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DeleteExpr(context->global, context->array, context->fromCastExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->global = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->array = true;
    }
    void PostCastExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), global(), array(), fromCastExpression() {}
        gendoc::codedom::CppObject* value;
        bool global;
        bool array;
        gendoc::codedom::CppObject* fromCastExpression;
    };
};

class Expression::PostfixExpressionRule : public gendoc::parsing::Rule
{
public:
    PostfixExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("pe")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A8Action));
        gendoc::parsing::NonterminalParser* primaryExpressionNonterminalParser = GetNonterminal(ToUtf32("PrimaryExpression"));
        primaryExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PostPrimaryExpression));
        gendoc::parsing::NonterminalParser* indexNonterminalParser = GetNonterminal(ToUtf32("index"));
        indexNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postindex));
        gendoc::parsing::NonterminalParser* invokeNonterminalParser = GetNonterminal(ToUtf32("invoke"));
        invokeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postinvoke));
        gendoc::parsing::NonterminalParser* memberNonterminalParser = GetNonterminal(ToUtf32("member"));
        memberNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postmember));
        gendoc::parsing::NonterminalParser* ptrmemberNonterminalParser = GetNonterminal(ToUtf32("ptrmember"));
        ptrmemberNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postptrmember));
        gendoc::parsing::NonterminalParser* typeSpecifierOrTypeNameNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierOrTypeName"));
        typeSpecifierOrTypeNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PostTypeSpecifierOrTypeName));
        gendoc::parsing::NonterminalParser* typeConstructionNonterminalParser = GetNonterminal(ToUtf32("typeConstruction"));
        typeConstructionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PosttypeConstruction));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->pe.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromPrimaryExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new IndexExpr(context->pe.release(), context->fromindex));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new InvokeExpr(context->pe.release(), context->frominvoke));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new MemberAccessExpr(context->pe.release(), context->frommember));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new PtrMemberAccessExpr(context->pe.release(), context->fromptrmember));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new PostIncrementExpr(context->pe.release()));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new PostDecrementExpr(context->pe.release()));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new InvokeExpr(context->fromTypeSpecifierOrTypeName, context->fromtypeConstruction));
    }
    void PostPrimaryExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPrimaryExpression_value = std::move(stack.top());
            context->fromPrimaryExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromPrimaryExpression_value.get());
            stack.pop();
        }
    }
    void Postindex(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromindex_value = std::move(stack.top());
            context->fromindex = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromindex_value.get());
            stack.pop();
        }
    }
    void Postinvoke(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frominvoke_value = std::move(stack.top());
            context->frominvoke = *static_cast<gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>*>(frominvoke_value.get());
            stack.pop();
        }
    }
    void Postmember(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frommember_value = std::move(stack.top());
            context->frommember = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::IdExpr*>*>(frommember_value.get());
            stack.pop();
        }
    }
    void Postptrmember(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromptrmember_value = std::move(stack.top());
            context->fromptrmember = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::IdExpr*>*>(fromptrmember_value.get());
            stack.pop();
        }
    }
    void PostTypeSpecifierOrTypeName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeSpecifierOrTypeName_value = std::move(stack.top());
            context->fromTypeSpecifierOrTypeName = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromTypeSpecifierOrTypeName_value.get());
            stack.pop();
        }
    }
    void PosttypeConstruction(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromtypeConstruction_value = std::move(stack.top());
            context->fromtypeConstruction = *static_cast<gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>*>(fromtypeConstruction_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), pe(), fromPrimaryExpression(), fromindex(), frominvoke(), frommember(), fromptrmember(), fromTypeSpecifierOrTypeName(), fromtypeConstruction() {}
        gendoc::codedom::CppObject* value;
        std::unique_ptr<CppObject> pe;
        gendoc::codedom::CppObject* fromPrimaryExpression;
        gendoc::codedom::CppObject* fromindex;
        std::vector<gendoc::codedom::CppObject*> frominvoke;
        gendoc::codedom::IdExpr* frommember;
        gendoc::codedom::IdExpr* fromptrmember;
        gendoc::codedom::CppObject* fromTypeSpecifierOrTypeName;
        std::vector<gendoc::codedom::CppObject*> fromtypeConstruction;
    };
};

class Expression::TypeSpecifierOrTypeNameRule : public gendoc::parsing::Rule
{
public:
    TypeSpecifierOrTypeNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::A1Action));
        gendoc::parsing::NonterminalParser* simpleTypeSpecifierNonterminalParser = GetNonterminal(ToUtf32("SimpleTypeSpecifier"));
        simpleTypeSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::PostSimpleTypeSpecifier));
        gendoc::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::PostTypeName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleTypeSpecifier;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeName;
    }
    void PostSimpleTypeSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSimpleTypeSpecifier_value = std::move(stack.top());
            context->fromSimpleTypeSpecifier = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeSpecifier*>*>(fromSimpleTypeSpecifier_value.get());
            stack.pop();
        }
    }
    void PostTypeName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeName_value = std::move(stack.top());
            context->fromTypeName = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeName*>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromSimpleTypeSpecifier(), fromTypeName() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::TypeSpecifier* fromSimpleTypeSpecifier;
        gendoc::codedom::TypeName* fromTypeName;
    };
};

class Expression::PostCastExpressionRule : public gendoc::parsing::Rule
{
public:
    PostCastExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A4Action));
        gendoc::parsing::NonterminalParser* dctNonterminalParser = GetNonterminal(ToUtf32("dct"));
        dctNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postdct));
        gendoc::parsing::NonterminalParser* dceNonterminalParser = GetNonterminal(ToUtf32("dce"));
        dceNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postdce));
        gendoc::parsing::NonterminalParser* sctNonterminalParser = GetNonterminal(ToUtf32("sct"));
        sctNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postsct));
        gendoc::parsing::NonterminalParser* sceNonterminalParser = GetNonterminal(ToUtf32("sce"));
        sceNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postsce));
        gendoc::parsing::NonterminalParser* rctNonterminalParser = GetNonterminal(ToUtf32("rct"));
        rctNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postrct));
        gendoc::parsing::NonterminalParser* rceNonterminalParser = GetNonterminal(ToUtf32("rce"));
        rceNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postrce));
        gendoc::parsing::NonterminalParser* cctNonterminalParser = GetNonterminal(ToUtf32("cct"));
        cctNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postcct));
        gendoc::parsing::NonterminalParser* cceNonterminalParser = GetNonterminal(ToUtf32("cce"));
        cceNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postcce));
        gendoc::parsing::NonterminalParser* typeidExprNonterminalParser = GetNonterminal(ToUtf32("typeidExpr"));
        typeidExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::PosttypeidExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"dynamic_cast", context->fromdct, context->fromdce);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"static_cast", context->fromsct, context->fromsce);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"reinterpret_cast", context->fromrct, context->fromrce);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"const_cast", context->fromcct, context->fromcce);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeIdExpr(context->fromtypeidExpr);
    }
    void Postdct(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromdct_value = std::move(stack.top());
            context->fromdct = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>*>(fromdct_value.get());
            stack.pop();
        }
    }
    void Postdce(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromdce_value = std::move(stack.top());
            context->fromdce = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromdce_value.get());
            stack.pop();
        }
    }
    void Postsct(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromsct_value = std::move(stack.top());
            context->fromsct = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>*>(fromsct_value.get());
            stack.pop();
        }
    }
    void Postsce(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromsce_value = std::move(stack.top());
            context->fromsce = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromsce_value.get());
            stack.pop();
        }
    }
    void Postrct(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromrct_value = std::move(stack.top());
            context->fromrct = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>*>(fromrct_value.get());
            stack.pop();
        }
    }
    void Postrce(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromrce_value = std::move(stack.top());
            context->fromrce = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromrce_value.get());
            stack.pop();
        }
    }
    void Postcct(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromcct_value = std::move(stack.top());
            context->fromcct = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>*>(fromcct_value.get());
            stack.pop();
        }
    }
    void Postcce(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromcce_value = std::move(stack.top());
            context->fromcce = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromcce_value.get());
            stack.pop();
        }
    }
    void PosttypeidExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromtypeidExpr_value = std::move(stack.top());
            context->fromtypeidExpr = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::CppObject*>*>(fromtypeidExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromdct(), fromdce(), fromsct(), fromsce(), fromrct(), fromrce(), fromcct(), fromcce(), fromtypeidExpr() {}
        gendoc::codedom::CppObject* value;
        gendoc::codedom::TypeId* fromdct;
        gendoc::codedom::CppObject* fromdce;
        gendoc::codedom::TypeId* fromsct;
        gendoc::codedom::CppObject* fromsce;
        gendoc::codedom::TypeId* fromrct;
        gendoc::codedom::CppObject* fromrce;
        gendoc::codedom::TypeId* fromcct;
        gendoc::codedom::CppObject* fromcce;
        gendoc::codedom::CppObject* fromtypeidExpr;
    };
};

class Expression::ExpressionListRule : public gendoc::parsing::Rule
{
public:
    ExpressionListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<gendoc::codedom::CppObject*>"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::vector<gendoc::codedom::CppObject*>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExpressionListRule>(this, &ExpressionListRule::A0Action));
        gendoc::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExpressionListRule>(this, &ExpressionListRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.push_back(context->fromAssignmentExpression);
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromAssignmentExpression() {}
        std::vector<gendoc::codedom::CppObject*> value;
        gendoc::codedom::CppObject* fromAssignmentExpression;
    };
};

class Expression::PrimaryExpressionRule : public gendoc::parsing::Rule
{
public:
    PrimaryExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("pe")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A4Action));
        gendoc::parsing::NonterminalParser* literalNonterminalParser = GetNonterminal(ToUtf32("Literal"));
        literalNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostLiteral));
        gendoc::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostExpression));
        gendoc::parsing::NonterminalParser* idExpressionNonterminalParser = GetNonterminal(ToUtf32("IdExpression"));
        idExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostIdExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->pe.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromLiteral);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new ThisAccess);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromExpression);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromIdExpression);
    }
    void PostLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLiteral_value = std::move(stack.top());
            context->fromLiteral = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Literal*>*>(fromLiteral_value.get());
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
    void PostIdExpression(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIdExpression_value = std::move(stack.top());
            context->fromIdExpression = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::IdExpr*>*>(fromIdExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), pe(), fromLiteral(), fromExpression(), fromIdExpression() {}
        gendoc::codedom::CppObject* value;
        std::unique_ptr<CppObject> pe;
        gendoc::codedom::Literal* fromLiteral;
        gendoc::codedom::CppObject* fromExpression;
        gendoc::codedom::IdExpr* fromIdExpression;
    };
};

class Expression::IdExpressionRule : public gendoc::parsing::Rule
{
public:
    IdExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::IdExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::IdExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A2Action));
        gendoc::parsing::NonterminalParser* id1NonterminalParser = GetNonterminal(ToUtf32("id1"));
        id1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::Postid1));
        gendoc::parsing::NonterminalParser* id2NonterminalParser = GetNonterminal(ToUtf32("id2"));
        id2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::Postid2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdExpr(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdExpr(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdExpr(std::u32string(matchBegin, matchEnd));
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
        Context(): value(), fromid1(), fromid2() {}
        gendoc::codedom::IdExpr* value;
        std::u32string fromid1;
        std::u32string fromid2;
    };
};

void Expression::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.code.Literal"));
    if (!grammar0)
    {
        grammar0 = gendoc::code::Literal::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.code.Declaration"));
    if (!grammar1)
    {
        grammar1 = gendoc::code::Declaration::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.code.Identifier"));
    if (!grammar2)
    {
        grammar2 = gendoc::code::Identifier::Create(pd);
    }
    AddGrammarReference(grammar2);
    gendoc::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("gendoc.code.Declarator"));
    if (!grammar3)
    {
        grammar3 = gendoc::code::Declarator::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void Expression::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Literal"), this, ToUtf32("Literal.Literal")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("SimpleTypeSpecifier"), this, ToUtf32("Declaration.SimpleTypeSpecifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Identifier.QualifiedId")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeId"), this, ToUtf32("Declarator.TypeId")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeSpecifierSeq"), this, ToUtf32("Declarator.TypeSpecifierSeq")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeName"), this, ToUtf32("Declaration.TypeName")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("PtrOperator"), this, ToUtf32("Declarator.PtrOperator")));
    AddRule(new ExpressionRule(ToUtf32("Expression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AssignmentExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser(','),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AssignmentExpression"), 0))))))));
    AddRule(new ConstantExpressionRule(ToUtf32("ConstantExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))));
    AddRule(new AssignmentExpressionRule(ToUtf32("AssignmentExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("LogicalOrExpression"), ToUtf32("LogicalOrExpression"), 0)),
                                new gendoc::parsing::NonterminalParser(ToUtf32("AssingmentOp"), ToUtf32("AssingmentOp"), 0)),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0))))),
                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ConditionalExpression"), ToUtf32("ConditionalExpression"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ThrowExpression"), ToUtf32("ThrowExpression"), 0)))));
    AddRule(new AssingmentOpRule(ToUtf32("AssingmentOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
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
                                                        new gendoc::parsing::CharParser('='),
                                                        new gendoc::parsing::StringParser(ToUtf32("*="))),
                                                    new gendoc::parsing::StringParser(ToUtf32("/="))),
                                                new gendoc::parsing::StringParser(ToUtf32("%="))),
                                            new gendoc::parsing::StringParser(ToUtf32("+="))),
                                        new gendoc::parsing::StringParser(ToUtf32("-="))),
                                    new gendoc::parsing::StringParser(ToUtf32(">>="))),
                                new gendoc::parsing::StringParser(ToUtf32("<<="))),
                            new gendoc::parsing::StringParser(ToUtf32("&="))),
                        new gendoc::parsing::StringParser(ToUtf32("^="))),
                    new gendoc::parsing::StringParser(ToUtf32("|=")))))));
    AddRule(new ThrowExpressionRule(ToUtf32("ThrowExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::KeywordParser(ToUtf32("throw")),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0)))))));
    AddRule(new ConditionalExpressionRule(ToUtf32("ConditionalExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("LogicalOrExpression"), 0)),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::CharParser('?'),
                                        new gendoc::parsing::ExpectationParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                                    new gendoc::parsing::CharParser(':')),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0))))))))));
    AddRule(new LogicalOrExpressionRule(ToUtf32("LogicalOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("LogicalAndExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::StringParser(ToUtf32("||")),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("LogicalAndExpression"), 0)))))))));
    AddRule(new LogicalAndExpressionRule(ToUtf32("LogicalAndExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("InclusiveOrExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::StringParser(ToUtf32("&&")),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("InclusiveOrExpression"), 0)))))))));
    AddRule(new InclusiveOrExpressionRule(ToUtf32("InclusiveOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ExclusiveOrExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::DifferenceParser(
                                new gendoc::parsing::CharParser('|'),
                                new gendoc::parsing::StringParser(ToUtf32("||")))),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ExclusiveOrExpression"), 0)))))))));
    AddRule(new ExclusiveOrExpressionRule(ToUtf32("ExclusiveOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AndExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('^'),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AndExpression"), 0)))))))));
    AddRule(new AndExpressionRule(ToUtf32("AndExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("EqualityExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::DifferenceParser(
                                new gendoc::parsing::CharParser('&'),
                                new gendoc::parsing::StringParser(ToUtf32("&&")))),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("EqualityExpression"), 0)))))))));
    AddRule(new EqualityExpressionRule(ToUtf32("EqualityExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("RelationalExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("EqOp"), ToUtf32("EqOp"), 0),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelationalExpression"), 0)))))))));
    AddRule(new EqOpRule(ToUtf32("EqOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::StringParser(ToUtf32("=="))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::StringParser(ToUtf32("!="))))));
    AddRule(new RelationalExpressionRule(ToUtf32("RelationalExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ShiftExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("RelOp"), ToUtf32("RelOp"), 0),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ShiftExpression"), 0)))))))));
    AddRule(new RelOpRule(ToUtf32("RelOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::StringParser(ToUtf32("<="))),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::StringParser(ToUtf32(">=")))),
                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::DifferenceParser(
                            new gendoc::parsing::CharParser('<'),
                            new gendoc::parsing::StringParser(ToUtf32("<<")))))),
            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::DifferenceParser(
                        new gendoc::parsing::CharParser('>'),
                        new gendoc::parsing::StringParser(ToUtf32(">>"))))))));
    AddRule(new ShiftExpressionRule(ToUtf32("ShiftExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AdditiveExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("ShiftOp"), ToUtf32("ShiftOp"), 0),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AdditiveExpression"), 0)))))))));
    AddRule(new ShiftOpRule(ToUtf32("ShiftOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::DifferenceParser(
                        new gendoc::parsing::StringParser(ToUtf32("<<")),
                        new gendoc::parsing::StringParser(ToUtf32("<<="))))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::DifferenceParser(
                        new gendoc::parsing::StringParser(ToUtf32(">>")),
                        new gendoc::parsing::StringParser(ToUtf32(">>="))))))));
    AddRule(new AdditiveExpressionRule(ToUtf32("AdditiveExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("MultiplicativeExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("AddOp"), ToUtf32("AddOp"), 0),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("MultiplicativeExpression"), 0)))))))));
    AddRule(new AddOpRule(ToUtf32("AddOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::DifferenceParser(
                        new gendoc::parsing::CharParser('+'),
                        new gendoc::parsing::StringParser(ToUtf32("+="))))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::DifferenceParser(
                        new gendoc::parsing::CharParser('-'),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::StringParser(ToUtf32("-=")),
                                new gendoc::parsing::StringParser(ToUtf32("->"))))))))));
    AddRule(new MultiplicativeExpressionRule(ToUtf32("MultiplicativeExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("PmExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("MulOp"), ToUtf32("MulOp"), 0),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("PmExpression"), 0)))))))));
    AddRule(new MulOpRule(ToUtf32("MulOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                            new gendoc::parsing::CharParser('/'),
                            new gendoc::parsing::StringParser(ToUtf32("/=")))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::DifferenceParser(
                        new gendoc::parsing::CharParser('%'),
                        new gendoc::parsing::StringParser(ToUtf32("%="))))))));
    AddRule(new PmExpressionRule(ToUtf32("PmExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("CastExpression"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("PmOp"), ToUtf32("PmOp"), 0),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("CastExpression"), 0)))))))));
    AddRule(new PmOpRule(ToUtf32("PmOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::StringParser(ToUtf32(".*"))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::StringParser(ToUtf32("->*"))))));
    AddRule(new CastExpressionRule(ToUtf32("CastExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::CharParser('('),
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0))),
                                    new gendoc::parsing::CharParser(')')),
                                new gendoc::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 0)))),
                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("UnaryExpression"), ToUtf32("UnaryExpression"), 0)))))));
    AddRule(new UnaryExpressionRule(ToUtf32("UnaryExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::AlternativeParser(
                                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("PostfixExpression"), ToUtf32("PostfixExpression"), 0)),
                                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("PostCastExpression"), ToUtf32("PostCastExpression"), 0)))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                new gendoc::parsing::GroupingParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::StringParser(ToUtf32("++")),
                                                        new gendoc::parsing::ExpectationParser(
                                                            new gendoc::parsing::NonterminalParser(ToUtf32("e1"), ToUtf32("UnaryExpression"), 0)))))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::StringParser(ToUtf32("--")),
                                                    new gendoc::parsing::ExpectationParser(
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("e2"), ToUtf32("UnaryExpression"), 0)))))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::NonterminalParser(ToUtf32("UnaryOperator"), ToUtf32("UnaryOperator"), 0),
                                                new gendoc::parsing::ExpectationParser(
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 0)))))),
                                new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::KeywordParser(ToUtf32("sizeof")),
                                                    new gendoc::parsing::CharParser('(')),
                                                new gendoc::parsing::ExpectationParser(
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0))),
                                            new gendoc::parsing::ExpectationParser(
                                                new gendoc::parsing::CharParser(')')))))),
                            new gendoc::parsing::ActionParser(ToUtf32("A7"),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::KeywordParser(ToUtf32("sizeof")),
                                        new gendoc::parsing::ExpectationParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("e3"), ToUtf32("UnaryExpression"), 0)))))),
                        new gendoc::parsing::ActionParser(ToUtf32("A8"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("NewExpression"), ToUtf32("NewExpression"), 0))),
                    new gendoc::parsing::ActionParser(ToUtf32("A9"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("DeleteExpression"), ToUtf32("DeleteExpression"), 0)))))));
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
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::DifferenceParser(
                            new gendoc::parsing::CharParser('!'),
                            new gendoc::parsing::StringParser(ToUtf32("!=")))))),
            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                new gendoc::parsing::CharParser('~')))));
    AddRule(new NewExpressionRule(ToUtf32("NewExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::OptionalParser(
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                            new gendoc::parsing::StringParser(ToUtf32("::"))))),
                                new gendoc::parsing::KeywordParser(ToUtf32("new"))),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("NewPlacement"), ToUtf32("NewPlacement"), 0))),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("NewTypeId"), ToUtf32("NewTypeId"), 0)),
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::CharParser('('),
                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0))),
                                    new gendoc::parsing::CharParser(')'))))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("NewInitializer"), ToUtf32("NewInitializer"), 0)))))));
    AddRule(new NewTypeIdRule(ToUtf32("NewTypeId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::EmptyParser()),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("TypeSpecifierSeq"), ToUtf32("TypeSpecifierSeq"), 1),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0)))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("NewDeclarator"), ToUtf32("NewDeclarator"), 0))))))))));
    AddRule(new NewDeclaratorRule(ToUtf32("NewDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("PtrOperator"), ToUtf32("PtrOperator"), 0),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("NewDeclarator"), ToUtf32("NewDeclarator"), 0))),
                    new gendoc::parsing::NonterminalParser(ToUtf32("DirectNewDeclarator"), ToUtf32("DirectNewDeclarator"), 0))))));
    AddRule(new DirectNewDeclaratorRule(ToUtf32("DirectNewDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('['),
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                new gendoc::parsing::CharParser(']')),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('['),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 0))),
                        new gendoc::parsing::CharParser(']')))))));
    AddRule(new NewPlacementRule(ToUtf32("NewPlacement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('('),
                        new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0)),
                    new gendoc::parsing::CharParser(')'))))));
    AddRule(new NewInitializerRule(ToUtf32("NewInitializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('('),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0))),
                    new gendoc::parsing::CharParser(')'))))));
    AddRule(new DeleteExpressionRule(ToUtf32("DeleteExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::StringParser(ToUtf32("::"))))),
                            new gendoc::parsing::KeywordParser(ToUtf32("delete"))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::CharParser('['),
                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                        new gendoc::parsing::CharParser(']')))))),
                    new gendoc::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 0))))));
    AddRule(new PostfixExpressionRule(ToUtf32("PostfixExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("PrimaryExpression"), ToUtf32("PrimaryExpression"), 0)),
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::AlternativeParser(
                                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                        new gendoc::parsing::GroupingParser(
                                                            new gendoc::parsing::SequenceParser(
                                                                new gendoc::parsing::SequenceParser(
                                                                    new gendoc::parsing::CharParser('['),
                                                                    new gendoc::parsing::ExpectationParser(
                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("index"), ToUtf32("Expression"), 0))),
                                                                new gendoc::parsing::ExpectationParser(
                                                                    new gendoc::parsing::CharParser(']'))))),
                                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                        new gendoc::parsing::GroupingParser(
                                                            new gendoc::parsing::SequenceParser(
                                                                new gendoc::parsing::SequenceParser(
                                                                    new gendoc::parsing::CharParser('('),
                                                                    new gendoc::parsing::OptionalParser(
                                                                        new gendoc::parsing::GroupingParser(
                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("invoke"), ToUtf32("ExpressionList"), 0)))),
                                                                new gendoc::parsing::ExpectationParser(
                                                                    new gendoc::parsing::CharParser(')')))))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                    new gendoc::parsing::GroupingParser(
                                                        new gendoc::parsing::SequenceParser(
                                                            new gendoc::parsing::GroupingParser(
                                                                new gendoc::parsing::DifferenceParser(
                                                                    new gendoc::parsing::CharParser('.'),
                                                                    new gendoc::parsing::StringParser(ToUtf32(".*")))),
                                                            new gendoc::parsing::ExpectationParser(
                                                                new gendoc::parsing::NonterminalParser(ToUtf32("member"), ToUtf32("IdExpression"), 0)))))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                                new gendoc::parsing::GroupingParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::GroupingParser(
                                                            new gendoc::parsing::DifferenceParser(
                                                                new gendoc::parsing::StringParser(ToUtf32("->")),
                                                                new gendoc::parsing::StringParser(ToUtf32("->*")))),
                                                        new gendoc::parsing::ExpectationParser(
                                                            new gendoc::parsing::NonterminalParser(ToUtf32("ptrmember"), ToUtf32("IdExpression"), 0)))))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                            new gendoc::parsing::StringParser(ToUtf32("++")))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A7"),
                                        new gendoc::parsing::StringParser(ToUtf32("--"))))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A8"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("TypeSpecifierOrTypeName"), ToUtf32("TypeSpecifierOrTypeName"), 0),
                                        new gendoc::parsing::CharParser('(')),
                                    new gendoc::parsing::OptionalParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("typeConstruction"), ToUtf32("ExpressionList"), 0))),
                                new gendoc::parsing::CharParser(')')))))))));
    AddRule(new TypeSpecifierOrTypeNameRule(ToUtf32("TypeSpecifierOrTypeName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("SimpleTypeSpecifier"), ToUtf32("SimpleTypeSpecifier"), 0)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0)))));
    AddRule(new PostCastExpressionRule(ToUtf32("PostCastExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::KeywordParser(ToUtf32("dynamic_cast")),
                                                        new gendoc::parsing::ExpectationParser(
                                                            new gendoc::parsing::CharParser('<'))),
                                                    new gendoc::parsing::ExpectationParser(
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("dct"), ToUtf32("TypeId"), 0))),
                                                new gendoc::parsing::ExpectationParser(
                                                    new gendoc::parsing::CharParser('>'))),
                                            new gendoc::parsing::ExpectationParser(
                                                new gendoc::parsing::CharParser('('))),
                                        new gendoc::parsing::ExpectationParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("dce"), ToUtf32("Expression"), 0))),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser(')'))))),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::KeywordParser(ToUtf32("static_cast")),
                                                        new gendoc::parsing::ExpectationParser(
                                                            new gendoc::parsing::CharParser('<'))),
                                                    new gendoc::parsing::ExpectationParser(
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("sct"), ToUtf32("TypeId"), 0))),
                                                new gendoc::parsing::ExpectationParser(
                                                    new gendoc::parsing::CharParser('>'))),
                                            new gendoc::parsing::ExpectationParser(
                                                new gendoc::parsing::CharParser('('))),
                                        new gendoc::parsing::ExpectationParser(
                                            new gendoc::parsing::NonterminalParser(ToUtf32("sce"), ToUtf32("Expression"), 0))),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser(')')))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::KeywordParser(ToUtf32("reinterpret_cast")),
                                                    new gendoc::parsing::ExpectationParser(
                                                        new gendoc::parsing::CharParser('<'))),
                                                new gendoc::parsing::ExpectationParser(
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("rct"), ToUtf32("TypeId"), 0))),
                                            new gendoc::parsing::ExpectationParser(
                                                new gendoc::parsing::CharParser('>'))),
                                        new gendoc::parsing::ExpectationParser(
                                            new gendoc::parsing::CharParser('('))),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("rce"), ToUtf32("Expression"), 0))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser(')')))))),
                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::KeywordParser(ToUtf32("const_cast")),
                                                new gendoc::parsing::ExpectationParser(
                                                    new gendoc::parsing::CharParser('<'))),
                                            new gendoc::parsing::ExpectationParser(
                                                new gendoc::parsing::NonterminalParser(ToUtf32("cct"), ToUtf32("TypeId"), 0))),
                                        new gendoc::parsing::ExpectationParser(
                                            new gendoc::parsing::CharParser('>'))),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser('('))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("cce"), ToUtf32("Expression"), 0))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::CharParser(')')))))),
            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("typeid")),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser('('))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("typeidExpr"), ToUtf32("Expression"), 0))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser(')'))))))));
    AddRule(new ExpressionListRule(ToUtf32("ExpressionList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ListParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0)),
            new gendoc::parsing::CharParser(','))));
    AddRule(new PrimaryExpressionRule(ToUtf32("PrimaryExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Literal"), ToUtf32("Literal"), 0)),
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::KeywordParser(ToUtf32("this")))),
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser('('),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0)),
                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser(')'))))),
                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("IdExpression"), ToUtf32("IdExpression"), 0)))))));
    AddRule(new IdExpressionRule(ToUtf32("IdExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("id1"), ToUtf32("QualifiedId"), 0),
                                new gendoc::parsing::StringParser(ToUtf32("::"))),
                            new gendoc::parsing::NonterminalParser(ToUtf32("OperatorFunctionId"), ToUtf32("OperatorFunctionId"), 0)))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::TokenParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::StringParser(ToUtf32("::"))),
                            new gendoc::parsing::NonterminalParser(ToUtf32("OperatorFunctionId"), ToUtf32("OperatorFunctionId"), 0))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("id2"), ToUtf32("QualifiedId"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("OperatorFunctionId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::KeywordParser(ToUtf32("operator")),
            new gendoc::parsing::NonterminalParser(ToUtf32("Operator"), ToUtf32("Operator"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Operator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                                                                            new gendoc::parsing::SequenceParser(
                                                                                                                                                                                new gendoc::parsing::CharParser('['),
                                                                                                                                                                                new gendoc::parsing::CharParser(']')),
                                                                                                                                                                            new gendoc::parsing::SequenceParser(
                                                                                                                                                                                new gendoc::parsing::CharParser('('),
                                                                                                                                                                                new gendoc::parsing::CharParser(')'))),
                                                                                                                                                                        new gendoc::parsing::StringParser(ToUtf32("->*"))),
                                                                                                                                                                    new gendoc::parsing::StringParser(ToUtf32("->"))),
                                                                                                                                                                new gendoc::parsing::CharParser(',')),
                                                                                                                                                            new gendoc::parsing::StringParser(ToUtf32("--"))),
                                                                                                                                                        new gendoc::parsing::StringParser(ToUtf32("++"))),
                                                                                                                                                    new gendoc::parsing::StringParser(ToUtf32("||"))),
                                                                                                                                                new gendoc::parsing::StringParser(ToUtf32("&&"))),
                                                                                                                                            new gendoc::parsing::StringParser(ToUtf32("<="))),
                                                                                                                                        new gendoc::parsing::StringParser(ToUtf32(">="))),
                                                                                                                                    new gendoc::parsing::StringParser(ToUtf32("!="))),
                                                                                                                                new gendoc::parsing::StringParser(ToUtf32("=="))),
                                                                                                                            new gendoc::parsing::StringParser(ToUtf32("<<="))),
                                                                                                                        new gendoc::parsing::StringParser(ToUtf32(">>="))),
                                                                                                                    new gendoc::parsing::StringParser(ToUtf32("<<"))),
                                                                                                                new gendoc::parsing::StringParser(ToUtf32(">>"))),
                                                                                                            new gendoc::parsing::StringParser(ToUtf32("|="))),
                                                                                                        new gendoc::parsing::StringParser(ToUtf32("&="))),
                                                                                                    new gendoc::parsing::StringParser(ToUtf32("^="))),
                                                                                                new gendoc::parsing::StringParser(ToUtf32("%="))),
                                                                                            new gendoc::parsing::StringParser(ToUtf32("/="))),
                                                                                        new gendoc::parsing::StringParser(ToUtf32("*="))),
                                                                                    new gendoc::parsing::StringParser(ToUtf32("-="))),
                                                                                new gendoc::parsing::StringParser(ToUtf32("+="))),
                                                                            new gendoc::parsing::CharParser('<')),
                                                                        new gendoc::parsing::CharParser('>')),
                                                                    new gendoc::parsing::CharParser('=')),
                                                                new gendoc::parsing::CharParser('!')),
                                                            new gendoc::parsing::CharParser('~')),
                                                        new gendoc::parsing::CharParser('|')),
                                                    new gendoc::parsing::CharParser('&')),
                                                new gendoc::parsing::CharParser('^')),
                                            new gendoc::parsing::CharParser('%')),
                                        new gendoc::parsing::CharParser('/')),
                                    new gendoc::parsing::CharParser('*')),
                                new gendoc::parsing::CharParser('-')),
                            new gendoc::parsing::CharParser('+')),
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("new")),
                                new gendoc::parsing::CharParser('[')),
                            new gendoc::parsing::CharParser(']'))),
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("delete")),
                            new gendoc::parsing::CharParser('[')),
                        new gendoc::parsing::CharParser(']'))),
                new gendoc::parsing::KeywordParser(ToUtf32("new"))),
            new gendoc::parsing::KeywordParser(ToUtf32("delete")))));
}

} } // namespace gendoc.code
