#include "Enumeration.hpp"
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
#include <gendoc/cppparser/TypeExpr.hpp>
#include <gendoc/cppparser/Expression.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Enumeration* Enumeration::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Enumeration* Enumeration::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Enumeration* grammar(new Enumeration(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Enumeration::Enumeration(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Enumeration"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

EnumTypeNode* Enumeration::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    EnumTypeNode* result = *static_cast<gendoc::parsing::ValueObject<EnumTypeNode*>*>(value.get());
    stack.pop();
    return result;
}

class Enumeration::EnumDeclarationRule : public gendoc::parsing::Rule
{
public:
    EnumDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("EnumTypeNode*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<EnumTypeNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumDeclarationRule>(this, &EnumDeclarationRule::A0Action));
        gendoc::parsing::NonterminalParser* enumSpecifierNonterminalParser = GetNonterminal(ToUtf32("EnumSpecifier"));
        enumSpecifierNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumDeclarationRule>(this, &EnumDeclarationRule::PreEnumSpecifier));
        enumSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumDeclarationRule>(this, &EnumDeclarationRule::PostEnumSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEnumSpecifier;
    }
    void PreEnumSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEnumSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumSpecifier_value = std::move(stack.top());
            context->fromEnumSpecifier = *static_cast<gendoc::parsing::ValueObject<EnumTypeNode*>*>(fromEnumSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromEnumSpecifier() {}
        ParsingContext* ctx;
        EnumTypeNode* value;
        EnumTypeNode* fromEnumSpecifier;
    };
};

class Enumeration::OpaqueEnumDeclarationRule : public gendoc::parsing::Rule
{
public:
    OpaqueEnumDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("enumName")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("enumBase")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OpaqueEnumDeclarationRule>(this, &OpaqueEnumDeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OpaqueEnumDeclarationRule>(this, &OpaqueEnumDeclarationRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OpaqueEnumDeclarationRule>(this, &OpaqueEnumDeclarationRule::A2Action));
        gendoc::parsing::NonterminalParser* enumKeyNonterminalParser = GetNonterminal(ToUtf32("EnumKey"));
        enumKeyNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<OpaqueEnumDeclarationRule>(this, &OpaqueEnumDeclarationRule::PostEnumKey));
        gendoc::parsing::NonterminalParser* enumNameNonterminalParser = GetNonterminal(ToUtf32("EnumName"));
        enumNameNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<OpaqueEnumDeclarationRule>(this, &OpaqueEnumDeclarationRule::PreEnumName));
        enumNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<OpaqueEnumDeclarationRule>(this, &OpaqueEnumDeclarationRule::PostEnumName));
        gendoc::parsing::NonterminalParser* enumBaseNonterminalParser = GetNonterminal(ToUtf32("EnumBase"));
        enumBaseNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<OpaqueEnumDeclarationRule>(this, &OpaqueEnumDeclarationRule::PreEnumBase));
        enumBaseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<OpaqueEnumDeclarationRule>(this, &OpaqueEnumDeclarationRule::PostEnumBase));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EnumTypeNode(span, context->fromEnumKey, context->enumName.release(), context->enumBase.release());
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->enumName.reset(context->fromEnumName);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->enumBase.reset(context->fromEnumBase);
    }
    void PostEnumKey(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumKey_value = std::move(stack.top());
            context->fromEnumKey = *static_cast<gendoc::parsing::ValueObject<EnumKey>*>(fromEnumKey_value.get());
            stack.pop();
        }
    }
    void PreEnumName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEnumName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumName_value = std::move(stack.top());
            context->fromEnumName = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromEnumName_value.get());
            stack.pop();
        }
    }
    void PreEnumBase(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEnumBase(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumBase_value = std::move(stack.top());
            context->fromEnumBase = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromEnumBase_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), enumName(), enumBase(), fromEnumKey(), fromEnumName(), fromEnumBase() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> enumName;
        std::unique_ptr<Node> enumBase;
        EnumKey fromEnumKey;
        Node* fromEnumName;
        Node* fromEnumBase;
    };
};

class Enumeration::EnumSpecifierRule : public gendoc::parsing::Rule
{
public:
    EnumSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("EnumTypeNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<EnumTypeNode>"), ToUtf32("enumTypeNode")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("openBraceSpan")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("closeBraceSpan")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<EnumTypeNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumSpecifierRule>(this, &EnumSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumSpecifierRule>(this, &EnumSpecifierRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumSpecifierRule>(this, &EnumSpecifierRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumSpecifierRule>(this, &EnumSpecifierRule::A3Action));
        gendoc::parsing::NonterminalParser* enumHeadNonterminalParser = GetNonterminal(ToUtf32("EnumHead"));
        enumHeadNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumSpecifierRule>(this, &EnumSpecifierRule::PreEnumHead));
        enumHeadNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumSpecifierRule>(this, &EnumSpecifierRule::PostEnumHead));
        gendoc::parsing::NonterminalParser* enumeratorsNonterminalParser = GetNonterminal(ToUtf32("Enumerators"));
        enumeratorsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumSpecifierRule>(this, &EnumSpecifierRule::PreEnumerators));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->enumTypeNode.release();
        context->value->SetOpenBraceSpan(context->openBraceSpan);
        context->value->SetCloseBraceSpan(context->closeBraceSpan);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->enumTypeNode.reset(context->fromEnumHead);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->openBraceSpan = span;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->closeBraceSpan = span;
    }
    void PreEnumHead(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEnumHead(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumHead_value = std::move(stack.top());
            context->fromEnumHead = *static_cast<gendoc::parsing::ValueObject<EnumTypeNode*>*>(fromEnumHead_value.get());
            stack.pop();
        }
    }
    void PreEnumerators(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<EnumTypeNode*>(context->enumTypeNode.get())));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), enumTypeNode(), openBraceSpan(), closeBraceSpan(), fromEnumHead() {}
        ParsingContext* ctx;
        EnumTypeNode* value;
        std::unique_ptr<EnumTypeNode> enumTypeNode;
        Span openBraceSpan;
        Span closeBraceSpan;
        EnumTypeNode* fromEnumHead;
    };
};

class Enumeration::EnumHeadRule : public gendoc::parsing::Rule
{
public:
    EnumHeadRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("EnumTypeNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("enumName")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<EnumTypeNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumHeadRule>(this, &EnumHeadRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumHeadRule>(this, &EnumHeadRule::A1Action));
        gendoc::parsing::NonterminalParser* enumKeyNonterminalParser = GetNonterminal(ToUtf32("EnumKey"));
        enumKeyNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumHeadRule>(this, &EnumHeadRule::PostEnumKey));
        gendoc::parsing::NonterminalParser* enumNameNonterminalParser = GetNonterminal(ToUtf32("EnumName"));
        enumNameNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumHeadRule>(this, &EnumHeadRule::PreEnumName));
        enumNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumHeadRule>(this, &EnumHeadRule::PostEnumName));
        gendoc::parsing::NonterminalParser* enumBaseNonterminalParser = GetNonterminal(ToUtf32("EnumBase"));
        enumBaseNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumHeadRule>(this, &EnumHeadRule::PreEnumBase));
        enumBaseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumHeadRule>(this, &EnumHeadRule::PostEnumBase));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EnumTypeNode(span, context->fromEnumKey, context->enumName.release(), context->fromEnumBase);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->enumName.reset(context->fromEnumName);
    }
    void PostEnumKey(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumKey_value = std::move(stack.top());
            context->fromEnumKey = *static_cast<gendoc::parsing::ValueObject<EnumKey>*>(fromEnumKey_value.get());
            stack.pop();
        }
    }
    void PreEnumName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEnumName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumName_value = std::move(stack.top());
            context->fromEnumName = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromEnumName_value.get());
            stack.pop();
        }
    }
    void PreEnumBase(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEnumBase(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumBase_value = std::move(stack.top());
            context->fromEnumBase = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromEnumBase_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), enumName(), fromEnumKey(), fromEnumName(), fromEnumBase() {}
        ParsingContext* ctx;
        EnumTypeNode* value;
        std::unique_ptr<Node> enumName;
        EnumKey fromEnumKey;
        Node* fromEnumName;
        Node* fromEnumBase;
    };
};

class Enumeration::EnumKeyRule : public gendoc::parsing::Rule
{
public:
    EnumKeyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("EnumKey"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<EnumKey>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumKeyRule>(this, &EnumKeyRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumKeyRule>(this, &EnumKeyRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumKeyRule>(this, &EnumKeyRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = EnumKey::enumClass_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = EnumKey::enumStruct_;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = EnumKey::enum_;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        EnumKey value;
    };
};

class Enumeration::EnumNameRule : public gendoc::parsing::Rule
{
public:
    EnumNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumNameRule>(this, &EnumNameRule::A0Action));
        gendoc::parsing::NonterminalParser* qualifiedIdNodeNonterminalParser = GetNonterminal(ToUtf32("QualifiedIdNode"));
        qualifiedIdNodeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumNameRule>(this, &EnumNameRule::PreQualifiedIdNode));
        qualifiedIdNodeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumNameRule>(this, &EnumNameRule::PostQualifiedIdNode));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromQualifiedIdNode;
    }
    void PreQualifiedIdNode(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostQualifiedIdNode(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromQualifiedIdNode_value = std::move(stack.top());
            context->fromQualifiedIdNode = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromQualifiedIdNode_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromQualifiedIdNode() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromQualifiedIdNode;
    };
};

class Enumeration::EnumBaseRule : public gendoc::parsing::Rule
{
public:
    EnumBaseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumBaseRule>(this, &EnumBaseRule::A0Action));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumBaseRule>(this, &EnumBaseRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumBaseRule>(this, &EnumBaseRule::PostTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeExpr;
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

class Enumeration::EnumeratorsRule : public gendoc::parsing::Rule
{
public:
    EnumeratorsRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("EnumTypeNode*"), ToUtf32("enumTypeNode")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> enumTypeNode_value = std::move(stack.top());
        context->enumTypeNode = *static_cast<gendoc::parsing::ValueObject<EnumTypeNode*>*>(enumTypeNode_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumeratorsRule>(this, &EnumeratorsRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumeratorsRule>(this, &EnumeratorsRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumeratorsRule>(this, &EnumeratorsRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumeratorsRule>(this, &EnumeratorsRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumeratorsRule>(this, &EnumeratorsRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumeratorsRule>(this, &EnumeratorsRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumeratorsRule>(this, &EnumeratorsRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->enumTypeNode->AddEnumerator(context->s, context->fromleft);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->enumTypeNode->AddEnumerator(context->s, context->fromright);
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
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<EnumeratorNode*>*>(fromleft_value.get());
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
            context->fromright = *static_cast<gendoc::parsing::ValueObject<EnumeratorNode*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): enumTypeNode(), ctx(), s(), fromleft(), fromright() {}
        EnumTypeNode* enumTypeNode;
        ParsingContext* ctx;
        Span s;
        EnumeratorNode* fromleft;
        EnumeratorNode* fromright;
    };
};

class Enumeration::EnumeratorDefinitionRule : public gendoc::parsing::Rule
{
public:
    EnumeratorDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("EnumeratorNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("assignmentSignSpan")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("valueStr")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<EnumeratorNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumeratorDefinitionRule>(this, &EnumeratorDefinitionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumeratorDefinitionRule>(this, &EnumeratorDefinitionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumeratorDefinitionRule>(this, &EnumeratorDefinitionRule::A2Action));
        gendoc::parsing::NonterminalParser* enumeratorNonterminalParser = GetNonterminal(ToUtf32("Enumerator"));
        enumeratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumeratorDefinitionRule>(this, &EnumeratorDefinitionRule::PostEnumerator));
        gendoc::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<EnumeratorDefinitionRule>(this, &EnumeratorDefinitionRule::PreConstantExpression));
        constantExpressionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumeratorDefinitionRule>(this, &EnumeratorDefinitionRule::PostConstantExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EnumeratorNode(span, context->fromEnumerator, context->fromConstantExpression, context->valueStr);
        context->value->SetAssignmentSignSpan(context->assignmentSignSpan);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->assignmentSignSpan = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->valueStr = std::u32string(matchBegin, matchEnd);
    }
    void PostEnumerator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumerator_value = std::move(stack.top());
            context->fromEnumerator = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromEnumerator_value.get());
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
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), assignmentSignSpan(), valueStr(), fromEnumerator(), fromConstantExpression() {}
        ParsingContext* ctx;
        EnumeratorNode* value;
        Span assignmentSignSpan;
        std::u32string valueStr;
        std::u32string fromEnumerator;
        Node* fromConstantExpression;
    };
};

class Enumeration::EnumeratorRule : public gendoc::parsing::Rule
{
public:
    EnumeratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnumeratorRule>(this, &EnumeratorRule::A0Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<EnumeratorRule>(this, &EnumeratorRule::PostIdentifier));
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

void Enumeration::GetReferencedGrammars()
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
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.cppparser.TypeExpr"));
    if (!grammar2)
    {
        grammar2 = gendoc::cppparser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void Enumeration::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("QualifiedIdNode"), this, ToUtf32("Identifier.QualifiedIdNode")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ConstantExpression"), this, ToUtf32("Expression.ConstantExpression")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRule(new EnumDeclarationRule(ToUtf32("EnumDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("EnumSpecifier"), ToUtf32("EnumSpecifier"), 1),
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new OpaqueEnumDeclarationRule(ToUtf32("OpaqueEnumDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("EnumKey"), ToUtf32("EnumKey"), 0),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("EnumName"), ToUtf32("EnumName"), 1))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("EnumBase"), ToUtf32("EnumBase"), 1))))),
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new EnumSpecifierRule(ToUtf32("EnumSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("EnumHead"), ToUtf32("EnumHead"), 1)),
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::CharParser('{'))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Enumerators"), ToUtf32("Enumerators"), 2)),
                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::CharParser('}'))))))));
    AddRule(new EnumHeadRule(ToUtf32("EnumHead"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("EnumKey"), ToUtf32("EnumKey"), 0),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("EnumName"), ToUtf32("EnumName"), 1))),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("EnumBase"), ToUtf32("EnumBase"), 1)))))));
    AddRule(new EnumKeyRule(ToUtf32("EnumKey"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("enum")),
                            new gendoc::parsing::KeywordParser(ToUtf32("class"))))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("enum")),
                            new gendoc::parsing::KeywordParser(ToUtf32("struct")))))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::KeywordParser(ToUtf32("enum"))))));
    AddRule(new EnumNameRule(ToUtf32("EnumName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("QualifiedIdNode"), ToUtf32("QualifiedIdNode"), 1))));
    AddRule(new EnumBaseRule(ToUtf32("EnumBase"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser(':'),
                    new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))))));
    AddRule(new EnumeratorsRule(ToUtf32("Enumerators"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::EmptyParser()),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("EnumeratorDefinition"), 1)),
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::CharParser(','),
                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("EnumeratorDefinition"), 1)))))))))));
    AddRule(new EnumeratorDefinitionRule(ToUtf32("EnumeratorDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("Enumerator"), ToUtf32("Enumerator"), 0),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::CharParser('=')),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 1))))))))));
    AddRule(new EnumeratorRule(ToUtf32("Enumerator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))));
}

} } // namespace gendoc.cppparser
