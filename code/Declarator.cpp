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
#include <gendoc/code/Expression.hpp>
#include <gendoc/code/Declaration.hpp>
#include <gendoc/util/TextUtils.hpp>
#include <gendoc/codedom/Type.hpp>

namespace gendoc { namespace code {

using gendoc::util::Trim;
using namespace gendoc::codedom;
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

Declarator::Declarator(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Declarator"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.code")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::codedom::InitDeclaratorList* Declarator::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    gendoc::codedom::InitDeclaratorList* result = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::InitDeclaratorList*>*>(value.get());
    stack.pop();
    return result;
}

class Declarator::InitDeclaratorListRule : public gendoc::parsing::Rule
{
public:
    InitDeclaratorListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::InitDeclaratorList*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<InitDeclaratorList>"), ToUtf32("idl")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::InitDeclaratorList*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitDeclaratorListRule>(this, &InitDeclaratorListRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitDeclaratorListRule>(this, &InitDeclaratorListRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitDeclaratorListRule>(this, &InitDeclaratorListRule::A2Action));
        gendoc::parsing::NonterminalParser* initDeclaratorNonterminalParser = GetNonterminal(ToUtf32("InitDeclarator"));
        initDeclaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitDeclaratorListRule>(this, &InitDeclaratorListRule::PostInitDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->idl.reset(new InitDeclaratorList);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->idl.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->idl->Add(context->fromInitDeclarator);
    }
    void PostInitDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromInitDeclarator_value = std::move(stack.top());
            context->fromInitDeclarator = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::InitDeclarator*>*>(fromInitDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), idl(), fromInitDeclarator() {}
        gendoc::codedom::InitDeclaratorList* value;
        std::unique_ptr<InitDeclaratorList> idl;
        gendoc::codedom::InitDeclarator* fromInitDeclarator;
    };
};

class Declarator::InitDeclaratorRule : public gendoc::parsing::Rule
{
public:
    InitDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::InitDeclarator*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::InitDeclarator*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitDeclaratorRule>(this, &InitDeclaratorRule::A0Action));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PostDeclarator));
        gendoc::parsing::NonterminalParser* initializerNonterminalParser = GetNonterminal(ToUtf32("Initializer"));
        initializerNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PostInitializer));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new InitDeclarator(context->fromDeclarator, context->fromInitializer);
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
    void PostInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromInitializer_value = std::move(stack.top());
            context->fromInitializer = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::Initializer*>*>(fromInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromDeclarator(), fromInitializer() {}
        gendoc::codedom::InitDeclarator* value;
        std::u32string fromDeclarator;
        gendoc::codedom::Initializer* fromInitializer;
    };
};

class Declarator::DeclaratorRule : public gendoc::parsing::Rule
{
public:
    DeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A0Action));
        gendoc::parsing::NonterminalParser* directDeclaratorNonterminalParser = GetNonterminal(ToUtf32("DirectDeclarator"));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Trim(std::u32string(matchBegin, matchEnd));
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
        Context(): value(), fromDeclarator() {}
        std::u32string value;
        std::u32string fromDeclarator;
    };
};

class Declarator::DirectDeclaratorRule : public gendoc::parsing::Rule
{
public:
    DirectDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("o")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DirectDeclaratorRule>(this, &DirectDeclaratorRule::A0Action));
        gendoc::parsing::NonterminalParser* declaratorIdNonterminalParser = GetNonterminal(ToUtf32("DeclaratorId"));
        gendoc::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DirectDeclaratorRule>(this, &DirectDeclaratorRule::PostConstantExpression));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DirectDeclaratorRule>(this, &DirectDeclaratorRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromConstantExpression);
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
        Context(): o(), fromConstantExpression(), fromDeclarator() {}
        std::unique_ptr<CppObject> o;
        gendoc::codedom::CppObject* fromConstantExpression;
        std::u32string fromDeclarator;
    };
};

class Declarator::DeclaratorIdRule : public gendoc::parsing::Rule
{
public:
    DeclaratorIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("o")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorIdRule>(this, &DeclaratorIdRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclaratorIdRule>(this, &DeclaratorIdRule::A1Action));
        gendoc::parsing::NonterminalParser* idExpressionNonterminalParser = GetNonterminal(ToUtf32("IdExpression"));
        idExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorIdRule>(this, &DeclaratorIdRule::PostIdExpression));
        gendoc::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclaratorIdRule>(this, &DeclaratorIdRule::PostTypeName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromIdExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromTypeName);
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
        Context(): o(), fromIdExpression(), fromTypeName() {}
        std::unique_ptr<CppObject> o;
        gendoc::codedom::IdExpr* fromIdExpression;
        gendoc::codedom::TypeName* fromTypeName;
    };
};

class Declarator::TypeIdRule : public gendoc::parsing::Rule
{
public:
    TypeIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A3Action));
        gendoc::parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierSeq"));
        typeSpecifierSeqNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TypeIdRule>(this, &TypeIdRule::PreTypeSpecifierSeq));
        gendoc::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeIdRule>(this, &TypeIdRule::PostTypeName));
        gendoc::parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("AbstractDeclarator"));
        abstractDeclaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeIdRule>(this, &TypeIdRule::PostAbstractDeclarator));
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
        context->ti->Declarator() = context->fromAbstractDeclarator;
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
        Context(): value(), ti(), fromTypeName(), fromAbstractDeclarator() {}
        gendoc::codedom::TypeId* value;
        std::unique_ptr<TypeId> ti;
        gendoc::codedom::TypeName* fromTypeName;
        std::u32string fromAbstractDeclarator;
    };
};

class Declarator::TypeRule : public gendoc::parsing::Rule
{
public:
    TypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Type*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Type>"), ToUtf32("t")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Type*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A3Action));
        gendoc::parsing::NonterminalParser* typeSpecifierNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifier"));
        typeSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeRule>(this, &TypeRule::PostTypeSpecifier));
        gendoc::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeRule>(this, &TypeRule::PostTypeName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->t.reset(new Type);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->t.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->t->Add(context->fromTypeSpecifier);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->t->Add(context->fromTypeName);
    }
    void PostTypeSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeSpecifier_value = std::move(stack.top());
            context->fromTypeSpecifier = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeSpecifier*>*>(fromTypeSpecifier_value.get());
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
        Context(): value(), t(), fromTypeSpecifier(), fromTypeName() {}
        gendoc::codedom::Type* value;
        std::unique_ptr<Type> t;
        gendoc::codedom::TypeSpecifier* fromTypeSpecifier;
        gendoc::codedom::TypeName* fromTypeName;
    };
};

class Declarator::TypeSpecifierSeqRule : public gendoc::parsing::Rule
{
public:
    TypeSpecifierSeqRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::codedom::TypeId*"), ToUtf32("typeId")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> typeId_value = std::move(stack.top());
        context->typeId = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeId*>*>(typeId_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypeSpecifierSeqRule>(this, &TypeSpecifierSeqRule::A0Action));
        gendoc::parsing::NonterminalParser* typeSpecifierNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifier"));
        typeSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypeSpecifierSeqRule>(this, &TypeSpecifierSeqRule::PostTypeSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeId->Add(context->fromTypeSpecifier);
    }
    void PostTypeSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypeSpecifier_value = std::move(stack.top());
            context->fromTypeSpecifier = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeSpecifier*>*>(fromTypeSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): typeId(), fromTypeSpecifier() {}
        gendoc::codedom::TypeId* typeId;
        gendoc::codedom::TypeSpecifier* fromTypeSpecifier;
    };
};

class Declarator::AbstractDeclaratorRule : public gendoc::parsing::Rule
{
public:
    AbstractDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AbstractDeclaratorRule>(this, &AbstractDeclaratorRule::A0Action));
        gendoc::parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("AbstractDeclarator"));
        abstractDeclaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AbstractDeclaratorRule>(this, &AbstractDeclaratorRule::PostAbstractDeclarator));
        gendoc::parsing::NonterminalParser* directAbstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("DirectAbstractDeclarator"));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Trim(std::u32string(matchBegin, matchEnd));
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
        Context(): value(), fromAbstractDeclarator() {}
        std::u32string value;
        std::u32string fromAbstractDeclarator;
    };
};

class Declarator::DirectAbstractDeclaratorRule : public gendoc::parsing::Rule
{
public:
    DirectAbstractDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("o")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DirectAbstractDeclaratorRule>(this, &DirectAbstractDeclaratorRule::A0Action));
        gendoc::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DirectAbstractDeclaratorRule>(this, &DirectAbstractDeclaratorRule::PostConstantExpression));
        gendoc::parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("AbstractDeclarator"));
        abstractDeclaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DirectAbstractDeclaratorRule>(this, &DirectAbstractDeclaratorRule::PostAbstractDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromConstantExpression);
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
        Context(): o(), fromConstantExpression(), fromAbstractDeclarator() {}
        std::unique_ptr<CppObject> o;
        gendoc::codedom::CppObject* fromConstantExpression;
        std::u32string fromAbstractDeclarator;
    };
};

class Declarator::CVQualifierSeqRule : public gendoc::parsing::Rule
{
public:
    CVQualifierSeqRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("o")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CVQualifierSeqRule>(this, &CVQualifierSeqRule::A0Action));
        gendoc::parsing::NonterminalParser* cVQualifierNonterminalParser = GetNonterminal(ToUtf32("CVQualifier"));
        cVQualifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CVQualifierSeqRule>(this, &CVQualifierSeqRule::PostCVQualifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromCVQualifier);
    }
    void PostCVQualifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCVQualifier_value = std::move(stack.top());
            context->fromCVQualifier = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::TypeSpecifier*>*>(fromCVQualifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): o(), fromCVQualifier() {}
        std::unique_ptr<CppObject> o;
        gendoc::codedom::TypeSpecifier* fromCVQualifier;
    };
};

class Declarator::InitializerRule : public gendoc::parsing::Rule
{
public:
    InitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::Initializer*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::Initializer*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A1Action));
        gendoc::parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal(ToUtf32("InitializerClause"));
        initializerClauseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostInitializerClause));
        gendoc::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Initializer(context->fromInitializerClause, GetEmptyVector());
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Initializer(nullptr, context->fromExpressionList);
    }
    void PostInitializerClause(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromInitializerClause_value = std::move(stack.top());
            context->fromInitializerClause = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::AssignInit*>*>(fromInitializerClause_value.get());
            stack.pop();
        }
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
        Context(): value(), fromInitializerClause(), fromExpressionList() {}
        gendoc::codedom::Initializer* value;
        gendoc::codedom::AssignInit* fromInitializerClause;
        std::vector<gendoc::codedom::CppObject*> fromExpressionList;
    };
};

class Declarator::InitializerClauseRule : public gendoc::parsing::Rule
{
public:
    InitializerClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::codedom::AssignInit*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::AssignInit*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A2Action));
        gendoc::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerClauseRule>(this, &InitializerClauseRule::PostAssignmentExpression));
        gendoc::parsing::NonterminalParser* initializerListNonterminalParser = GetNonterminal(ToUtf32("InitializerList"));
        initializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<InitializerClauseRule>(this, &InitializerClauseRule::PreInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AssignInit(context->fromAssignmentExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AssignInit(nullptr);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AssignInit(nullptr);
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
    void PreInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::codedom::AssignInit*>(context->value)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromAssignmentExpression() {}
        gendoc::codedom::AssignInit* value;
        gendoc::codedom::CppObject* fromAssignmentExpression;
    };
};

class Declarator::InitializerListRule : public gendoc::parsing::Rule
{
public:
    InitializerListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::codedom::AssignInit*"), ToUtf32("init")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> init_value = std::move(stack.top());
        context->init = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::AssignInit*>*>(init_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitializerListRule>(this, &InitializerListRule::A0Action));
        gendoc::parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal(ToUtf32("InitializerClause"));
        initializerClauseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<InitializerListRule>(this, &InitializerListRule::PostInitializerClause));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->init->Add(context->fromInitializerClause);
    }
    void PostInitializerClause(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromInitializerClause_value = std::move(stack.top());
            context->fromInitializerClause = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::AssignInit*>*>(fromInitializerClause_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): init(), fromInitializerClause() {}
        gendoc::codedom::AssignInit* init;
        gendoc::codedom::AssignInit* fromInitializerClause;
    };
};

void Declarator::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.code.Expression"));
    if (!grammar0)
    {
        grammar0 = gendoc::code::Expression::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.code.Declaration"));
    if (!grammar1)
    {
        grammar1 = gendoc::code::Declaration::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void Declarator::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ConstantExpression"), this, ToUtf32("Expression.ConstantExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeSpecifier"), this, ToUtf32("Declaration.TypeSpecifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("IdExpression"), this, ToUtf32("Expression.IdExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ExpressionList"), this, ToUtf32("Expression.ExpressionList")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("AssignmentExpression"), this, ToUtf32("Expression.AssignmentExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeName"), this, ToUtf32("Declaration.TypeName")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("CVQualifier"), this, ToUtf32("Declaration.CVQualifier")));
    AddRule(new InitDeclaratorListRule(ToUtf32("InitDeclaratorList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::EmptyParser()),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::ListParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("InitDeclarator"), ToUtf32("InitDeclarator"), 0)),
                        new gendoc::parsing::CharParser(',')))))));
    AddRule(new InitDeclaratorRule(ToUtf32("InitDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("Initializer"), ToUtf32("Initializer"), 0)))))));
    AddRule(new DeclaratorRule(ToUtf32("Declarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("DirectDeclarator"), ToUtf32("DirectDeclarator"), 0),
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("PtrOperator"), ToUtf32("PtrOperator"), 0),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)))))));
    AddRule(new DirectDeclaratorRule(ToUtf32("DirectDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("DeclaratorId"), ToUtf32("DeclaratorId"), 0),
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::CharParser('['),
                                        new gendoc::parsing::OptionalParser(
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 0))))),
                                    new gendoc::parsing::CharParser(']')),
                                new gendoc::parsing::CharParser('(')),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)),
                        new gendoc::parsing::CharParser(')')))))));
    AddRule(new DeclaratorIdRule(ToUtf32("DeclaratorId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("IdExpression"), ToUtf32("IdExpression"), 0)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0)))));
    AddRule(new TypeIdRule(ToUtf32("TypeId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                    new gendoc::parsing::NonterminalParser(ToUtf32("AbstractDeclarator"), ToUtf32("AbstractDeclarator"), 0))))))))));
    AddRule(new TypeRule(ToUtf32("Type"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::EmptyParser()),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::PositiveParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("TypeSpecifier"), ToUtf32("TypeSpecifier"), 0)))),
                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0))))))));
    AddRule(new TypeSpecifierSeqRule(ToUtf32("TypeSpecifierSeq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("TypeSpecifier"), ToUtf32("TypeSpecifier"), 0))))));
    AddRule(new AbstractDeclaratorRule(ToUtf32("AbstractDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("PtrOperator"), ToUtf32("PtrOperator"), 0),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("AbstractDeclarator"), ToUtf32("AbstractDeclarator"), 0))),
                    new gendoc::parsing::NonterminalParser(ToUtf32("DirectAbstractDeclarator"), ToUtf32("DirectAbstractDeclarator"), 0))))));
    AddRule(new DirectAbstractDeclaratorRule(ToUtf32("DirectAbstractDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('['),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 0))))),
                        new gendoc::parsing::CharParser(']')),
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('('),
                            new gendoc::parsing::NonterminalParser(ToUtf32("AbstractDeclarator"), ToUtf32("AbstractDeclarator"), 0)),
                        new gendoc::parsing::CharParser(')')))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("PtrOperator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('*'),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("CVQualifierSeq"), ToUtf32("CVQualifierSeq"), 0))),
            new gendoc::parsing::CharParser('&'))));
    AddRule(new CVQualifierSeqRule(ToUtf32("CVQualifierSeq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("CVQualifier"), ToUtf32("CVQualifier"), 0))))));
    AddRule(new InitializerRule(ToUtf32("Initializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('='),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("InitializerClause"), ToUtf32("InitializerClause"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('('),
                            new gendoc::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0)),
                        new gendoc::parsing::CharParser(')')))))));
    AddRule(new InitializerClauseRule(ToUtf32("InitializerClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0)),
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::CharParser('{')),
                        new gendoc::parsing::NonterminalParser(ToUtf32("InitializerList"), ToUtf32("InitializerList"), 1)),
                    new gendoc::parsing::CharParser('}'))),
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('{'),
                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                    new gendoc::parsing::CharParser('}'))))));
    AddRule(new InitializerListRule(ToUtf32("InitializerList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ListParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("InitializerClause"), ToUtf32("InitializerClause"), 0)),
            new gendoc::parsing::CharParser(','))));
}

} } // namespace gendoc.code
