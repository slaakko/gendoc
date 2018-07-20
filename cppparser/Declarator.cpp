#include "Declarator.hpp"
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
#include <gendoc/cppparser/Function.hpp>
#include <gendoc/cppparser/TypeExpr.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Declarator* Declarator::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Declarator* Declarator::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Declarator* grammar(new Declarator(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Declarator::Declarator(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Declarator"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

Node* Declarator::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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

class Declarator::InitDeclaratorRule : public gendoc::parsing::Rule
{
public:
    InitDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitDeclaratorRule>(this, &InitDeclaratorRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitDeclaratorRule>(this, &InitDeclaratorRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitDeclaratorRule>(this, &InitDeclaratorRule::A2Action));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PreDeclarator));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PostDeclarator));
        gendoc::parsing::NonterminalParser* initializerNonterminalParser = GetNonterminal(ToUtf32("Initializer"));
        initializerNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PreInitializer));
        initializerNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PostInitializer));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->declarator.reset(context->fromDeclarator);
        context->s = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->value = new InitDeclaratorNode(context->s, context->declarator.release(), context->fromInitializer);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->declarator.release();
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
    void PreInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromInitializer_value = std::move(stack.top());
            context->fromInitializer = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), declarator(), s(), fromDeclarator(), fromInitializer() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> declarator;
        Span s;
        Node* fromDeclarator;
        Node* fromInitializer;
    };
};

class Declarator::DeclaratorRule : public gendoc::parsing::Rule
{
public:
    DeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("node")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A7Action));
        gendoc::parsing::NonterminalParser* idExpressionNonterminalParser = GetNonterminal(ToUtf32("IdExpression"));
        idExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclaratorRule>(this, &DeclaratorRule::PreIdExpression));
        idExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::PostIdExpression));
        gendoc::parsing::NonterminalParser* id1NonterminalParser = GetNonterminal(ToUtf32("id1"));
        id1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::Postid1));
        gendoc::parsing::NonterminalParser* nestedNameSpecifierNonterminalParser = GetNonterminal(ToUtf32("NestedNameSpecifier"));
        nestedNameSpecifierNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclaratorRule>(this, &DeclaratorRule::PreNestedNameSpecifier));
        nestedNameSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::PostNestedNameSpecifier));
        gendoc::parsing::NonterminalParser* id2NonterminalParser = GetNonterminal(ToUtf32("id2"));
        id2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::Postid2));
        gendoc::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclaratorRule>(this, &DeclaratorRule::PreParameterList));
        parameterListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::PostParameterList));
        gendoc::parsing::NonterminalParser* cVSpecifierSequenceNonterminalParser = GetNonterminal(ToUtf32("CVSpecifierSequence"));
        cVSpecifierSequenceNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::PostCVSpecifierSequence));
        gendoc::parsing::NonterminalParser* sizeNonterminalParser = GetNonterminal(ToUtf32("size"));
        sizeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclaratorRule>(this, &DeclaratorRule::Presize));
        sizeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::Postsize));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->node.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(new IdDeclaratorNode(span, context->fromIdExpression));
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(new FunctionPtrIdNode(span, context->fromid1));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(new MemberFunctionPtrIdNode(span, context->node.release(), context->fromid2));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(context->fromNestedNameSpecifier);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->node.reset(new FunctionDeclaratorNode(context->s, context->node.release(), context->fromParameterList, context->fromCVSpecifierSequence));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        pass = !context->ctx->InFunctionBody();
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->node.reset(new ArrayDeclaratorNode(context->s, context->node.release(), context->fromsize));
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
    void PreNestedNameSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostNestedNameSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNestedNameSpecifier_value = std::move(stack.top());
            context->fromNestedNameSpecifier = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromNestedNameSpecifier_value.get());
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
    void PreParameterList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostParameterList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromParameterList_value = std::move(stack.top());
            context->fromParameterList = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromParameterList_value.get());
            stack.pop();
        }
    }
    void PostCVSpecifierSequence(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCVSpecifierSequence_value = std::move(stack.top());
            context->fromCVSpecifierSequence = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromCVSpecifierSequence_value.get());
            stack.pop();
        }
    }
    void Presize(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postsize(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromsize_value = std::move(stack.top());
            context->fromsize = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromsize_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), node(), s(), fromIdExpression(), fromid1(), fromNestedNameSpecifier(), fromid2(), fromParameterList(), fromCVSpecifierSequence(), fromsize() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> node;
        Span s;
        Node* fromIdExpression;
        std::u32string fromid1;
        Node* fromNestedNameSpecifier;
        std::u32string fromid2;
        Node* fromParameterList;
        Specifier fromCVSpecifierSequence;
        Node* fromsize;
    };
};

class Declarator::InitializerRule : public gendoc::parsing::Rule
{
public:
    InitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A2Action));
        gendoc::parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal(ToUtf32("InitializerClause"));
        initializerClauseNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitializerRule>(this, &InitializerRule::PreInitializerClause));
        initializerClauseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostInitializerClause));
        gendoc::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitializerRule>(this, &InitializerRule::PreExpressionList));
        expressionListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostExpressionList));
        gendoc::parsing::NonterminalParser* bracedInitializerListNonterminalParser = GetNonterminal(ToUtf32("BracedInitializerList"));
        bracedInitializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitializerRule>(this, &InitializerRule::PreBracedInitializerList));
        bracedInitializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostBracedInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AssignmentInitializerNode(span, context->fromInitializerClause);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExpressionListInitializerNode(span, context->fromExpressionList);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBracedInitializerList;
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
        Context(): ctx(), value(), fromInitializerClause(), fromExpressionList(), fromBracedInitializerList() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromInitializerClause;
        Node* fromExpressionList;
        BracedInitializerListNode* fromBracedInitializerList;
    };
};

class Declarator::BraceOrEqualInitializerRule : public gendoc::parsing::Rule
{
public:
    BraceOrEqualInitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BraceOrEqualInitializerRule>(this, &BraceOrEqualInitializerRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BraceOrEqualInitializerRule>(this, &BraceOrEqualInitializerRule::A1Action));
        gendoc::parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal(ToUtf32("InitializerClause"));
        initializerClauseNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BraceOrEqualInitializerRule>(this, &BraceOrEqualInitializerRule::PreInitializerClause));
        initializerClauseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BraceOrEqualInitializerRule>(this, &BraceOrEqualInitializerRule::PostInitializerClause));
        gendoc::parsing::NonterminalParser* bracedInitializerListNonterminalParser = GetNonterminal(ToUtf32("BracedInitializerList"));
        bracedInitializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BraceOrEqualInitializerRule>(this, &BraceOrEqualInitializerRule::PreBracedInitializerList));
        bracedInitializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BraceOrEqualInitializerRule>(this, &BraceOrEqualInitializerRule::PostBracedInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AssignmentInitializerNode(span, context->fromInitializerClause);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBracedInitializerList;
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromInitializerClause(), fromBracedInitializerList() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromInitializerClause;
        BracedInitializerListNode* fromBracedInitializerList;
    };
};

class Declarator::BracedInitializerListRule : public gendoc::parsing::Rule
{
public:
    BracedInitializerListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("BracedInitializerListNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<BracedInitializerListNode>"), ToUtf32("node")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<BracedInitializerListNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BracedInitializerListRule>(this, &BracedInitializerListRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BracedInitializerListRule>(this, &BracedInitializerListRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BracedInitializerListRule>(this, &BracedInitializerListRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BracedInitializerListRule>(this, &BracedInitializerListRule::A3Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BracedInitializerListRule>(this, &BracedInitializerListRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BracedInitializerListRule>(this, &BracedInitializerListRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BracedInitializerListRule>(this, &BracedInitializerListRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BracedInitializerListRule>(this, &BracedInitializerListRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(new BracedInitializerListNode(span));
        context->s = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->node->AddInitializer(context->s, context->fromleft);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->node->AddInitializer(context->s, context->fromright);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node->SetEndBraceSpan(span);
        context->node->SetSpanEnd(span.End());
        context->value = context->node.release();
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
        Context(): ctx(), value(), node(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        BracedInitializerListNode* value;
        std::unique_ptr<BracedInitializerListNode> node;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Declarator::InitializerClauseRule : public gendoc::parsing::Rule
{
public:
    InitializerClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A1Action));
        gendoc::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitializerClauseRule>(this, &InitializerClauseRule::PreAssignmentExpression));
        assignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerClauseRule>(this, &InitializerClauseRule::PostAssignmentExpression));
        gendoc::parsing::NonterminalParser* bracedInitializerListNonterminalParser = GetNonterminal(ToUtf32("BracedInitializerList"));
        bracedInitializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitializerClauseRule>(this, &InitializerClauseRule::PreBracedInitializerList));
        bracedInitializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerClauseRule>(this, &InitializerClauseRule::PostBracedInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExpressionInitializerNode(span, context->fromAssignmentExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBracedInitializerList;
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
        Context(): ctx(), value(), fromAssignmentExpression(), fromBracedInitializerList() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromAssignmentExpression;
        BracedInitializerListNode* fromBracedInitializerList;
    };
};

class Declarator::InitializerListRule : public gendoc::parsing::Rule
{
public:
    InitializerListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("node")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerListRule>(this, &InitializerListRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerListRule>(this, &InitializerListRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerListRule>(this, &InitializerListRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitializerListRule>(this, &InitializerListRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerListRule>(this, &InitializerListRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitializerListRule>(this, &InitializerListRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerListRule>(this, &InitializerListRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->node.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->node.reset(new ExpressionSequenceNode(context->s, context->node.release(), context->fromright));
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
        Context(): ctx(), value(), node(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> node;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

void Declarator::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Identifier"));
    if (!grammar0)
    {
        grammar0 = gendoc::cppparser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Expression"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::Expression::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Function"));
    if (!grammar2)
    {
        grammar2 = gendoc::cppparser::Function::Create(pd);
    }
    AddGrammarReference(grammar2);
    gendoc::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("gendoc.cppparser.TypeExpr"));
    if (!grammar3)
    {
        grammar3 = gendoc::cppparser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void Declarator::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ExpressionList"), this, ToUtf32("Expression.ExpressionList")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("NestedNameSpecifier"), this, ToUtf32("Identifier.NestedNameSpecifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ConstantExpression"), this, ToUtf32("Expression.ConstantExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("AssignmentExpression"), this, ToUtf32("Expression.AssignmentExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("IdExpression"), this, ToUtf32("Expression.IdExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ParameterList"), this, ToUtf32("Function.ParameterList")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("CVSpecifierSequence"), this, ToUtf32("TypeExpr.CVSpecifierSequence")));
    AddRule(new InitDeclaratorRule(ToUtf32("InitDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 1)),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Initializer"), ToUtf32("Initializer"), 1)),
                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                        new gendoc::parsing::EmptyParser()))))));
    AddRule(new DeclaratorRule(ToUtf32("Declarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("IdExpression"), ToUtf32("IdExpression"), 1)),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::CharParser('('),
                                                    new gendoc::parsing::CharParser('*')),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("id1"), ToUtf32("Identifier"), 0)),
                                            new gendoc::parsing::CharParser(')'))))),
                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::CharParser('('),
                                                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("NestedNameSpecifier"), ToUtf32("NestedNameSpecifier"), 1))),
                                                new gendoc::parsing::CharParser('*')),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("id2"), ToUtf32("Identifier"), 0)),
                                        new gendoc::parsing::CharParser(')')))))),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                                new gendoc::parsing::EmptyParser()),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 1)),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("CVSpecifierSequence"), ToUtf32("CVSpecifierSequence"), 0)))),
                            new gendoc::parsing::KleeneStarParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A7"),
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::CharParser('['),
                                                    new gendoc::parsing::OptionalParser(
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("size"), ToUtf32("ConstantExpression"), 1))),
                                                new gendoc::parsing::ExpectationParser(
                                                    new gendoc::parsing::CharParser(']'))))))))))))));
    AddRule(new InitializerRule(ToUtf32("Initializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('='),
                            new gendoc::parsing::NonterminalParser(ToUtf32("InitializerClause"), ToUtf32("InitializerClause"), 1)))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser('('),
                                new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 1)),
                            new gendoc::parsing::CharParser(')'))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("BracedInitializerList"), ToUtf32("BracedInitializerList"), 1)))));
    AddRule(new BraceOrEqualInitializerRule(ToUtf32("BraceOrEqualInitializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('='),
                        new gendoc::parsing::NonterminalParser(ToUtf32("InitializerClause"), ToUtf32("InitializerClause"), 1)))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("BracedInitializerList"), ToUtf32("BracedInitializerList"), 1)))));
    AddRule(new BracedInitializerListRule(ToUtf32("BracedInitializerList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::CharParser('{')),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("InitializerClause"), 1)),
                            new gendoc::parsing::KleeneStarParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::CharParser(','),
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("InitializerClause"), 1))))))))),
            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                new gendoc::parsing::CharParser('}')))));
    AddRule(new InitializerClauseRule(ToUtf32("InitializerClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("BracedInitializerList"), ToUtf32("BracedInitializerList"), 1)))));
    AddRule(new InitializerListRule(ToUtf32("InitializerList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("InitializerClause"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser(','),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("InitializerClause"), 1))))))))));
}

} } // namespace gendoc.cppparser
