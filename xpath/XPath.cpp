#include "XPath.hpp"
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

namespace gendoc { namespace xpath {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

XPath* XPath::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

XPath* XPath::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    XPath* grammar(new XPath(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

XPath::XPath(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("XPath"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.xpath")), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back(ToUtf32("ancestor"));
    keywords0.push_back(ToUtf32("ancestor-or-self"));
    keywords0.push_back(ToUtf32("attribute"));
    keywords0.push_back(ToUtf32("child"));
    keywords0.push_back(ToUtf32("descendant"));
    keywords0.push_back(ToUtf32("descendant-or-self"));
    keywords0.push_back(ToUtf32("following"));
    keywords0.push_back(ToUtf32("following-sibling"));
    keywords0.push_back(ToUtf32("namespace"));
    keywords0.push_back(ToUtf32("parent"));
    keywords0.push_back(ToUtf32("preceding"));
    keywords0.push_back(ToUtf32("preceding-sibling"));
    keywords0.push_back(ToUtf32("self"));
}

XPathExpr* XPath::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    XPathExpr* result = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(value.get());
    stack.pop();
    return result;
}

class XPath::ExprRule : public gendoc::parsing::Rule
{
public:
    ExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ExprRule>(this, &ExprRule::A0Action));
        gendoc::parsing::NonterminalParser* orExprNonterminalParser = GetNonterminal(ToUtf32("OrExpr"));
        orExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ExprRule>(this, &ExprRule::PostOrExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOrExpr;
    }
    void PostOrExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOrExpr_value = std::move(stack.top());
            context->fromOrExpr = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromOrExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromOrExpr() {}
        XPathExpr* value;
        XPathExpr* fromOrExpr;
    };
};

class XPath::OrExprRule : public gendoc::parsing::Rule
{
public:
    OrExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OrExprRule>(this, &OrExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OrExprRule>(this, &OrExprRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<OrExprRule>(this, &OrExprRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<OrExprRule>(this, &OrExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathOrExpr(context->value, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::AndExprRule : public gendoc::parsing::Rule
{
public:
    AndExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AndExprRule>(this, &AndExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AndExprRule>(this, &AndExprRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AndExprRule>(this, &AndExprRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AndExprRule>(this, &AndExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathAndExpr(context->value, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::EqualityExprRule : public gendoc::parsing::Rule
{
public:
    EqualityExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExprRule>(this, &EqualityExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExprRule>(this, &EqualityExprRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExprRule>(this, &EqualityExprRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EqualityExprRule>(this, &EqualityExprRule::A3Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EqualityExprRule>(this, &EqualityExprRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EqualityExprRule>(this, &EqualityExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::equal;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::notEqual;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::equal: context->value = new XPathEqualExpr(context->value, context->fromright);
            break;
            case Operator::notEqual: context->value = new XPathNotEqualExpr(context->value, context->fromright);
            break;
        }
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::RelationalExprRule : public gendoc::parsing::Rule
{
public:
    RelationalExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A5Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RelationalExprRule>(this, &RelationalExprRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RelationalExprRule>(this, &RelationalExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::lessOrEqual;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::greaterOrEqual;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::less;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::greater;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::less: context->value = new XPathLessExpr(context->value, context->fromright);
            break;
            case Operator::greater: context->value = new XPathGreaterExpr(context->value, context->fromright);
            break;
            case Operator::lessOrEqual: context->value = new XPathLessOrEqualExpr(context->value, context->fromright);
            break;
            case Operator::greaterOrEqual: context->value = new XPathGreaterOrEqualExpr(context->value, context->fromright);
            break;
        }
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::AdditiveExprRule : public gendoc::parsing::Rule
{
public:
    AdditiveExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExprRule>(this, &AdditiveExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExprRule>(this, &AdditiveExprRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExprRule>(this, &AdditiveExprRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AdditiveExprRule>(this, &AdditiveExprRule::A3Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AdditiveExprRule>(this, &AdditiveExprRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AdditiveExprRule>(this, &AdditiveExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::plus;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::minus;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::plus: context->value = new XPathAddExpr(context->value, context->fromright);
            break;
            case Operator::minus: context->value = new XPathSubExpr(context->value, context->fromright);
            break;
        }
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::MultiplicativeExprRule : public gendoc::parsing::Rule
{
public:
    MultiplicativeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A4Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MultiplicativeExprRule>(this, &MultiplicativeExprRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MultiplicativeExprRule>(this, &MultiplicativeExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::mul;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::div;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::mod;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::mul: context->value = new XPathMulExpr(context->value, context->fromright);
            break;
            case Operator::div: context->value = new XPathDivExpr(context->value, context->fromright);
            break;
            case Operator::mod: context->value = new XPathModExpr(context->value, context->fromright);
            break;
        }
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::UnaryExprRule : public gendoc::parsing::Rule
{
public:
    UnaryExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExprRule>(this, &UnaryExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnaryExprRule>(this, &UnaryExprRule::A1Action));
        gendoc::parsing::NonterminalParser* subjectNonterminalParser = GetNonterminal(ToUtf32("subject"));
        subjectNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExprRule>(this, &UnaryExprRule::Postsubject));
        gendoc::parsing::NonterminalParser* unionExprNonterminalParser = GetNonterminal(ToUtf32("unionExpr"));
        unionExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnaryExprRule>(this, &UnaryExprRule::PostunionExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathUnaryMinusExpr(context->fromsubject);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromunionExpr;
    }
    void Postsubject(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromsubject_value = std::move(stack.top());
            context->fromsubject = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromsubject_value.get());
            stack.pop();
        }
    }
    void PostunionExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromunionExpr_value = std::move(stack.top());
            context->fromunionExpr = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromunionExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromsubject(), fromunionExpr() {}
        XPathExpr* value;
        XPathExpr* fromsubject;
        XPathExpr* fromunionExpr;
    };
};

class XPath::UnionExprRule : public gendoc::parsing::Rule
{
public:
    UnionExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnionExprRule>(this, &UnionExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnionExprRule>(this, &UnionExprRule::A1Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnionExprRule>(this, &UnionExprRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnionExprRule>(this, &UnionExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathUnionExpr(context->value, context->fromright);
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::PathExprRule : public gendoc::parsing::Rule
{
public:
    PathExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<XPathExpr>"), ToUtf32("fnc")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A5Action));
        gendoc::parsing::NonterminalParser* locationPathNonterminalParser = GetNonterminal(ToUtf32("LocationPath"));
        locationPathNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PathExprRule>(this, &PathExprRule::PostLocationPath));
        gendoc::parsing::NonterminalParser* functionCallNonterminalParser = GetNonterminal(ToUtf32("FunctionCall"));
        functionCallNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PathExprRule>(this, &PathExprRule::PostFunctionCall));
        gendoc::parsing::NonterminalParser* filterExprNonterminalParser = GetNonterminal(ToUtf32("FilterExpr"));
        filterExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PathExprRule>(this, &PathExprRule::PostFilterExpr));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PathExprRule>(this, &PathExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLocationPath;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fnc.reset(context->fromFunctionCall);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFilterExpr;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::slashSlash;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::slash;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::slash: context->value = new XPathCombineStepExpr(context->value, context->fromright);
            break;
            case Operator::slashSlash: context->value = new XPathCombineStepExpr(context->value, new XPathCombineStepExpr(new XPathLocationStepExpr(Axis::descendantOrSelf, new XPathAnyNodeTest), context->fromright));
            break;
        }
    }
    void PostLocationPath(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLocationPath_value = std::move(stack.top());
            context->fromLocationPath = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromLocationPath_value.get());
            stack.pop();
        }
    }
    void PostFunctionCall(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFunctionCall_value = std::move(stack.top());
            context->fromFunctionCall = *static_cast<gendoc::parsing::ValueObject<XPathFunctionCall*>*>(fromFunctionCall_value.get());
            stack.pop();
        }
    }
    void PostFilterExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFilterExpr_value = std::move(stack.top());
            context->fromFilterExpr = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromFilterExpr_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), op(), fnc(), fromLocationPath(), fromFunctionCall(), fromFilterExpr(), fromright() {}
        XPathExpr* value;
        Operator op;
        std::unique_ptr<XPathExpr> fnc;
        XPathExpr* fromLocationPath;
        XPathFunctionCall* fromFunctionCall;
        XPathExpr* fromFilterExpr;
        XPathExpr* fromright;
    };
};

class XPath::FilterExprRule : public gendoc::parsing::Rule
{
public:
    FilterExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FilterExprRule>(this, &FilterExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FilterExprRule>(this, &FilterExprRule::A1Action));
        gendoc::parsing::NonterminalParser* primaryNonterminalParser = GetNonterminal(ToUtf32("primary"));
        primaryNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FilterExprRule>(this, &FilterExprRule::Postprimary));
        gendoc::parsing::NonterminalParser* predicateNonterminalParser = GetNonterminal(ToUtf32("predicate"));
        predicateNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FilterExprRule>(this, &FilterExprRule::Postpredicate));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromprimary;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathFilterExpr(context->value, context->frompredicate);
    }
    void Postprimary(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromprimary_value = std::move(stack.top());
            context->fromprimary = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromprimary_value.get());
            stack.pop();
        }
    }
    void Postpredicate(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frompredicate_value = std::move(stack.top());
            context->frompredicate = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(frompredicate_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromprimary(), frompredicate() {}
        XPathExpr* value;
        XPathExpr* fromprimary;
        XPathExpr* frompredicate;
    };
};

class XPath::LocationPathRule : public gendoc::parsing::Rule
{
public:
    LocationPathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LocationPathRule>(this, &LocationPathRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LocationPathRule>(this, &LocationPathRule::A1Action));
        gendoc::parsing::NonterminalParser* absoluteLocationPathNonterminalParser = GetNonterminal(ToUtf32("AbsoluteLocationPath"));
        absoluteLocationPathNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LocationPathRule>(this, &LocationPathRule::PostAbsoluteLocationPath));
        gendoc::parsing::NonterminalParser* relativeLocationPathNonterminalParser = GetNonterminal(ToUtf32("RelativeLocationPath"));
        relativeLocationPathNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LocationPathRule>(this, &LocationPathRule::PostRelativeLocationPath));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAbsoluteLocationPath;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRelativeLocationPath;
    }
    void PostAbsoluteLocationPath(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAbsoluteLocationPath_value = std::move(stack.top());
            context->fromAbsoluteLocationPath = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromAbsoluteLocationPath_value.get());
            stack.pop();
        }
    }
    void PostRelativeLocationPath(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromRelativeLocationPath_value = std::move(stack.top());
            context->fromRelativeLocationPath = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromRelativeLocationPath_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromAbsoluteLocationPath(), fromRelativeLocationPath() {}
        XPathExpr* value;
        XPathExpr* fromAbsoluteLocationPath;
        XPathExpr* fromRelativeLocationPath;
    };
};

class XPath::AbsoluteLocationPathRule : public gendoc::parsing::Rule
{
public:
    AbsoluteLocationPathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::A2Action));
        gendoc::parsing::NonterminalParser* abbreviatedAbsoluteLocationPathNonterminalParser = GetNonterminal(ToUtf32("AbbreviatedAbsoluteLocationPath"));
        abbreviatedAbsoluteLocationPathNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::PostAbbreviatedAbsoluteLocationPath));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAbbreviatedAbsoluteLocationPath;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathRootNodeExpr;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathCombineStepExpr(context->value, context->fromright);
    }
    void PostAbbreviatedAbsoluteLocationPath(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAbbreviatedAbsoluteLocationPath_value = std::move(stack.top());
            context->fromAbbreviatedAbsoluteLocationPath = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromAbbreviatedAbsoluteLocationPath_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromAbbreviatedAbsoluteLocationPath(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromAbbreviatedAbsoluteLocationPath;
        XPathExpr* fromright;
    };
};

class XPath::RelativeLocationPathRule : public gendoc::parsing::Rule
{
public:
    RelativeLocationPathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelativeLocationPathRule>(this, &RelativeLocationPathRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelativeLocationPathRule>(this, &RelativeLocationPathRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelativeLocationPathRule>(this, &RelativeLocationPathRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RelativeLocationPathRule>(this, &RelativeLocationPathRule::A3Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RelativeLocationPathRule>(this, &RelativeLocationPathRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RelativeLocationPathRule>(this, &RelativeLocationPathRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::slashSlash;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::slash;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::slash: context->value = new XPathCombineStepExpr(context->value, context->fromright);
            break;
            case Operator::slashSlash: context->value = new XPathCombineStepExpr(context->value, new XPathCombineStepExpr(new XPathLocationStepExpr(Axis::descendantOrSelf, new XPathAnyNodeTest), context->fromright));
            break;
        }
    }
    void Postleft(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<XPathLocationStepExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathLocationStepExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathLocationStepExpr* fromleft;
        XPathLocationStepExpr* fromright;
    };
};

class XPath::AbbreviatedAbsoluteLocationPathRule : public gendoc::parsing::Rule
{
public:
    AbbreviatedAbsoluteLocationPathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AbbreviatedAbsoluteLocationPathRule>(this, &AbbreviatedAbsoluteLocationPathRule::A0Action));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AbbreviatedAbsoluteLocationPathRule>(this, &AbbreviatedAbsoluteLocationPathRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathCombineStepExpr(new XPathRootNodeExpr, new XPathCombineStepExpr(new XPathLocationStepExpr(Axis::descendantOrSelf, new XPathAnyNodeTest), context->fromright));
    }
    void Postright(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromright;
    };
};

class XPath::StepRule : public gendoc::parsing::Rule
{
public:
    StepRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathLocationStepExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathLocationStepExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StepRule>(this, &StepRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StepRule>(this, &StepRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StepRule>(this, &StepRule::A2Action));
        gendoc::parsing::NonterminalParser* axisNonterminalParser = GetNonterminal(ToUtf32("axis"));
        axisNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StepRule>(this, &StepRule::Postaxis));
        gendoc::parsing::NonterminalParser* nodeTestNonterminalParser = GetNonterminal(ToUtf32("nodeTest"));
        nodeTestNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StepRule>(this, &StepRule::PostnodeTest));
        gendoc::parsing::NonterminalParser* predicateNonterminalParser = GetNonterminal(ToUtf32("predicate"));
        predicateNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StepRule>(this, &StepRule::Postpredicate));
        gendoc::parsing::NonterminalParser* abbreviatedStepNonterminalParser = GetNonterminal(ToUtf32("AbbreviatedStep"));
        abbreviatedStepNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StepRule>(this, &StepRule::PostAbbreviatedStep));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLocationStepExpr(context->fromaxis, context->fromnodeTest);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddPredicate(context->frompredicate);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAbbreviatedStep;
    }
    void Postaxis(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromaxis_value = std::move(stack.top());
            context->fromaxis = *static_cast<gendoc::parsing::ValueObject<Axis>*>(fromaxis_value.get());
            stack.pop();
        }
    }
    void PostnodeTest(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromnodeTest_value = std::move(stack.top());
            context->fromnodeTest = *static_cast<gendoc::parsing::ValueObject<XPathNodeTestExpr*>*>(fromnodeTest_value.get());
            stack.pop();
        }
    }
    void Postpredicate(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frompredicate_value = std::move(stack.top());
            context->frompredicate = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(frompredicate_value.get());
            stack.pop();
        }
    }
    void PostAbbreviatedStep(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAbbreviatedStep_value = std::move(stack.top());
            context->fromAbbreviatedStep = *static_cast<gendoc::parsing::ValueObject<XPathLocationStepExpr*>*>(fromAbbreviatedStep_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromaxis(), fromnodeTest(), frompredicate(), fromAbbreviatedStep() {}
        XPathLocationStepExpr* value;
        Axis fromaxis;
        XPathNodeTestExpr* fromnodeTest;
        XPathExpr* frompredicate;
        XPathLocationStepExpr* fromAbbreviatedStep;
    };
};

class XPath::AbbreviatedStepRule : public gendoc::parsing::Rule
{
public:
    AbbreviatedStepRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathLocationStepExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathLocationStepExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AbbreviatedStepRule>(this, &AbbreviatedStepRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AbbreviatedStepRule>(this, &AbbreviatedStepRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLocationStepExpr(Axis::parent, new XPathAnyNodeTest);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLocationStepExpr(Axis::self, new XPathAnyNodeTest);
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        XPathLocationStepExpr* value;
    };
};

class XPath::AxisSpecifierRule : public gendoc::parsing::Rule
{
public:
    AxisSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Axis"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Axis>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AxisSpecifierRule>(this, &AxisSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AxisSpecifierRule>(this, &AxisSpecifierRule::A1Action));
        gendoc::parsing::NonterminalParser* axisNameNonterminalParser = GetNonterminal(ToUtf32("AxisName"));
        axisNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AxisSpecifierRule>(this, &AxisSpecifierRule::PostAxisName));
        gendoc::parsing::NonterminalParser* abbreviatedAxisSpecifierNonterminalParser = GetNonterminal(ToUtf32("AbbreviatedAxisSpecifier"));
        abbreviatedAxisSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AxisSpecifierRule>(this, &AxisSpecifierRule::PostAbbreviatedAxisSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAxisName;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAbbreviatedAxisSpecifier;
    }
    void PostAxisName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAxisName_value = std::move(stack.top());
            context->fromAxisName = *static_cast<gendoc::parsing::ValueObject<Axis>*>(fromAxisName_value.get());
            stack.pop();
        }
    }
    void PostAbbreviatedAxisSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAbbreviatedAxisSpecifier_value = std::move(stack.top());
            context->fromAbbreviatedAxisSpecifier = *static_cast<gendoc::parsing::ValueObject<Axis>*>(fromAbbreviatedAxisSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromAxisName(), fromAbbreviatedAxisSpecifier() {}
        Axis value;
        Axis fromAxisName;
        Axis fromAbbreviatedAxisSpecifier;
    };
};

class XPath::AxisNameRule : public gendoc::parsing::Rule
{
public:
    AxisNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Axis"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Axis>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AxisNameRule>(this, &AxisNameRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = GetAxis(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Axis value;
    };
};

class XPath::AbbreviatedAxisSpecifierRule : public gendoc::parsing::Rule
{
public:
    AbbreviatedAxisSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Axis"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Axis>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AbbreviatedAxisSpecifierRule>(this, &AbbreviatedAxisSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AbbreviatedAxisSpecifierRule>(this, &AbbreviatedAxisSpecifierRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Axis::attribute;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Axis::child;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Axis value;
    };
};

class XPath::PredicateRule : public gendoc::parsing::Rule
{
public:
    PredicateRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PredicateRule>(this, &PredicateRule::A0Action));
        gendoc::parsing::NonterminalParser* predicateExprNonterminalParser = GetNonterminal(ToUtf32("PredicateExpr"));
        predicateExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PredicateRule>(this, &PredicateRule::PostPredicateExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPredicateExpr;
    }
    void PostPredicateExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPredicateExpr_value = std::move(stack.top());
            context->fromPredicateExpr = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromPredicateExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromPredicateExpr() {}
        XPathExpr* value;
        XPathExpr* fromPredicateExpr;
    };
};

class XPath::PredicateExprRule : public gendoc::parsing::Rule
{
public:
    PredicateExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PredicateExprRule>(this, &PredicateExprRule::A0Action));
        gendoc::parsing::NonterminalParser* exprNonterminalParser = GetNonterminal(ToUtf32("Expr"));
        exprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PredicateExprRule>(this, &PredicateExprRule::PostExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpr;
    }
    void PostExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpr_value = std::move(stack.top());
            context->fromExpr = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromExpr() {}
        XPathExpr* value;
        XPathExpr* fromExpr;
    };
};

class XPath::NodeTestRule : public gendoc::parsing::Rule
{
public:
    NodeTestRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathNodeTestExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathNodeTestExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NodeTestRule>(this, &NodeTestRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NodeTestRule>(this, &NodeTestRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NodeTestRule>(this, &NodeTestRule::A2Action));
        gendoc::parsing::NonterminalParser* literalNonterminalParser = GetNonterminal(ToUtf32("Literal"));
        literalNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NodeTestRule>(this, &NodeTestRule::PostLiteral));
        gendoc::parsing::NonterminalParser* nodeTypeNonterminalParser = GetNonterminal(ToUtf32("NodeType"));
        nodeTypeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NodeTestRule>(this, &NodeTestRule::PostNodeType));
        gendoc::parsing::NonterminalParser* nameTestNonterminalParser = GetNonterminal(ToUtf32("NameTest"));
        nameTestNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NodeTestRule>(this, &NodeTestRule::PostNameTest));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathPILiteralTest(context->fromLiteral);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNodeType;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNameTest;
    }
    void PostLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLiteral_value = std::move(stack.top());
            context->fromLiteral = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromLiteral_value.get());
            stack.pop();
        }
    }
    void PostNodeType(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNodeType_value = std::move(stack.top());
            context->fromNodeType = *static_cast<gendoc::parsing::ValueObject<XPathNodeTestExpr*>*>(fromNodeType_value.get());
            stack.pop();
        }
    }
    void PostNameTest(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNameTest_value = std::move(stack.top());
            context->fromNameTest = *static_cast<gendoc::parsing::ValueObject<XPathNodeTestExpr*>*>(fromNameTest_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromLiteral(), fromNodeType(), fromNameTest() {}
        XPathNodeTestExpr* value;
        XPathExpr* fromLiteral;
        XPathNodeTestExpr* fromNodeType;
        XPathNodeTestExpr* fromNameTest;
    };
};

class XPath::NameTestRule : public gendoc::parsing::Rule
{
public:
    NameTestRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathNodeTestExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathNodeTestExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NameTestRule>(this, &NameTestRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NameTestRule>(this, &NameTestRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NameTestRule>(this, &NameTestRule::A2Action));
        gendoc::parsing::NonterminalParser* nCNameNonterminalParser = GetNonterminal(ToUtf32("NCName"));
        nCNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NameTestRule>(this, &NameTestRule::PostNCName));
        gendoc::parsing::NonterminalParser* qNameNonterminalParser = GetNonterminal(ToUtf32("QName"));
        qNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NameTestRule>(this, &NameTestRule::PostQName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathPrincipalNodeTest;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathPrefixTest(context->fromNCName);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathNameTest(context->fromQName);
    }
    void PostNCName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNCName_value = std::move(stack.top());
            context->fromNCName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromNCName_value.get());
            stack.pop();
        }
    }
    void PostQName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromQName_value = std::move(stack.top());
            context->fromQName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromQName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromNCName(), fromQName() {}
        XPathNodeTestExpr* value;
        std::u32string fromNCName;
        std::u32string fromQName;
    };
};

class XPath::NodeTypeRule : public gendoc::parsing::Rule
{
public:
    NodeTypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathNodeTestExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathNodeTestExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NodeTypeRule>(this, &NodeTypeRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NodeTypeRule>(this, &NodeTypeRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NodeTypeRule>(this, &NodeTypeRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NodeTypeRule>(this, &NodeTypeRule::A3Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathCommentNodeTest;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathTextNodeTest;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathPINodeTest;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathAnyNodeTest;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        XPathNodeTestExpr* value;
    };
};

class XPath::PrimaryExprRule : public gendoc::parsing::Rule
{
public:
    PrimaryExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A4Action));
        gendoc::parsing::NonterminalParser* functionCallNonterminalParser = GetNonterminal(ToUtf32("FunctionCall"));
        functionCallNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostFunctionCall));
        gendoc::parsing::NonterminalParser* variableReferenceNonterminalParser = GetNonterminal(ToUtf32("VariableReference"));
        variableReferenceNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostVariableReference));
        gendoc::parsing::NonterminalParser* exprNonterminalParser = GetNonterminal(ToUtf32("Expr"));
        exprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostExpr));
        gendoc::parsing::NonterminalParser* literalNonterminalParser = GetNonterminal(ToUtf32("Literal"));
        literalNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostLiteral));
        gendoc::parsing::NonterminalParser* numberNonterminalParser = GetNonterminal(ToUtf32("Number"));
        numberNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostNumber));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFunctionCall;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromVariableReference;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpr;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLiteral;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNumber;
    }
    void PostFunctionCall(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFunctionCall_value = std::move(stack.top());
            context->fromFunctionCall = *static_cast<gendoc::parsing::ValueObject<XPathFunctionCall*>*>(fromFunctionCall_value.get());
            stack.pop();
        }
    }
    void PostVariableReference(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromVariableReference_value = std::move(stack.top());
            context->fromVariableReference = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromVariableReference_value.get());
            stack.pop();
        }
    }
    void PostExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpr_value = std::move(stack.top());
            context->fromExpr = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromExpr_value.get());
            stack.pop();
        }
    }
    void PostLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLiteral_value = std::move(stack.top());
            context->fromLiteral = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromLiteral_value.get());
            stack.pop();
        }
    }
    void PostNumber(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNumber_value = std::move(stack.top());
            context->fromNumber = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromNumber_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromFunctionCall(), fromVariableReference(), fromExpr(), fromLiteral(), fromNumber() {}
        XPathExpr* value;
        XPathFunctionCall* fromFunctionCall;
        XPathExpr* fromVariableReference;
        XPathExpr* fromExpr;
        XPathExpr* fromLiteral;
        XPathExpr* fromNumber;
    };
};

class XPath::VariableReferenceRule : public gendoc::parsing::Rule
{
public:
    VariableReferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<VariableReferenceRule>(this, &VariableReferenceRule::A0Action));
        gendoc::parsing::NonterminalParser* qNameNonterminalParser = GetNonterminal(ToUtf32("QName"));
        qNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<VariableReferenceRule>(this, &VariableReferenceRule::PostQName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathVariableReference(context->fromQName);
    }
    void PostQName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromQName_value = std::move(stack.top());
            context->fromQName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromQName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromQName() {}
        XPathExpr* value;
        std::u32string fromQName;
    };
};

class XPath::LiteralRule : public gendoc::parsing::Rule
{
public:
    LiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLiteral(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLiteral(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        XPathExpr* value;
    };
};

class XPath::NumberRule : public gendoc::parsing::Rule
{
public:
    NumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NumberRule>(this, &NumberRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NumberRule>(this, &NumberRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathNumberExpr(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathNumberExpr(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        XPathExpr* value;
    };
};

class XPath::FunctionCallRule : public gendoc::parsing::Rule
{
public:
    FunctionCallRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathFunctionCall*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathFunctionCall*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FunctionCallRule>(this, &FunctionCallRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FunctionCallRule>(this, &FunctionCallRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FunctionCallRule>(this, &FunctionCallRule::A2Action));
        gendoc::parsing::NonterminalParser* functionNameNonterminalParser = GetNonterminal(ToUtf32("FunctionName"));
        functionNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FunctionCallRule>(this, &FunctionCallRule::PostFunctionName));
        gendoc::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FunctionCallRule>(this, &FunctionCallRule::Postfirst));
        gendoc::parsing::NonterminalParser* nextNonterminalParser = GetNonterminal(ToUtf32("next"));
        nextNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FunctionCallRule>(this, &FunctionCallRule::Postnext));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathFunctionCall(context->fromFunctionName);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddArgument(context->fromfirst);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddArgument(context->fromnext);
    }
    void PostFunctionName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFunctionName_value = std::move(stack.top());
            context->fromFunctionName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromFunctionName_value.get());
            stack.pop();
        }
    }
    void Postfirst(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postnext(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromnext_value = std::move(stack.top());
            context->fromnext = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromnext_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromFunctionName(), fromfirst(), fromnext() {}
        XPathFunctionCall* value;
        std::u32string fromFunctionName;
        XPathExpr* fromfirst;
        XPathExpr* fromnext;
    };
};

class XPath::ArgumentRule : public gendoc::parsing::Rule
{
public:
    ArgumentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ArgumentRule>(this, &ArgumentRule::A0Action));
        gendoc::parsing::NonterminalParser* exprNonterminalParser = GetNonterminal(ToUtf32("Expr"));
        exprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ArgumentRule>(this, &ArgumentRule::PostExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpr;
    }
    void PostExpr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExpr_value = std::move(stack.top());
            context->fromExpr = *static_cast<gendoc::parsing::ValueObject<XPathExpr*>*>(fromExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromExpr() {}
        XPathExpr* value;
        XPathExpr* fromExpr;
    };
};

class XPath::FunctionNameRule : public gendoc::parsing::Rule
{
public:
    FunctionNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<XPathExpr>"), ToUtf32("nodeType")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FunctionNameRule>(this, &FunctionNameRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FunctionNameRule>(this, &FunctionNameRule::A1Action));
        gendoc::parsing::NonterminalParser* qNameNonterminalParser = GetNonterminal(ToUtf32("QName"));
        qNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FunctionNameRule>(this, &FunctionNameRule::PostQName));
        gendoc::parsing::NonterminalParser* nodeTypeNonterminalParser = GetNonterminal(ToUtf32("NodeType"));
        nodeTypeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FunctionNameRule>(this, &FunctionNameRule::PostNodeType));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromQName;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->nodeType.reset(context->fromNodeType);
    }
    void PostQName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromQName_value = std::move(stack.top());
            context->fromQName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromQName_value.get());
            stack.pop();
        }
    }
    void PostNodeType(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNodeType_value = std::move(stack.top());
            context->fromNodeType = *static_cast<gendoc::parsing::ValueObject<XPathNodeTestExpr*>*>(fromNodeType_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), nodeType(), fromQName(), fromNodeType() {}
        std::u32string value;
        std::unique_ptr<XPathExpr> nodeType;
        std::u32string fromQName;
        XPathNodeTestExpr* fromNodeType;
    };
};

class XPath::QNameRule : public gendoc::parsing::Rule
{
public:
    QNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QNameRule>(this, &QNameRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<QNameRule>(this, &QNameRule::A1Action));
        gendoc::parsing::NonterminalParser* prefixedNameNonterminalParser = GetNonterminal(ToUtf32("PrefixedName"));
        prefixedNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QNameRule>(this, &QNameRule::PostPrefixedName));
        gendoc::parsing::NonterminalParser* unprefixedNameNonterminalParser = GetNonterminal(ToUtf32("UnprefixedName"));
        unprefixedNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<QNameRule>(this, &QNameRule::PostUnprefixedName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrefixedName;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUnprefixedName;
    }
    void PostPrefixedName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPrefixedName_value = std::move(stack.top());
            context->fromPrefixedName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromPrefixedName_value.get());
            stack.pop();
        }
    }
    void PostUnprefixedName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUnprefixedName_value = std::move(stack.top());
            context->fromUnprefixedName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromUnprefixedName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromPrefixedName(), fromUnprefixedName() {}
        std::u32string value;
        std::u32string fromPrefixedName;
        std::u32string fromUnprefixedName;
    };
};

class XPath::PrefixedNameRule : public gendoc::parsing::Rule
{
public:
    PrefixedNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrefixedNameRule>(this, &PrefixedNameRule::A0Action));
        gendoc::parsing::NonterminalParser* prefixNonterminalParser = GetNonterminal(ToUtf32("Prefix"));
        prefixNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrefixedNameRule>(this, &PrefixedNameRule::PostPrefix));
        gendoc::parsing::NonterminalParser* localPartNonterminalParser = GetNonterminal(ToUtf32("LocalPart"));
        localPartNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrefixedNameRule>(this, &PrefixedNameRule::PostLocalPart));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void PostPrefix(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPrefix_value = std::move(stack.top());
            context->fromPrefix = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromPrefix_value.get());
            stack.pop();
        }
    }
    void PostLocalPart(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLocalPart_value = std::move(stack.top());
            context->fromLocalPart = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromLocalPart_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromPrefix(), fromLocalPart() {}
        std::u32string value;
        std::u32string fromPrefix;
        std::u32string fromLocalPart;
    };
};

class XPath::UnprefixedNameRule : public gendoc::parsing::Rule
{
public:
    UnprefixedNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnprefixedNameRule>(this, &UnprefixedNameRule::A0Action));
        gendoc::parsing::NonterminalParser* localPartNonterminalParser = GetNonterminal(ToUtf32("LocalPart"));
        localPartNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UnprefixedNameRule>(this, &UnprefixedNameRule::PostLocalPart));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLocalPart;
    }
    void PostLocalPart(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLocalPart_value = std::move(stack.top());
            context->fromLocalPart = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromLocalPart_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromLocalPart() {}
        std::u32string value;
        std::u32string fromLocalPart;
    };
};

class XPath::PrefixRule : public gendoc::parsing::Rule
{
public:
    PrefixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A0Action));
        gendoc::parsing::NonterminalParser* nCNameNonterminalParser = GetNonterminal(ToUtf32("NCName"));
        nCNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrefixRule>(this, &PrefixRule::PostNCName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNCName;
    }
    void PostNCName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNCName_value = std::move(stack.top());
            context->fromNCName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromNCName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromNCName() {}
        std::u32string value;
        std::u32string fromNCName;
    };
};

class XPath::LocalPartRule : public gendoc::parsing::Rule
{
public:
    LocalPartRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LocalPartRule>(this, &LocalPartRule::A0Action));
        gendoc::parsing::NonterminalParser* nCNameNonterminalParser = GetNonterminal(ToUtf32("NCName"));
        nCNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LocalPartRule>(this, &LocalPartRule::PostNCName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNCName;
    }
    void PostNCName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNCName_value = std::move(stack.top());
            context->fromNCName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromNCName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromNCName() {}
        std::u32string value;
        std::u32string fromNCName;
    };
};

class XPath::NCNameRule : public gendoc::parsing::Rule
{
public:
    NCNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NCNameRule>(this, &NCNameRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

void XPath::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.xml.Xml"));
    if (!grammar1)
    {
        grammar1 = gendoc::xml::Xml::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void XPath::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("NameChar"), this, ToUtf32("gendoc.xml.Xml.NameChar")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("NameStartChar"), this, ToUtf32("gendoc.xml.Xml.NameStartChar")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("gendoc.parsing.stdlib.identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("spaces"), this, ToUtf32("gendoc.parsing.stdlib.spaces")));
    AddRule(new ExprRule(ToUtf32("Expr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("OrExpr"), ToUtf32("OrExpr"), 0))));
    AddRule(new OrExprRule(ToUtf32("OrExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AndExpr"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("or")),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AndExpr"), 0))))))));
    AddRule(new AndExprRule(ToUtf32("AndExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("EqualityExpr"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("and")),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("EqualityExpr"), 0))))))));
    AddRule(new EqualityExprRule(ToUtf32("EqualityExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("RelationalExpr"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::CharParser('=')),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::StringParser(ToUtf32("!="))))),
                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelationalExpr"), 0))))))));
    AddRule(new RelationalExprRule(ToUtf32("RelationalExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AdditiveExpr"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                            new gendoc::parsing::StringParser(ToUtf32("<="))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::StringParser(ToUtf32(">=")))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                        new gendoc::parsing::CharParser('<'))),
                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                    new gendoc::parsing::CharParser('>')))),
                        new gendoc::parsing::ActionParser(ToUtf32("A5"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AdditiveExpr"), 0))))))));
    AddRule(new AdditiveExprRule(ToUtf32("AdditiveExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("MultiplicativeExpr"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::CharParser('+')),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::CharParser('-')))),
                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("MultiplicativeExpr"), 0))))))));
    AddRule(new MultiplicativeExprRule(ToUtf32("MultiplicativeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("UnaryExpr"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::CharParser('*')),
                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                        new gendoc::parsing::StringParser(ToUtf32("div")))),
                                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                    new gendoc::parsing::StringParser(ToUtf32("mod"))))),
                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("UnaryExpr"), 0))))))));
    AddRule(new UnaryExprRule(ToUtf32("UnaryExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('-'),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("subject"), ToUtf32("UnaryExpr"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("unionExpr"), ToUtf32("UnionExpr"), 0)))));
    AddRule(new UnionExprRule(ToUtf32("UnionExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("PathExpr"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('|'),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("PathExpr"), 0))))))));
    AddRule(new PathExprRule(ToUtf32("PathExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::DifferenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("LocationPath"), ToUtf32("LocationPath"), 0),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("FunctionCall"), ToUtf32("FunctionCall"), 0))))),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("FilterExpr"), ToUtf32("FilterExpr"), 0)),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                        new gendoc::parsing::StringParser(ToUtf32("//"))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                        new gendoc::parsing::CharParser('/')))),
                            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelativeLocationPath"), 0)))))))));
    AddRule(new FilterExprRule(ToUtf32("FilterExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("primary"), ToUtf32("PrimaryExpr"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("predicate"), ToUtf32("Predicate"), 0)))))));
    AddRule(new LocationPathRule(ToUtf32("LocationPath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("AbsoluteLocationPath"), ToUtf32("AbsoluteLocationPath"), 0)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("RelativeLocationPath"), ToUtf32("RelativeLocationPath"), 0)))));
    AddRule(new AbsoluteLocationPathRule(ToUtf32("AbsoluteLocationPath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("AbbreviatedAbsoluteLocationPath"), ToUtf32("AbbreviatedAbsoluteLocationPath"), 0)),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::CharParser('/')),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelativeLocationPath"), 0))))))));
    AddRule(new RelativeLocationPathRule(ToUtf32("RelativeLocationPath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Step"), 0)),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::StringParser(ToUtf32("//"))),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::CharParser('/')))),
                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Step"), 0))))))));
    AddRule(new AbbreviatedAbsoluteLocationPathRule(ToUtf32("AbbreviatedAbsoluteLocationPath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::StringParser(ToUtf32("//")),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelativeLocationPath"), 0)))));
    AddRule(new StepRule(ToUtf32("Step"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("axis"), ToUtf32("AxisSpecifier"), 0),
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("nodeTest"), ToUtf32("NodeTest"), 0))),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("predicate"), ToUtf32("Predicate"), 0))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("AbbreviatedStep"), ToUtf32("AbbreviatedStep"), 0)))));
    AddRule(new AbbreviatedStepRule(ToUtf32("AbbreviatedStep"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::StringParser(ToUtf32(".."))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::CharParser('.')))));
    AddRule(new AxisSpecifierRule(ToUtf32("AxisSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("AxisName"), ToUtf32("AxisName"), 0),
                        new gendoc::parsing::StringParser(ToUtf32("::"))))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("AbbreviatedAxisSpecifier"), ToUtf32("AbbreviatedAxisSpecifier"), 0)))));
    AddRule(new AxisNameRule(ToUtf32("AxisName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordListParser(ToUtf32("identifier"), keywords0))));
    AddRule(new AbbreviatedAxisSpecifierRule(ToUtf32("AbbreviatedAxisSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::CharParser('@')),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::EmptyParser()))));
    AddRule(new PredicateRule(ToUtf32("Predicate"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('['),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("PredicateExpr"), ToUtf32("PredicateExpr"), 0))),
            new gendoc::parsing::CharParser(']'))));
    AddRule(new PredicateExprRule(ToUtf32("PredicateExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("Expr"), ToUtf32("Expr"), 0))));
    AddRule(new NodeTestRule(ToUtf32("NodeTest"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("processing-instruction")),
                                    new gendoc::parsing::CharParser('(')),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Literal"), ToUtf32("Literal"), 0)),
                            new gendoc::parsing::CharParser(')')))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("NodeType"), ToUtf32("NodeType"), 0),
                                new gendoc::parsing::CharParser('(')),
                            new gendoc::parsing::CharParser(')'))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("NameTest"), ToUtf32("NameTest"), 0)))));
    AddRule(new NameTestRule(ToUtf32("NameTest"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::CharParser('*')),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("NCName"), ToUtf32("NCName"), 0),
                                new gendoc::parsing::CharParser(':')),
                            new gendoc::parsing::CharParser('*'))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::NonterminalParser(ToUtf32("QName"), ToUtf32("QName"), 0)))));
    AddRule(new NodeTypeRule(ToUtf32("NodeType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::KeywordParser(ToUtf32("comment"))),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::KeywordParser(ToUtf32("text")))),
                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                    new gendoc::parsing::KeywordParser(ToUtf32("processing-instruction")))),
            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                new gendoc::parsing::KeywordParser(ToUtf32("node"))))));
    AddRule(new PrimaryExprRule(ToUtf32("PrimaryExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("FunctionCall"), ToUtf32("FunctionCall"), 0)),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("VariableReference"), ToUtf32("VariableReference"), 0))),
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('('),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Expr"), ToUtf32("Expr"), 0)),
                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                            new gendoc::parsing::CharParser(')')))),
                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Literal"), ToUtf32("Literal"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                new gendoc::parsing::NonterminalParser(ToUtf32("Number"), ToUtf32("Number"), 0)))));
    AddRule(new VariableReferenceRule(ToUtf32("VariableReference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('$'),
                    new gendoc::parsing::NonterminalParser(ToUtf32("QName"), ToUtf32("QName"), 0))))));
    AddRule(new LiteralRule(ToUtf32("Literal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('\"'),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::KleeneStarParser(
                                    new gendoc::parsing::CharSetParser(ToUtf32("\""), true))))),
                    new gendoc::parsing::CharParser('\"'))),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('\''),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::KleeneStarParser(
                                    new gendoc::parsing::CharSetParser(ToUtf32("\'"), true))))),
                    new gendoc::parsing::CharParser('\''))))));
    AddRule(new NumberRule(ToUtf32("Number"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Digits"), ToUtf32("Digits"), 0),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::CharParser('.'),
                                    new gendoc::parsing::OptionalParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("Digits"), ToUtf32("Digits"), 0)))))))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('.'),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Digits"), ToUtf32("Digits"), 0)))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Digits"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::PositiveParser(
                new gendoc::parsing::CharSetParser(ToUtf32("0-9"))))));
    AddRule(new FunctionCallRule(ToUtf32("FunctionCall"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("FunctionName"), ToUtf32("FunctionName"), 0)),
                        new gendoc::parsing::CharParser('('))),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("Argument"), 0)),
                            new gendoc::parsing::KleeneStarParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::CharParser(','),
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("next"), ToUtf32("Argument"), 0))))))))),
            new gendoc::parsing::CharParser(')'))));
    AddRule(new ArgumentRule(ToUtf32("Argument"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("Expr"), ToUtf32("Expr"), 0))));
    AddRule(new FunctionNameRule(ToUtf32("FunctionName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::DifferenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("QName"), ToUtf32("QName"), 0),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("NodeType"), ToUtf32("NodeType"), 0)))))));
    AddRule(new QNameRule(ToUtf32("QName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("PrefixedName"), ToUtf32("PrefixedName"), 0)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("UnprefixedName"), ToUtf32("UnprefixedName"), 0)))));
    AddRule(new PrefixedNameRule(ToUtf32("PrefixedName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Prefix"), ToUtf32("Prefix"), 0),
                        new gendoc::parsing::CharParser(':')),
                    new gendoc::parsing::NonterminalParser(ToUtf32("LocalPart"), ToUtf32("LocalPart"), 0))))));
    AddRule(new UnprefixedNameRule(ToUtf32("UnprefixedName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("LocalPart"), ToUtf32("LocalPart"), 0))));
    AddRule(new PrefixRule(ToUtf32("Prefix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("NCName"), ToUtf32("NCName"), 0))));
    AddRule(new LocalPartRule(ToUtf32("LocalPart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("NCName"), ToUtf32("NCName"), 0))));
    AddRule(new NCNameRule(ToUtf32("NCName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::DifferenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("NameStartChar"), ToUtf32("NameStartChar"), 0),
                            new gendoc::parsing::CharParser(':'))),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::DifferenceParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("NameChar"), ToUtf32("NameChar"), 0),
                                new gendoc::parsing::CharParser(':')))))))));
    SetSkipRuleName(ToUtf32("spaces"));
}

} } // namespace gendoc.xpath
