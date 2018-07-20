#include "Declaration.hpp"
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
#include <gendoc/cppparser/CppSourceFileGrammar.hpp>
#include <gendoc/cppparser/Declarator.hpp>
#include <gendoc/cppparser/Function.hpp>
#include <gendoc/cppparser/Template.hpp>
#include <gendoc/cppparser/Class.hpp>
#include <gendoc/cppparser/Enumeration.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Declaration* Declaration::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Declaration* Declaration::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Declaration* grammar(new Declaration(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Declaration::Declaration(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Declaration"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

Node* Declaration::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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

class Declaration::DeclarationRule : public gendoc::parsing::Rule
{
public:
    DeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A9Action));
        gendoc::parsing::NonterminalParser* namespaceDefinitionNonterminalParser = GetNonterminal(ToUtf32("NamespaceDefinition"));
        namespaceDefinitionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreNamespaceDefinition));
        namespaceDefinitionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostNamespaceDefinition));
        gendoc::parsing::NonterminalParser* functionDefinitionNonterminalParser = GetNonterminal(ToUtf32("FunctionDefinition"));
        functionDefinitionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreFunctionDefinition));
        functionDefinitionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostFunctionDefinition));
        gendoc::parsing::NonterminalParser* specialMemberFunctionDefinitionNonterminalParser = GetNonterminal(ToUtf32("SpecialMemberFunctionDefinition"));
        specialMemberFunctionDefinitionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreSpecialMemberFunctionDefinition));
        specialMemberFunctionDefinitionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostSpecialMemberFunctionDefinition));
        gendoc::parsing::NonterminalParser* templateDeclarationNonterminalParser = GetNonterminal(ToUtf32("TemplateDeclaration"));
        templateDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreTemplateDeclaration));
        templateDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostTemplateDeclaration));
        gendoc::parsing::NonterminalParser* explicitInstantiationNonterminalParser = GetNonterminal(ToUtf32("ExplicitInstantiation"));
        explicitInstantiationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreExplicitInstantiation));
        explicitInstantiationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostExplicitInstantiation));
        gendoc::parsing::NonterminalParser* explicitSpecializationNonterminalParser = GetNonterminal(ToUtf32("ExplicitSpecialization"));
        explicitSpecializationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreExplicitSpecialization));
        explicitSpecializationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostExplicitSpecialization));
        gendoc::parsing::NonterminalParser* classDeclarationNonterminalParser = GetNonterminal(ToUtf32("ClassDeclaration"));
        classDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreClassDeclaration));
        classDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostClassDeclaration));
        gendoc::parsing::NonterminalParser* enumDeclarationNonterminalParser = GetNonterminal(ToUtf32("EnumDeclaration"));
        enumDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreEnumDeclaration));
        enumDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostEnumDeclaration));
        gendoc::parsing::NonterminalParser* linkageSpecificationNonterminalParser = GetNonterminal(ToUtf32("LinkageSpecification"));
        linkageSpecificationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreLinkageSpecification));
        linkageSpecificationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostLinkageSpecification));
        gendoc::parsing::NonterminalParser* blockDeclarationNonterminalParser = GetNonterminal(ToUtf32("BlockDeclaration"));
        blockDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreBlockDeclaration));
        blockDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostBlockDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNamespaceDefinition;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFunctionDefinition;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpecialMemberFunctionDefinition;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTemplateDeclaration;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExplicitInstantiation;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExplicitSpecialization;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClassDeclaration;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEnumDeclaration;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLinkageSpecification;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBlockDeclaration;
    }
    void PreNamespaceDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostNamespaceDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNamespaceDefinition_value = std::move(stack.top());
            context->fromNamespaceDefinition = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromNamespaceDefinition_value.get());
            stack.pop();
        }
    }
    void PreFunctionDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostFunctionDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFunctionDefinition_value = std::move(stack.top());
            context->fromFunctionDefinition = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromFunctionDefinition_value.get());
            stack.pop();
        }
    }
    void PreSpecialMemberFunctionDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostSpecialMemberFunctionDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSpecialMemberFunctionDefinition_value = std::move(stack.top());
            context->fromSpecialMemberFunctionDefinition = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromSpecialMemberFunctionDefinition_value.get());
            stack.pop();
        }
    }
    void PreTemplateDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTemplateDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTemplateDeclaration_value = std::move(stack.top());
            context->fromTemplateDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromTemplateDeclaration_value.get());
            stack.pop();
        }
    }
    void PreExplicitInstantiation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExplicitInstantiation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExplicitInstantiation_value = std::move(stack.top());
            context->fromExplicitInstantiation = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExplicitInstantiation_value.get());
            stack.pop();
        }
    }
    void PreExplicitSpecialization(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExplicitSpecialization(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromExplicitSpecialization_value = std::move(stack.top());
            context->fromExplicitSpecialization = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromExplicitSpecialization_value.get());
            stack.pop();
        }
    }
    void PreClassDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClassDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassDeclaration_value = std::move(stack.top());
            context->fromClassDeclaration = *static_cast<gendoc::parsing::ValueObject<ClassNode*>*>(fromClassDeclaration_value.get());
            stack.pop();
        }
    }
    void PreEnumDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEnumDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnumDeclaration_value = std::move(stack.top());
            context->fromEnumDeclaration = *static_cast<gendoc::parsing::ValueObject<EnumTypeNode*>*>(fromEnumDeclaration_value.get());
            stack.pop();
        }
    }
    void PreLinkageSpecification(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostLinkageSpecification(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLinkageSpecification_value = std::move(stack.top());
            context->fromLinkageSpecification = *static_cast<gendoc::parsing::ValueObject<LinkageSpecificationNode*>*>(fromLinkageSpecification_value.get());
            stack.pop();
        }
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
        Context(): ctx(), value(), fromNamespaceDefinition(), fromFunctionDefinition(), fromSpecialMemberFunctionDefinition(), fromTemplateDeclaration(), fromExplicitInstantiation(), fromExplicitSpecialization(), fromClassDeclaration(), fromEnumDeclaration(), fromLinkageSpecification(), fromBlockDeclaration() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromNamespaceDefinition;
        Node* fromFunctionDefinition;
        Node* fromSpecialMemberFunctionDefinition;
        Node* fromTemplateDeclaration;
        Node* fromExplicitInstantiation;
        Node* fromExplicitSpecialization;
        ClassNode* fromClassDeclaration;
        EnumTypeNode* fromEnumDeclaration;
        LinkageSpecificationNode* fromLinkageSpecification;
        Node* fromBlockDeclaration;
    };
};

class Declaration::NamespaceDefinitionRule : public gendoc::parsing::Rule
{
public:
    NamespaceDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::A1Action));
        gendoc::parsing::NonterminalParser* namedNamespaceDefinitionNonterminalParser = GetNonterminal(ToUtf32("NamedNamespaceDefinition"));
        namedNamespaceDefinitionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::PreNamedNamespaceDefinition));
        namedNamespaceDefinitionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::PostNamedNamespaceDefinition));
        gendoc::parsing::NonterminalParser* unnamedNamespaceDefinitionNonterminalParser = GetNonterminal(ToUtf32("UnnamedNamespaceDefinition"));
        unnamedNamespaceDefinitionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::PreUnnamedNamespaceDefinition));
        unnamedNamespaceDefinitionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::PostUnnamedNamespaceDefinition));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNamedNamespaceDefinition;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUnnamedNamespaceDefinition;
    }
    void PreNamedNamespaceDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostNamedNamespaceDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNamedNamespaceDefinition_value = std::move(stack.top());
            context->fromNamedNamespaceDefinition = *static_cast<gendoc::parsing::ValueObject<NamespaceNode*>*>(fromNamedNamespaceDefinition_value.get());
            stack.pop();
        }
    }
    void PreUnnamedNamespaceDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostUnnamedNamespaceDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUnnamedNamespaceDefinition_value = std::move(stack.top());
            context->fromUnnamedNamespaceDefinition = *static_cast<gendoc::parsing::ValueObject<NamespaceNode*>*>(fromUnnamedNamespaceDefinition_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromNamedNamespaceDefinition(), fromUnnamedNamespaceDefinition() {}
        ParsingContext* ctx;
        Node* value;
        NamespaceNode* fromNamedNamespaceDefinition;
        NamespaceNode* fromUnnamedNamespaceDefinition;
    };
};

class Declaration::NamedNamespaceDefinitionRule : public gendoc::parsing::Rule
{
public:
    NamedNamespaceDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("NamespaceNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<NamespaceNode>"), ToUtf32("ns")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<NamespaceNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamedNamespaceDefinitionRule>(this, &NamedNamespaceDefinitionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamedNamespaceDefinitionRule>(this, &NamedNamespaceDefinitionRule::A1Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamedNamespaceDefinitionRule>(this, &NamedNamespaceDefinitionRule::PostIdentifier));
        gendoc::parsing::NonterminalParser* declarationsNonterminalParser = GetNonterminal(ToUtf32("Declarations"));
        declarationsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NamedNamespaceDefinitionRule>(this, &NamedNamespaceDefinitionRule::PreDeclarations));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ns.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ns.reset(new NamespaceNode(span, context->fromIdentifier));
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
    void PreDeclarations(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<NamespaceNode*>(context->ns.get())));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), ns(), fromIdentifier() {}
        ParsingContext* ctx;
        NamespaceNode* value;
        std::unique_ptr<NamespaceNode> ns;
        std::u32string fromIdentifier;
    };
};

class Declaration::UnnamedNamespaceDefinitionRule : public gendoc::parsing::Rule
{
public:
    UnnamedNamespaceDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("NamespaceNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<NamespaceNode>"), ToUtf32("ns")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<NamespaceNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnnamedNamespaceDefinitionRule>(this, &UnnamedNamespaceDefinitionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnnamedNamespaceDefinitionRule>(this, &UnnamedNamespaceDefinitionRule::A1Action));
        gendoc::parsing::NonterminalParser* declarationsNonterminalParser = GetNonterminal(ToUtf32("Declarations"));
        declarationsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UnnamedNamespaceDefinitionRule>(this, &UnnamedNamespaceDefinitionRule::PreDeclarations));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ns.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ns.reset(new NamespaceNode(span));
    }
    void PreDeclarations(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<NamespaceNode*>(context->ns.get())));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), ns() {}
        ParsingContext* ctx;
        NamespaceNode* value;
        std::unique_ptr<NamespaceNode> ns;
    };
};

class Declaration::BlockDeclarationRule : public gendoc::parsing::Rule
{
public:
    BlockDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A6Action));
        gendoc::parsing::NonterminalParser* simpleDeclarationNonterminalParser = GetNonterminal(ToUtf32("SimpleDeclaration"));
        simpleDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PreSimpleDeclaration));
        simpleDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostSimpleDeclaration));
        gendoc::parsing::NonterminalParser* aliasDeclarationNonterminalParser = GetNonterminal(ToUtf32("AliasDeclaration"));
        aliasDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PreAliasDeclaration));
        aliasDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostAliasDeclaration));
        gendoc::parsing::NonterminalParser* usingDirectiveNonterminalParser = GetNonterminal(ToUtf32("UsingDirective"));
        usingDirectiveNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PreUsingDirective));
        usingDirectiveNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostUsingDirective));
        gendoc::parsing::NonterminalParser* usingDeclarationNonterminalParser = GetNonterminal(ToUtf32("UsingDeclaration"));
        usingDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PreUsingDeclaration));
        usingDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostUsingDeclaration));
        gendoc::parsing::NonterminalParser* opaqueEnumDeclarationNonterminalParser = GetNonterminal(ToUtf32("OpaqueEnumDeclaration"));
        opaqueEnumDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PreOpaqueEnumDeclaration));
        opaqueEnumDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostOpaqueEnumDeclaration));
        gendoc::parsing::NonterminalParser* typedefDeclarationNonterminalParser = GetNonterminal(ToUtf32("TypedefDeclaration"));
        typedefDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PreTypedefDeclaration));
        typedefDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostTypedefDeclaration));
        gendoc::parsing::NonterminalParser* forwardClassDeclarationNonterminalParser = GetNonterminal(ToUtf32("ForwardClassDeclaration"));
        forwardClassDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PreForwardClassDeclaration));
        forwardClassDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostForwardClassDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleDeclaration;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAliasDeclaration;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUsingDirective;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUsingDeclaration;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOpaqueEnumDeclaration;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypedefDeclaration;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromForwardClassDeclaration;
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
    void PreAliasDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostAliasDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAliasDeclaration_value = std::move(stack.top());
            context->fromAliasDeclaration = *static_cast<gendoc::parsing::ValueObject<AliasDeclarationNode*>*>(fromAliasDeclaration_value.get());
            stack.pop();
        }
    }
    void PreUsingDirective(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostUsingDirective(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUsingDirective_value = std::move(stack.top());
            context->fromUsingDirective = *static_cast<gendoc::parsing::ValueObject<UsingDirectiveNode*>*>(fromUsingDirective_value.get());
            stack.pop();
        }
    }
    void PreUsingDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostUsingDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUsingDeclaration_value = std::move(stack.top());
            context->fromUsingDeclaration = *static_cast<gendoc::parsing::ValueObject<UsingDeclarationNode*>*>(fromUsingDeclaration_value.get());
            stack.pop();
        }
    }
    void PreOpaqueEnumDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostOpaqueEnumDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOpaqueEnumDeclaration_value = std::move(stack.top());
            context->fromOpaqueEnumDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromOpaqueEnumDeclaration_value.get());
            stack.pop();
        }
    }
    void PreTypedefDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypedefDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTypedefDeclaration_value = std::move(stack.top());
            context->fromTypedefDeclaration = *static_cast<gendoc::parsing::ValueObject<TypedefNode*>*>(fromTypedefDeclaration_value.get());
            stack.pop();
        }
    }
    void PreForwardClassDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostForwardClassDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromForwardClassDeclaration_value = std::move(stack.top());
            context->fromForwardClassDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromForwardClassDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromSimpleDeclaration(), fromAliasDeclaration(), fromUsingDirective(), fromUsingDeclaration(), fromOpaqueEnumDeclaration(), fromTypedefDeclaration(), fromForwardClassDeclaration() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromSimpleDeclaration;
        AliasDeclarationNode* fromAliasDeclaration;
        UsingDirectiveNode* fromUsingDirective;
        UsingDeclarationNode* fromUsingDeclaration;
        Node* fromOpaqueEnumDeclaration;
        TypedefNode* fromTypedefDeclaration;
        Node* fromForwardClassDeclaration;
    };
};

class Declaration::SimpleDeclarationRule : public gendoc::parsing::Rule
{
public:
    SimpleDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("typeExpr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("initDeclarator")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleDeclarationRule>(this, &SimpleDeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleDeclarationRule>(this, &SimpleDeclarationRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SimpleDeclarationRule>(this, &SimpleDeclarationRule::A2Action));
        gendoc::parsing::NonterminalParser* declSpecifiersNonterminalParser = GetNonterminal(ToUtf32("DeclSpecifiers"));
        declSpecifiersNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SimpleDeclarationRule>(this, &SimpleDeclarationRule::PostDeclSpecifiers));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SimpleDeclarationRule>(this, &SimpleDeclarationRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SimpleDeclarationRule>(this, &SimpleDeclarationRule::PostTypeExpr));
        gendoc::parsing::NonterminalParser* initDeclaratorNonterminalParser = GetNonterminal(ToUtf32("InitDeclarator"));
        initDeclaratorNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SimpleDeclarationRule>(this, &SimpleDeclarationRule::PreInitDeclarator));
        initDeclaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SimpleDeclarationRule>(this, &SimpleDeclarationRule::PostInitDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SimpleDeclarationNode(span, context->fromDeclSpecifiers, context->typeExpr.release(), context->initDeclarator.release());
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeExpr.reset(context->fromTypeExpr);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->initDeclarator.reset(context->fromInitDeclarator);
    }
    void PostDeclSpecifiers(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclSpecifiers_value = std::move(stack.top());
            context->fromDeclSpecifiers = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromDeclSpecifiers_value.get());
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
    void PreInitDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInitDeclarator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromInitDeclarator_value = std::move(stack.top());
            context->fromInitDeclarator = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromInitDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), typeExpr(), initDeclarator(), fromDeclSpecifiers(), fromTypeExpr(), fromInitDeclarator() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> typeExpr;
        std::unique_ptr<Node> initDeclarator;
        Specifier fromDeclSpecifiers;
        Node* fromTypeExpr;
        Node* fromInitDeclarator;
    };
};

class Declaration::DeclSpecifiersRule : public gendoc::parsing::Rule
{
public:
    DeclSpecifiersRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Specifier"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Specifier>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclSpecifiersRule>(this, &DeclSpecifiersRule::A0Action));
        gendoc::parsing::NonterminalParser* declSpecifierNonterminalParser = GetNonterminal(ToUtf32("DeclSpecifier"));
        declSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclSpecifiersRule>(this, &DeclSpecifiersRule::PostDeclSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | context->fromDeclSpecifier;
    }
    void PostDeclSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDeclSpecifier_value = std::move(stack.top());
            context->fromDeclSpecifier = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromDeclSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromDeclSpecifier() {}
        Specifier value;
        Specifier fromDeclSpecifier;
    };
};

class Declaration::DeclSpecifierRule : public gendoc::parsing::Rule
{
public:
    DeclSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Specifier"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Specifier>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A5Action));
        gendoc::parsing::NonterminalParser* storageClassSpecifierNonterminalParser = GetNonterminal(ToUtf32("StorageClassSpecifier"));
        storageClassSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclSpecifierRule>(this, &DeclSpecifierRule::PostStorageClassSpecifier));
        gendoc::parsing::NonterminalParser* functionSpecifierNonterminalParser = GetNonterminal(ToUtf32("FunctionSpecifier"));
        functionSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclSpecifierRule>(this, &DeclSpecifierRule::PostFunctionSpecifier));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DeclSpecifierRule>(this, &DeclSpecifierRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromStorageClassSpecifier;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFunctionSpecifier;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::friend_;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::constExpr;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::extension;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::extension;
    }
    void PostStorageClassSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStorageClassSpecifier_value = std::move(stack.top());
            context->fromStorageClassSpecifier = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromStorageClassSpecifier_value.get());
            stack.pop();
        }
    }
    void PostFunctionSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFunctionSpecifier_value = std::move(stack.top());
            context->fromFunctionSpecifier = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromFunctionSpecifier_value.get());
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
        Context(): value(), fromStorageClassSpecifier(), fromFunctionSpecifier(), fromIdentifier() {}
        Specifier value;
        Specifier fromStorageClassSpecifier;
        Specifier fromFunctionSpecifier;
        std::u32string fromIdentifier;
    };
};

class Declaration::StorageClassSpecifierRule : public gendoc::parsing::Rule
{
public:
    StorageClassSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Specifier"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Specifier>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StorageClassSpecifierRule>(this, &StorageClassSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StorageClassSpecifierRule>(this, &StorageClassSpecifierRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StorageClassSpecifierRule>(this, &StorageClassSpecifierRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StorageClassSpecifierRule>(this, &StorageClassSpecifierRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StorageClassSpecifierRule>(this, &StorageClassSpecifierRule::A4Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::register_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::static_;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::threadLocal;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::extern_;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::mutable_;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Specifier value;
    };
};

class Declaration::FunctionSpecifierRule : public gendoc::parsing::Rule
{
public:
    FunctionSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Specifier"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Specifier>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FunctionSpecifierRule>(this, &FunctionSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FunctionSpecifierRule>(this, &FunctionSpecifierRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FunctionSpecifierRule>(this, &FunctionSpecifierRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::inline_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::virtual_;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::explicit_;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Specifier value;
    };
};

class Declaration::AliasDeclarationRule : public gendoc::parsing::Rule
{
public:
    AliasDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("AliasDeclarationNode*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<AliasDeclarationNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AliasDeclarationRule>(this, &AliasDeclarationRule::A0Action));
        gendoc::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AliasDeclarationRule>(this, &AliasDeclarationRule::PostIdentifier));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<AliasDeclarationRule>(this, &AliasDeclarationRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<AliasDeclarationRule>(this, &AliasDeclarationRule::PostTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AliasDeclarationNode(span, context->fromIdentifier, context->fromTypeExpr);
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
        Context(): ctx(), value(), fromIdentifier(), fromTypeExpr() {}
        ParsingContext* ctx;
        AliasDeclarationNode* value;
        std::u32string fromIdentifier;
        Node* fromTypeExpr;
    };
};

class Declaration::UsingDirectiveRule : public gendoc::parsing::Rule
{
public:
    UsingDirectiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("UsingDirectiveNode*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<UsingDirectiveNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UsingDirectiveRule>(this, &UsingDirectiveRule::A0Action));
        gendoc::parsing::NonterminalParser* qualifiedIdNodeNonterminalParser = GetNonterminal(ToUtf32("QualifiedIdNode"));
        qualifiedIdNodeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UsingDirectiveRule>(this, &UsingDirectiveRule::PreQualifiedIdNode));
        qualifiedIdNodeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UsingDirectiveRule>(this, &UsingDirectiveRule::PostQualifiedIdNode));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UsingDirectiveNode(span, context->fromQualifiedIdNode);
        context->ctx->GetSourceFileNode()->AddUsingDirectiveOrDeclaration(context->value);
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
        UsingDirectiveNode* value;
        Node* fromQualifiedIdNode;
    };
};

class Declaration::UsingDeclarationRule : public gendoc::parsing::Rule
{
public:
    UsingDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("UsingDeclarationNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("qid")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<UsingDeclarationNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UsingDeclarationRule>(this, &UsingDeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UsingDeclarationRule>(this, &UsingDeclarationRule::A1Action));
        gendoc::parsing::NonterminalParser* qualifiedIdNodeNonterminalParser = GetNonterminal(ToUtf32("QualifiedIdNode"));
        qualifiedIdNodeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<UsingDeclarationRule>(this, &UsingDeclarationRule::PreQualifiedIdNode));
        qualifiedIdNodeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<UsingDeclarationRule>(this, &UsingDeclarationRule::PostQualifiedIdNode));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UsingDeclarationNode(span, context->qid.release());
        context->ctx->GetSourceFileNode()->AddUsingDirectiveOrDeclaration(context->value);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->qid.reset(context->fromQualifiedIdNode);
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
        Context(): ctx(), value(), qid(), fromQualifiedIdNode() {}
        ParsingContext* ctx;
        UsingDeclarationNode* value;
        std::unique_ptr<Node> qid;
        Node* fromQualifiedIdNode;
    };
};

class Declaration::TypedefDeclarationRule : public gendoc::parsing::Rule
{
public:
    TypedefDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("TypedefNode*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<TypedefNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TypedefDeclarationRule>(this, &TypedefDeclarationRule::A0Action));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TypedefDeclarationRule>(this, &TypedefDeclarationRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypedefDeclarationRule>(this, &TypedefDeclarationRule::PostTypeExpr));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<TypedefDeclarationRule>(this, &TypedefDeclarationRule::PreDeclarator));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<TypedefDeclarationRule>(this, &TypedefDeclarationRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypedefNode(span, context->fromTypeExpr, context->fromDeclarator);
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
        Context(): ctx(), value(), fromTypeExpr(), fromDeclarator() {}
        ParsingContext* ctx;
        TypedefNode* value;
        Node* fromTypeExpr;
        Node* fromDeclarator;
    };
};

class Declaration::LinkageSpecificationRule : public gendoc::parsing::Rule
{
public:
    LinkageSpecificationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("LinkageSpecificationNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<LinkageSpecificationNode>"), ToUtf32("ls")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<LinkageSpecificationNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LinkageSpecificationRule>(this, &LinkageSpecificationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LinkageSpecificationRule>(this, &LinkageSpecificationRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LinkageSpecificationRule>(this, &LinkageSpecificationRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LinkageSpecificationRule>(this, &LinkageSpecificationRule::A3Action));
        gendoc::parsing::NonterminalParser* stringLiteralNonterminalParser = GetNonterminal(ToUtf32("StringLiteral"));
        stringLiteralNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LinkageSpecificationRule>(this, &LinkageSpecificationRule::PostStringLiteral));
        gendoc::parsing::NonterminalParser* d1NonterminalParser = GetNonterminal(ToUtf32("d1"));
        d1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LinkageSpecificationRule>(this, &LinkageSpecificationRule::Pred1));
        d1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LinkageSpecificationRule>(this, &LinkageSpecificationRule::Postd1));
        gendoc::parsing::NonterminalParser* d2NonterminalParser = GetNonterminal(ToUtf32("d2"));
        d2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<LinkageSpecificationRule>(this, &LinkageSpecificationRule::Pred2));
        d2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<LinkageSpecificationRule>(this, &LinkageSpecificationRule::Postd2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ls.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ls.reset(new LinkageSpecificationNode(span, context->fromStringLiteral));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ls->AddDeclaration(span, context->fromd1);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ls->AddDeclaration(span, context->fromd2);
    }
    void PostStringLiteral(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromStringLiteral_value = std::move(stack.top());
            context->fromStringLiteral = *static_cast<gendoc::parsing::ValueObject<StringLiteralNode*>*>(fromStringLiteral_value.get());
            stack.pop();
        }
    }
    void Pred1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postd1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromd1_value = std::move(stack.top());
            context->fromd1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromd1_value.get());
            stack.pop();
        }
    }
    void Pred2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postd2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromd2_value = std::move(stack.top());
            context->fromd2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromd2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), ls(), fromStringLiteral(), fromd1(), fromd2() {}
        ParsingContext* ctx;
        LinkageSpecificationNode* value;
        std::unique_ptr<LinkageSpecificationNode> ls;
        StringLiteralNode* fromStringLiteral;
        Node* fromd1;
        Node* fromd2;
    };
};

void Declaration::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Enumeration"));
    if (!grammar0)
    {
        grammar0 = gendoc::cppparser::Enumeration::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Template"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::Template::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Class"));
    if (!grammar2)
    {
        grammar2 = gendoc::cppparser::Class::Create(pd);
    }
    AddGrammarReference(grammar2);
    gendoc::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Literal"));
    if (!grammar3)
    {
        grammar3 = gendoc::cppparser::Literal::Create(pd);
    }
    AddGrammarReference(grammar3);
    gendoc::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Identifier"));
    if (!grammar4)
    {
        grammar4 = gendoc::cppparser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar4);
    gendoc::parsing::Grammar* grammar5 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Declarator"));
    if (!grammar5)
    {
        grammar5 = gendoc::cppparser::Declarator::Create(pd);
    }
    AddGrammarReference(grammar5);
    gendoc::parsing::Grammar* grammar6 = pd->GetGrammar(ToUtf32("gendoc.cppparser.TypeExpr"));
    if (!grammar6)
    {
        grammar6 = gendoc::cppparser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar6);
    gendoc::parsing::Grammar* grammar7 = pd->GetGrammar(ToUtf32("gendoc.cppparser.CppSourceFile"));
    if (!grammar7)
    {
        grammar7 = gendoc::cppparser::CppSourceFile::Create(pd);
    }
    AddGrammarReference(grammar7);
    gendoc::parsing::Grammar* grammar8 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Function"));
    if (!grammar8)
    {
        grammar8 = gendoc::cppparser::Function::Create(pd);
    }
    AddGrammarReference(grammar8);
}

void Declaration::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ExplicitSpecialization"), this, ToUtf32("Template.ExplicitSpecialization")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("SpecialMemberFunctionDefinition"), this, ToUtf32("Class.SpecialMemberFunctionDefinition")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Declarations"), this, ToUtf32("CppSourceFile.Declarations")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("InitDeclarator"), this, ToUtf32("Declarator.InitDeclarator")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Declarator"), this, ToUtf32("Declarator.Declarator")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TemplateDeclaration"), this, ToUtf32("Template.TemplateDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("QualifiedIdNode"), this, ToUtf32("Identifier.QualifiedIdNode")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("FunctionDefinition"), this, ToUtf32("Function.FunctionDefinition")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ExplicitInstantiation"), this, ToUtf32("Template.ExplicitInstantiation")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ClassDeclaration"), this, ToUtf32("Class.ClassDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("ForwardClassDeclaration"), this, ToUtf32("Class.ForwardClassDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("EnumDeclaration"), this, ToUtf32("Enumeration.EnumDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("OpaqueEnumDeclaration"), this, ToUtf32("Enumeration.OpaqueEnumDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("StringLiteral"), this, ToUtf32("Literal.StringLiteral")));
    AddRule(new DeclarationRule(ToUtf32("Declaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                new gendoc::parsing::NonterminalParser(ToUtf32("NamespaceDefinition"), ToUtf32("NamespaceDefinition"), 1)),
                                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("FunctionDefinition"), ToUtf32("FunctionDefinition"), 1))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("SpecialMemberFunctionDefinition"), ToUtf32("SpecialMemberFunctionDefinition"), 1))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("TemplateDeclaration"), ToUtf32("TemplateDeclaration"), 1))),
                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("ExplicitInstantiation"), ToUtf32("ExplicitInstantiation"), 1))),
                            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("ExplicitSpecialization"), ToUtf32("ExplicitSpecialization"), 1))),
                        new gendoc::parsing::ActionParser(ToUtf32("A6"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("ClassDeclaration"), ToUtf32("ClassDeclaration"), 1))),
                    new gendoc::parsing::ActionParser(ToUtf32("A7"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("EnumDeclaration"), ToUtf32("EnumDeclaration"), 1))),
                new gendoc::parsing::ActionParser(ToUtf32("A8"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("LinkageSpecification"), ToUtf32("LinkageSpecification"), 1))),
            new gendoc::parsing::ActionParser(ToUtf32("A9"),
                new gendoc::parsing::NonterminalParser(ToUtf32("BlockDeclaration"), ToUtf32("BlockDeclaration"), 1)))));
    AddRule(new NamespaceDefinitionRule(ToUtf32("NamespaceDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::NonterminalParser(ToUtf32("NamedNamespaceDefinition"), ToUtf32("NamedNamespaceDefinition"), 1)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("UnnamedNamespaceDefinition"), ToUtf32("UnnamedNamespaceDefinition"), 1)))));
    AddRule(new NamedNamespaceDefinitionRule(ToUtf32("NamedNamespaceDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("namespace")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::CharParser('{'))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Declarations"), ToUtf32("Declarations"), 2)),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser('}')))))));
    AddRule(new UnnamedNamespaceDefinitionRule(ToUtf32("UnnamedNamespaceDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("namespace")),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::CharParser('{'))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Declarations"), ToUtf32("Declarations"), 2)),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser('}')))))));
    AddRule(new BlockDeclarationRule(ToUtf32("BlockDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("SimpleDeclaration"), ToUtf32("SimpleDeclaration"), 1)),
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("AliasDeclaration"), ToUtf32("AliasDeclaration"), 1))),
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("UsingDirective"), ToUtf32("UsingDirective"), 1))),
                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("UsingDeclaration"), ToUtf32("UsingDeclaration"), 1))),
                    new gendoc::parsing::ActionParser(ToUtf32("A4"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("OpaqueEnumDeclaration"), ToUtf32("OpaqueEnumDeclaration"), 1))),
                new gendoc::parsing::ActionParser(ToUtf32("A5"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("TypedefDeclaration"), ToUtf32("TypedefDeclaration"), 1))),
            new gendoc::parsing::ActionParser(ToUtf32("A6"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ForwardClassDeclaration"), ToUtf32("ForwardClassDeclaration"), 1)))));
    AddRule(new SimpleDeclarationRule(ToUtf32("SimpleDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("DeclSpecifiers"), ToUtf32("DeclSpecifiers"), 0),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))),
                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("InitDeclarator"), ToUtf32("InitDeclarator"), 1))),
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new DeclSpecifiersRule(ToUtf32("DeclSpecifiers"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("DeclSpecifier"), ToUtf32("DeclSpecifier"), 0))))));
    AddRule(new DeclSpecifierRule(ToUtf32("DeclSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("StorageClassSpecifier"), ToUtf32("StorageClassSpecifier"), 0)),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("FunctionSpecifier"), ToUtf32("FunctionSpecifier"), 0))),
                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                            new gendoc::parsing::KeywordParser(ToUtf32("friend")))),
                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                        new gendoc::parsing::KeywordParser(ToUtf32("constexpr")))),
                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("__declspec")),
                                    new gendoc::parsing::CharParser('(')),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)),
                            new gendoc::parsing::CharParser(')'))))),
            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                new gendoc::parsing::KeywordParser(ToUtf32("__thread"))))));
    AddRule(new StorageClassSpecifierRule(ToUtf32("StorageClassSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::KeywordParser(ToUtf32("register"))),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::KeywordParser(ToUtf32("static")))),
                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                        new gendoc::parsing::KeywordParser(ToUtf32("thread_local")))),
                new gendoc::parsing::ActionParser(ToUtf32("A3"),
                    new gendoc::parsing::KeywordParser(ToUtf32("extern")))),
            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                new gendoc::parsing::KeywordParser(ToUtf32("mutable"))))));
    AddRule(new FunctionSpecifierRule(ToUtf32("FunctionSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::KeywordParser(ToUtf32("inline"))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::KeywordParser(ToUtf32("virtual")))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::KeywordParser(ToUtf32("explicit"))))));
    AddRule(new AliasDeclarationRule(ToUtf32("AliasDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::KeywordParser(ToUtf32("using")),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)),
                            new gendoc::parsing::CharParser('=')),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser(';')))))));
    AddRule(new UsingDirectiveRule(ToUtf32("UsingDirective"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("using")),
                            new gendoc::parsing::KeywordParser(ToUtf32("namespace"))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("QualifiedIdNode"), ToUtf32("QualifiedIdNode"), 1))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser(';')))))));
    AddRule(new UsingDeclarationRule(ToUtf32("UsingDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("using")),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("QualifiedIdNode"), ToUtf32("QualifiedIdNode"), 1))),
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new TypedefDeclarationRule(ToUtf32("TypedefDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::KeywordParser(ToUtf32("typedef")),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 1))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser(';')))))));
    AddRule(new LinkageSpecificationRule(ToUtf32("LinkageSpecification"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("extern")),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("StringLiteral"), ToUtf32("StringLiteral"), 0))),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::CharParser('{'),
                                    new gendoc::parsing::KleeneStarParser(
                                        new gendoc::parsing::GroupingParser(
                                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("d1"), ToUtf32("Declaration"), 1))))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::CharParser('}'))),
                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("d2"), ToUtf32("Declaration"), 1)))))))));
}

} } // namespace gendoc.cppparser
