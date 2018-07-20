#include "Class.hpp"
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
#include <gendoc/cppparser/Template.hpp>
#include <gendoc/cppparser/TypeExpr.hpp>
#include <gendoc/cppparser/Declarator.hpp>
#include <gendoc/cppparser/Function.hpp>
#include <gendoc/cppparser/Enumeration.hpp>
#include <gendoc/cppparser/Expression.hpp>
#include <gendoc/cppparser/Identifier.hpp>
#include <gendoc/cppparser/Literal.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Class* Class::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Class* Class::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Class* grammar(new Class(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Class::Class(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Class"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

ClassNode* Class::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    ClassNode* result = *static_cast<gendoc::parsing::ValueObject<ClassNode*>*>(value.get());
    stack.pop();
    return result;
}

class Class::ClassDeclarationRule : public gendoc::parsing::Rule
{
public:
    ClassDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ClassNode*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ClassNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassDeclarationRule>(this, &ClassDeclarationRule::A0Action));
        gendoc::parsing::NonterminalParser* classSpecifierNonterminalParser = GetNonterminal(ToUtf32("ClassSpecifier"));
        classSpecifierNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ClassDeclarationRule>(this, &ClassDeclarationRule::PreClassSpecifier));
        classSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ClassDeclarationRule>(this, &ClassDeclarationRule::PostClassSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClassSpecifier;
    }
    void PreClassSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClassSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassSpecifier_value = std::move(stack.top());
            context->fromClassSpecifier = *static_cast<gendoc::parsing::ValueObject<ClassNode*>*>(fromClassSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromClassSpecifier() {}
        ParsingContext* ctx;
        ClassNode* value;
        ClassNode* fromClassSpecifier;
    };
};

class Class::ForwardClassDeclarationRule : public gendoc::parsing::Rule
{
public:
    ForwardClassDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("cn")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForwardClassDeclarationRule>(this, &ForwardClassDeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ForwardClassDeclarationRule>(this, &ForwardClassDeclarationRule::A1Action));
        gendoc::parsing::NonterminalParser* classKeyNonterminalParser = GetNonterminal(ToUtf32("ClassKey"));
        classKeyNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForwardClassDeclarationRule>(this, &ForwardClassDeclarationRule::PostClassKey));
        gendoc::parsing::NonterminalParser* classNameNonterminalParser = GetNonterminal(ToUtf32("ClassName"));
        classNameNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ForwardClassDeclarationRule>(this, &ForwardClassDeclarationRule::PreClassName));
        classNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ForwardClassDeclarationRule>(this, &ForwardClassDeclarationRule::PostClassName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ForwardClassDeclarationNode(span, context->fromClassKey, context->cn.release());
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->cn.reset(context->fromClassName);
    }
    void PostClassKey(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassKey_value = std::move(stack.top());
            context->fromClassKey = *static_cast<gendoc::parsing::ValueObject<ClassKey>*>(fromClassKey_value.get());
            stack.pop();
        }
    }
    void PreClassName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClassName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassName_value = std::move(stack.top());
            context->fromClassName = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromClassName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), cn(), fromClassKey(), fromClassName() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> cn;
        ClassKey fromClassKey;
        Node* fromClassName;
    };
};

class Class::ClassSpecifierRule : public gendoc::parsing::Rule
{
public:
    ClassSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ClassNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<ClassNode>"), ToUtf32("classNode")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ClassNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassSpecifierRule>(this, &ClassSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassSpecifierRule>(this, &ClassSpecifierRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassSpecifierRule>(this, &ClassSpecifierRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassSpecifierRule>(this, &ClassSpecifierRule::A3Action));
        gendoc::parsing::NonterminalParser* classHeadNonterminalParser = GetNonterminal(ToUtf32("ClassHead"));
        classHeadNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ClassSpecifierRule>(this, &ClassSpecifierRule::PreClassHead));
        classHeadNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ClassSpecifierRule>(this, &ClassSpecifierRule::PostClassHead));
        gendoc::parsing::NonterminalParser* memberSpecificationsNonterminalParser = GetNonterminal(ToUtf32("MemberSpecifications"));
        memberSpecificationsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ClassSpecifierRule>(this, &ClassSpecifierRule::PreMemberSpecifications));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->classNode.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->classNode.reset(context->fromClassHead);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->classNode->SetOpenBraceSpan(span);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->classNode->SetCloseBraceSpan(span);
    }
    void PreClassHead(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClassHead(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassHead_value = std::move(stack.top());
            context->fromClassHead = *static_cast<gendoc::parsing::ValueObject<ClassNode*>*>(fromClassHead_value.get());
            stack.pop();
        }
    }
    void PreMemberSpecifications(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ClassNode*>(context->classNode.get())));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), classNode(), fromClassHead() {}
        ParsingContext* ctx;
        ClassNode* value;
        std::unique_ptr<ClassNode> classNode;
        ClassNode* fromClassHead;
    };
};

class Class::MemberSpecificationsRule : public gendoc::parsing::Rule
{
public:
    MemberSpecificationsRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ClassNode*"), ToUtf32("classNode")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> classNode_value = std::move(stack.top());
        context->classNode = *static_cast<gendoc::parsing::ValueObject<ClassNode*>*>(classNode_value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* memberSpecificationNonterminalParser = GetNonterminal(ToUtf32("MemberSpecification"));
        memberSpecificationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberSpecificationsRule>(this, &MemberSpecificationsRule::PreMemberSpecification));
    }
    void PreMemberSpecification(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ClassNode*>(context->classNode)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), classNode() {}
        ParsingContext* ctx;
        ClassNode* classNode;
    };
};

class Class::MemberSpecificationRule : public gendoc::parsing::Rule
{
public:
    MemberSpecificationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ClassNode*"), ToUtf32("classNode")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> classNode_value = std::move(stack.top());
        context->classNode = *static_cast<gendoc::parsing::ValueObject<ClassNode*>*>(classNode_value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<gendoc::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberSpecificationRule>(this, &MemberSpecificationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberSpecificationRule>(this, &MemberSpecificationRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberSpecificationRule>(this, &MemberSpecificationRule::A2Action));
        gendoc::parsing::NonterminalParser* accessSpecifierNonterminalParser = GetNonterminal(ToUtf32("AccessSpecifier"));
        accessSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberSpecificationRule>(this, &MemberSpecificationRule::PostAccessSpecifier));
        gendoc::parsing::NonterminalParser* m1NonterminalParser = GetNonterminal(ToUtf32("m1"));
        m1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberSpecificationRule>(this, &MemberSpecificationRule::Prem1));
        m1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberSpecificationRule>(this, &MemberSpecificationRule::Postm1));
        gendoc::parsing::NonterminalParser* m2NonterminalParser = GetNonterminal(ToUtf32("m2"));
        m2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberSpecificationRule>(this, &MemberSpecificationRule::Prem2));
        m2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberSpecificationRule>(this, &MemberSpecificationRule::Postm2));
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
        context->classNode->AddDeclaration(context->s, new MemberAccessDeclarationNode(context->s, context->fromAccessSpecifier, context->fromm1, true));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->classNode->AddDeclaration(context->s, new MemberAccessDeclarationNode(context->s, context->classNode->DefaultAccess(), context->fromm2, false));
    }
    void PostAccessSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAccessSpecifier_value = std::move(stack.top());
            context->fromAccessSpecifier = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromAccessSpecifier_value.get());
            stack.pop();
        }
    }
    void Prem1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postm1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromm1_value = std::move(stack.top());
            context->fromm1 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromm1_value.get());
            stack.pop();
        }
    }
    void Prem2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postm2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromm2_value = std::move(stack.top());
            context->fromm2 = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromm2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), classNode(), s(), fromAccessSpecifier(), fromm1(), fromm2() {}
        ParsingContext* ctx;
        ClassNode* classNode;
        Span s;
        Specifier fromAccessSpecifier;
        Node* fromm1;
        Node* fromm2;
    };
};

class Class::MemberDeclarationRule : public gendoc::parsing::Rule
{
public:
    MemberDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("typeExpr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("declarator")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("initializer")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A9Action));
        gendoc::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A10Action));
        gendoc::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A11Action));
        gendoc::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A12Action));
        gendoc::parsing::ActionParser* a13ActionParser = GetAction(ToUtf32("A13"));
        a13ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberDeclarationRule>(this, &MemberDeclarationRule::A13Action));
        gendoc::parsing::NonterminalParser* declSpecifiersNonterminalParser = GetNonterminal(ToUtf32("DeclSpecifiers"));
        declSpecifiersNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostDeclSpecifiers));
        gendoc::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostTypeExpr));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreDeclarator));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostDeclarator));
        gendoc::parsing::NonterminalParser* virtPureSpecifiersNonterminalParser = GetNonterminal(ToUtf32("VirtPureSpecifiers"));
        virtPureSpecifiersNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreVirtPureSpecifiers));
        virtPureSpecifiersNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostVirtPureSpecifiers));
        gendoc::parsing::NonterminalParser* braceOrEqualInitializerNonterminalParser = GetNonterminal(ToUtf32("BraceOrEqualInitializer"));
        braceOrEqualInitializerNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreBraceOrEqualInitializer));
        braceOrEqualInitializerNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostBraceOrEqualInitializer));
        gendoc::parsing::NonterminalParser* specialMemberFunctionDeclarationNonterminalParser = GetNonterminal(ToUtf32("SpecialMemberFunctionDeclaration"));
        specialMemberFunctionDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreSpecialMemberFunctionDeclaration));
        specialMemberFunctionDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostSpecialMemberFunctionDeclaration));
        gendoc::parsing::NonterminalParser* functionDefinitionNonterminalParser = GetNonterminal(ToUtf32("FunctionDefinition"));
        functionDefinitionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreFunctionDefinition));
        functionDefinitionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostFunctionDefinition));
        gendoc::parsing::NonterminalParser* specialMemberFunctionDefinitionNonterminalParser = GetNonterminal(ToUtf32("SpecialMemberFunctionDefinition"));
        specialMemberFunctionDefinitionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreSpecialMemberFunctionDefinition));
        specialMemberFunctionDefinitionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostSpecialMemberFunctionDefinition));
        gendoc::parsing::NonterminalParser* usingDeclarationNonterminalParser = GetNonterminal(ToUtf32("UsingDeclaration"));
        usingDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreUsingDeclaration));
        usingDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostUsingDeclaration));
        gendoc::parsing::NonterminalParser* typedefDeclarationNonterminalParser = GetNonterminal(ToUtf32("TypedefDeclaration"));
        typedefDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreTypedefDeclaration));
        typedefDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostTypedefDeclaration));
        gendoc::parsing::NonterminalParser* templateDeclarationNonterminalParser = GetNonterminal(ToUtf32("TemplateDeclaration"));
        templateDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreTemplateDeclaration));
        templateDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostTemplateDeclaration));
        gendoc::parsing::NonterminalParser* classDeclarationNonterminalParser = GetNonterminal(ToUtf32("ClassDeclaration"));
        classDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreClassDeclaration));
        classDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostClassDeclaration));
        gendoc::parsing::NonterminalParser* enumDeclarationNonterminalParser = GetNonterminal(ToUtf32("EnumDeclaration"));
        enumDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreEnumDeclaration));
        enumDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostEnumDeclaration));
        gendoc::parsing::NonterminalParser* aliasDeclarationNonterminalParser = GetNonterminal(ToUtf32("AliasDeclaration"));
        aliasDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreAliasDeclaration));
        aliasDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostAliasDeclaration));
        gendoc::parsing::NonterminalParser* forwardClassDeclarationNonterminalParser = GetNonterminal(ToUtf32("ForwardClassDeclaration"));
        forwardClassDeclarationNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PreForwardClassDeclaration));
        forwardClassDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberDeclarationRule>(this, &MemberDeclarationRule::PostForwardClassDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MemberDeclarationNode(span, context->fromDeclSpecifiers | context->fromVirtPureSpecifiers, context->typeExpr.release(), context->declarator.release(), context->initializer.release());
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
        context->initializer.reset(context->fromBraceOrEqualInitializer);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpecialMemberFunctionDeclaration;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFunctionDefinition;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpecialMemberFunctionDefinition;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUsingDeclaration;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypedefDeclaration;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTemplateDeclaration;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClassDeclaration;
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEnumDeclaration;
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAliasDeclaration;
    }
    void A13Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromForwardClassDeclaration;
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
    void PreVirtPureSpecifiers(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<bool>(IsFunctionDeclarator(context->declarator.get()))));
    }
    void PostVirtPureSpecifiers(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromVirtPureSpecifiers_value = std::move(stack.top());
            context->fromVirtPureSpecifiers = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromVirtPureSpecifiers_value.get());
            stack.pop();
        }
    }
    void PreBraceOrEqualInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostBraceOrEqualInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBraceOrEqualInitializer_value = std::move(stack.top());
            context->fromBraceOrEqualInitializer = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromBraceOrEqualInitializer_value.get());
            stack.pop();
        }
    }
    void PreSpecialMemberFunctionDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostSpecialMemberFunctionDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSpecialMemberFunctionDeclaration_value = std::move(stack.top());
            context->fromSpecialMemberFunctionDeclaration = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromSpecialMemberFunctionDeclaration_value.get());
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
        Context(): ctx(), value(), typeExpr(), declarator(), initializer(), fromDeclSpecifiers(), fromTypeExpr(), fromDeclarator(), fromVirtPureSpecifiers(), fromBraceOrEqualInitializer(), fromSpecialMemberFunctionDeclaration(), fromFunctionDefinition(), fromSpecialMemberFunctionDefinition(), fromUsingDeclaration(), fromTypedefDeclaration(), fromTemplateDeclaration(), fromClassDeclaration(), fromEnumDeclaration(), fromAliasDeclaration(), fromForwardClassDeclaration() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> typeExpr;
        std::unique_ptr<Node> declarator;
        std::unique_ptr<Node> initializer;
        Specifier fromDeclSpecifiers;
        Node* fromTypeExpr;
        Node* fromDeclarator;
        Specifier fromVirtPureSpecifiers;
        Node* fromBraceOrEqualInitializer;
        Node* fromSpecialMemberFunctionDeclaration;
        Node* fromFunctionDefinition;
        Node* fromSpecialMemberFunctionDefinition;
        UsingDeclarationNode* fromUsingDeclaration;
        TypedefNode* fromTypedefDeclaration;
        Node* fromTemplateDeclaration;
        ClassNode* fromClassDeclaration;
        EnumTypeNode* fromEnumDeclaration;
        AliasDeclarationNode* fromAliasDeclaration;
        Node* fromForwardClassDeclaration;
    };
};

class Class::SpecialMemberFunctionDeclarationRule : public gendoc::parsing::Rule
{
public:
    SpecialMemberFunctionDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("declarator")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpecialMemberFunctionDeclarationRule>(this, &SpecialMemberFunctionDeclarationRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpecialMemberFunctionDeclarationRule>(this, &SpecialMemberFunctionDeclarationRule::A1Action));
        gendoc::parsing::NonterminalParser* declSpecifiersNonterminalParser = GetNonterminal(ToUtf32("DeclSpecifiers"));
        declSpecifiersNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SpecialMemberFunctionDeclarationRule>(this, &SpecialMemberFunctionDeclarationRule::PostDeclSpecifiers));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SpecialMemberFunctionDeclarationRule>(this, &SpecialMemberFunctionDeclarationRule::PreDeclarator));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SpecialMemberFunctionDeclarationRule>(this, &SpecialMemberFunctionDeclarationRule::PostDeclarator));
        gendoc::parsing::NonterminalParser* virtPureSpecifiersNonterminalParser = GetNonterminal(ToUtf32("VirtPureSpecifiers"));
        virtPureSpecifiersNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SpecialMemberFunctionDeclarationRule>(this, &SpecialMemberFunctionDeclarationRule::PreVirtPureSpecifiers));
        virtPureSpecifiersNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SpecialMemberFunctionDeclarationRule>(this, &SpecialMemberFunctionDeclarationRule::PostVirtPureSpecifiers));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MemberDeclarationNode(span, context->fromDeclSpecifiers | context->fromVirtPureSpecifiers, nullptr, context->declarator.release(), nullptr);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->declarator.reset(context->fromDeclarator);
        pass = IsFunctionDeclarator(context->declarator.get());
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
    void PreVirtPureSpecifiers(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<bool>(true)));
    }
    void PostVirtPureSpecifiers(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromVirtPureSpecifiers_value = std::move(stack.top());
            context->fromVirtPureSpecifiers = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromVirtPureSpecifiers_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), declarator(), fromDeclSpecifiers(), fromDeclarator(), fromVirtPureSpecifiers() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> declarator;
        Specifier fromDeclSpecifiers;
        Node* fromDeclarator;
        Specifier fromVirtPureSpecifiers;
    };
};

class Class::ClassHeadRule : public gendoc::parsing::Rule
{
public:
    ClassHeadRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ClassNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("cn")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ClassNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassHeadRule>(this, &ClassHeadRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassHeadRule>(this, &ClassHeadRule::A1Action));
        gendoc::parsing::NonterminalParser* classKeyNonterminalParser = GetNonterminal(ToUtf32("ClassKey"));
        classKeyNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ClassHeadRule>(this, &ClassHeadRule::PostClassKey));
        gendoc::parsing::NonterminalParser* classNameNonterminalParser = GetNonterminal(ToUtf32("ClassName"));
        classNameNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ClassHeadRule>(this, &ClassHeadRule::PreClassName));
        classNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ClassHeadRule>(this, &ClassHeadRule::PostClassName));
        gendoc::parsing::NonterminalParser* classVirtSpecifiersNonterminalParser = GetNonterminal(ToUtf32("ClassVirtSpecifiers"));
        classVirtSpecifiersNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ClassHeadRule>(this, &ClassHeadRule::PostClassVirtSpecifiers));
        gendoc::parsing::NonterminalParser* baseClauseNonterminalParser = GetNonterminal(ToUtf32("BaseClause"));
        baseClauseNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ClassHeadRule>(this, &ClassHeadRule::PreBaseClause));
        baseClauseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ClassHeadRule>(this, &ClassHeadRule::PostBaseClause));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ClassNode(span, context->fromClassKey, context->cn.release(), context->fromClassVirtSpecifiers, context->fromBaseClause);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->cn.reset(context->fromClassName);
    }
    void PostClassKey(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassKey_value = std::move(stack.top());
            context->fromClassKey = *static_cast<gendoc::parsing::ValueObject<ClassKey>*>(fromClassKey_value.get());
            stack.pop();
        }
    }
    void PreClassName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClassName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassName_value = std::move(stack.top());
            context->fromClassName = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromClassName_value.get());
            stack.pop();
        }
    }
    void PostClassVirtSpecifiers(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassVirtSpecifiers_value = std::move(stack.top());
            context->fromClassVirtSpecifiers = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromClassVirtSpecifiers_value.get());
            stack.pop();
        }
    }
    void PreBaseClause(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostBaseClause(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBaseClause_value = std::move(stack.top());
            context->fromBaseClause = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromBaseClause_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), cn(), fromClassKey(), fromClassName(), fromClassVirtSpecifiers(), fromBaseClause() {}
        ParsingContext* ctx;
        ClassNode* value;
        std::unique_ptr<Node> cn;
        ClassKey fromClassKey;
        Node* fromClassName;
        Specifier fromClassVirtSpecifiers;
        Node* fromBaseClause;
    };
};

class Class::ClassKeyRule : public gendoc::parsing::Rule
{
public:
    ClassKeyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("ClassKey"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ClassKey>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassKeyRule>(this, &ClassKeyRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassKeyRule>(this, &ClassKeyRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassKeyRule>(this, &ClassKeyRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ClassKey::class_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ClassKey::struct_;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ClassKey::union_;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        ClassKey value;
    };
};

class Class::ClassNameRule : public gendoc::parsing::Rule
{
public:
    ClassNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassNameRule>(this, &ClassNameRule::A0Action));
        gendoc::parsing::NonterminalParser* qualifiedIdNodeNonterminalParser = GetNonterminal(ToUtf32("QualifiedIdNode"));
        qualifiedIdNodeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ClassNameRule>(this, &ClassNameRule::PreQualifiedIdNode));
        qualifiedIdNodeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ClassNameRule>(this, &ClassNameRule::PostQualifiedIdNode));
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

class Class::ClassVirtSpecifiersRule : public gendoc::parsing::Rule
{
public:
    ClassVirtSpecifiersRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassVirtSpecifiersRule>(this, &ClassVirtSpecifiersRule::A0Action));
        gendoc::parsing::NonterminalParser* classVirtSpecifierNonterminalParser = GetNonterminal(ToUtf32("ClassVirtSpecifier"));
        classVirtSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ClassVirtSpecifiersRule>(this, &ClassVirtSpecifiersRule::PostClassVirtSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | context->fromClassVirtSpecifier;
    }
    void PostClassVirtSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassVirtSpecifier_value = std::move(stack.top());
            context->fromClassVirtSpecifier = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromClassVirtSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromClassVirtSpecifier() {}
        Specifier value;
        Specifier fromClassVirtSpecifier;
    };
};

class Class::ClassVirtSpecifierRule : public gendoc::parsing::Rule
{
public:
    ClassVirtSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassVirtSpecifierRule>(this, &ClassVirtSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClassVirtSpecifierRule>(this, &ClassVirtSpecifierRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::final_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
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

class Class::BaseClauseRule : public gendoc::parsing::Rule
{
public:
    BaseClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BaseClauseRule>(this, &BaseClauseRule::A0Action));
        gendoc::parsing::NonterminalParser* baseClassSpecifierListNonterminalParser = GetNonterminal(ToUtf32("BaseClassSpecifierList"));
        baseClassSpecifierListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BaseClauseRule>(this, &BaseClauseRule::PreBaseClassSpecifierList));
        baseClassSpecifierListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BaseClauseRule>(this, &BaseClauseRule::PostBaseClassSpecifierList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBaseClassSpecifierList;
    }
    void PreBaseClassSpecifierList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostBaseClassSpecifierList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBaseClassSpecifierList_value = std::move(stack.top());
            context->fromBaseClassSpecifierList = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromBaseClassSpecifierList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromBaseClassSpecifierList() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromBaseClassSpecifierList;
    };
};

class Class::BaseClassSpecifierListRule : public gendoc::parsing::Rule
{
public:
    BaseClassSpecifierListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BaseClassSpecifierListRule>(this, &BaseClassSpecifierListRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BaseClassSpecifierListRule>(this, &BaseClassSpecifierListRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BaseClassSpecifierListRule>(this, &BaseClassSpecifierListRule::A2Action));
        gendoc::parsing::NonterminalParser* b1NonterminalParser = GetNonterminal(ToUtf32("b1"));
        b1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BaseClassSpecifierListRule>(this, &BaseClassSpecifierListRule::Preb1));
        b1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BaseClassSpecifierListRule>(this, &BaseClassSpecifierListRule::Postb1));
        gendoc::parsing::NonterminalParser* b2NonterminalParser = GetNonterminal(ToUtf32("b2"));
        b2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BaseClassSpecifierListRule>(this, &BaseClassSpecifierListRule::Preb2));
        b2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BaseClassSpecifierListRule>(this, &BaseClassSpecifierListRule::Postb2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->node.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node.reset(context->fromb1);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->node.reset(new BaseClassSpecifierSequenceNode(context->s, context->node.release(), context->fromb2));
    }
    void Preb1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postb1(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromb1_value = std::move(stack.top());
            context->fromb1 = *static_cast<gendoc::parsing::ValueObject<BaseClassSpecifierNode*>*>(fromb1_value.get());
            stack.pop();
        }
    }
    void Preb2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postb2(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromb2_value = std::move(stack.top());
            context->fromb2 = *static_cast<gendoc::parsing::ValueObject<BaseClassSpecifierNode*>*>(fromb2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), node(), s(), fromb1(), fromb2() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> node;
        Span s;
        BaseClassSpecifierNode* fromb1;
        BaseClassSpecifierNode* fromb2;
    };
};

class Class::BaseClassSpecifierRule : public gendoc::parsing::Rule
{
public:
    BaseClassSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("BaseClassSpecifierNode*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<BaseClassSpecifierNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BaseClassSpecifierRule>(this, &BaseClassSpecifierRule::A0Action));
        gendoc::parsing::NonterminalParser* baseSpecifiersNonterminalParser = GetNonterminal(ToUtf32("BaseSpecifiers"));
        baseSpecifiersNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BaseClassSpecifierRule>(this, &BaseClassSpecifierRule::PostBaseSpecifiers));
        gendoc::parsing::NonterminalParser* classNameNonterminalParser = GetNonterminal(ToUtf32("ClassName"));
        classNameNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<BaseClassSpecifierRule>(this, &BaseClassSpecifierRule::PreClassName));
        classNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BaseClassSpecifierRule>(this, &BaseClassSpecifierRule::PostClassName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BaseClassSpecifierNode(span, context->fromBaseSpecifiers, context->fromClassName);
    }
    void PostBaseSpecifiers(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBaseSpecifiers_value = std::move(stack.top());
            context->fromBaseSpecifiers = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromBaseSpecifiers_value.get());
            stack.pop();
        }
    }
    void PreClassName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClassName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClassName_value = std::move(stack.top());
            context->fromClassName = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromClassName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromBaseSpecifiers(), fromClassName() {}
        ParsingContext* ctx;
        BaseClassSpecifierNode* value;
        Specifier fromBaseSpecifiers;
        Node* fromClassName;
    };
};

class Class::BaseSpecifiersRule : public gendoc::parsing::Rule
{
public:
    BaseSpecifiersRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BaseSpecifiersRule>(this, &BaseSpecifiersRule::A0Action));
        gendoc::parsing::NonterminalParser* baseSpecifierNonterminalParser = GetNonterminal(ToUtf32("BaseSpecifier"));
        baseSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BaseSpecifiersRule>(this, &BaseSpecifiersRule::PostBaseSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | context->fromBaseSpecifier;
    }
    void PostBaseSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBaseSpecifier_value = std::move(stack.top());
            context->fromBaseSpecifier = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromBaseSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromBaseSpecifier() {}
        Specifier value;
        Specifier fromBaseSpecifier;
    };
};

class Class::BaseSpecifierRule : public gendoc::parsing::Rule
{
public:
    BaseSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BaseSpecifierRule>(this, &BaseSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BaseSpecifierRule>(this, &BaseSpecifierRule::A1Action));
        gendoc::parsing::NonterminalParser* accessSpecifierNonterminalParser = GetNonterminal(ToUtf32("AccessSpecifier"));
        accessSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BaseSpecifierRule>(this, &BaseSpecifierRule::PostAccessSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::virtual_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAccessSpecifier;
    }
    void PostAccessSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAccessSpecifier_value = std::move(stack.top());
            context->fromAccessSpecifier = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromAccessSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromAccessSpecifier() {}
        Specifier value;
        Specifier fromAccessSpecifier;
    };
};

class Class::AccessSpecifierRule : public gendoc::parsing::Rule
{
public:
    AccessSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AccessSpecifierRule>(this, &AccessSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AccessSpecifierRule>(this, &AccessSpecifierRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AccessSpecifierRule>(this, &AccessSpecifierRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::public_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::protected_;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifier::private_;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        Specifier value;
    };
};

class Class::VirtPureSpecifiersRule : public gendoc::parsing::Rule
{
public:
    VirtPureSpecifiersRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("bool"), ToUtf32("functionMember")));
        SetValueTypeName(ToUtf32("Specifier"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> functionMember_value = std::move(stack.top());
        context->functionMember = *static_cast<gendoc::parsing::ValueObject<bool>*>(functionMember_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<VirtPureSpecifiersRule>(this, &VirtPureSpecifiersRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<VirtPureSpecifiersRule>(this, &VirtPureSpecifiersRule::A1Action));
        gendoc::parsing::NonterminalParser* virtSpecifierNonterminalParser = GetNonterminal(ToUtf32("VirtSpecifier"));
        virtSpecifierNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<VirtPureSpecifiersRule>(this, &VirtPureSpecifiersRule::PreVirtSpecifier));
        virtSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<VirtPureSpecifiersRule>(this, &VirtPureSpecifiersRule::PostVirtSpecifier));
        gendoc::parsing::NonterminalParser* pureSpecifierNonterminalParser = GetNonterminal(ToUtf32("PureSpecifier"));
        pureSpecifierNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<VirtPureSpecifiersRule>(this, &VirtPureSpecifiersRule::PrePureSpecifier));
        pureSpecifierNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<VirtPureSpecifiersRule>(this, &VirtPureSpecifiersRule::PostPureSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | context->fromVirtSpecifier;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | context->fromPureSpecifier;
    }
    void PreVirtSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<bool>(context->functionMember)));
    }
    void PostVirtSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromVirtSpecifier_value = std::move(stack.top());
            context->fromVirtSpecifier = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromVirtSpecifier_value.get());
            stack.pop();
        }
    }
    void PrePureSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<bool>(context->functionMember)));
    }
    void PostPureSpecifier(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPureSpecifier_value = std::move(stack.top());
            context->fromPureSpecifier = *static_cast<gendoc::parsing::ValueObject<Specifier>*>(fromPureSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): functionMember(), value(), fromVirtSpecifier(), fromPureSpecifier() {}
        bool functionMember;
        Specifier value;
        Specifier fromVirtSpecifier;
        Specifier fromPureSpecifier;
    };
};

class Class::VirtSpecifierRule : public gendoc::parsing::Rule
{
public:
    VirtSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("bool"), ToUtf32("functionMember")));
        SetValueTypeName(ToUtf32("Specifier"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> functionMember_value = std::move(stack.top());
        context->functionMember = *static_cast<gendoc::parsing::ValueObject<bool>*>(functionMember_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<VirtSpecifierRule>(this, &VirtSpecifierRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<VirtSpecifierRule>(this, &VirtSpecifierRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<VirtSpecifierRule>(this, &VirtSpecifierRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        pass = context->functionMember;
        context->value = Specifier::override_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        pass = context->functionMember;
        context->value = Specifier::final_;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        pass = context->functionMember;
        context->value = Specifier::new_;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): functionMember(), value() {}
        bool functionMember;
        Specifier value;
    };
};

class Class::PureSpecifierRule : public gendoc::parsing::Rule
{
public:
    PureSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("bool"), ToUtf32("functionMember")));
        SetValueTypeName(ToUtf32("Specifier"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> functionMember_value = std::move(stack.top());
        context->functionMember = *static_cast<gendoc::parsing::ValueObject<bool>*>(functionMember_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PureSpecifierRule>(this, &PureSpecifierRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        pass = context->functionMember;
        context->value = Specifier::pure_;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): functionMember(), value() {}
        bool functionMember;
        Specifier value;
    };
};

class Class::SpecialMemberFunctionDefinitionRule : public gendoc::parsing::Rule
{
public:
    SpecialMemberFunctionDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("declarator")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("ctorInitializer")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Specifier"), ToUtf32("defaultOrDeleteSpecifier")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::A4Action));
        gendoc::parsing::NonterminalParser* declSpecifiersNonterminalParser = GetNonterminal(ToUtf32("DeclSpecifiers"));
        declSpecifiersNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::PostDeclSpecifiers));
        gendoc::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::PreDeclarator));
        declaratorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::PostDeclarator));
        gendoc::parsing::NonterminalParser* ctorInitializerNonterminalParser = GetNonterminal(ToUtf32("CtorInitializer"));
        ctorInitializerNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::PreCtorInitializer));
        ctorInitializerNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::PostCtorInitializer));
        gendoc::parsing::NonterminalParser* functionBodyNonterminalParser = GetNonterminal(ToUtf32("FunctionBody"));
        functionBodyNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::PreFunctionBody));
        functionBodyNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SpecialMemberFunctionDefinitionRule>(this, &SpecialMemberFunctionDefinitionRule::PostFunctionBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpecialMemberFunctionNode(span, context->fromDeclSpecifiers | context->defaultOrDeleteSpecifier, context->declarator.release(), context->ctorInitializer.release(), context->fromFunctionBody);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->declarator.reset(context->fromDeclarator);
        pass = IsFunctionDeclarator(context->declarator.get());
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctorInitializer.reset(context->fromCtorInitializer);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->defaultOrDeleteSpecifier = Specifier::default_;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->defaultOrDeleteSpecifier = Specifier::delete_;
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
    void PreCtorInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCtorInitializer(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCtorInitializer_value = std::move(stack.top());
            context->fromCtorInitializer = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromCtorInitializer_value.get());
            stack.pop();
        }
    }
    void PreFunctionBody(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostFunctionBody(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFunctionBody_value = std::move(stack.top());
            context->fromFunctionBody = *static_cast<gendoc::parsing::ValueObject<CompoundStatementNode*>*>(fromFunctionBody_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), declarator(), ctorInitializer(), defaultOrDeleteSpecifier(), fromDeclSpecifiers(), fromDeclarator(), fromCtorInitializer(), fromFunctionBody() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> declarator;
        std::unique_ptr<Node> ctorInitializer;
        Specifier defaultOrDeleteSpecifier;
        Specifier fromDeclSpecifiers;
        Node* fromDeclarator;
        Node* fromCtorInitializer;
        CompoundStatementNode* fromFunctionBody;
    };
};

class Class::CtorInitializerRule : public gendoc::parsing::Rule
{
public:
    CtorInitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CtorInitializerRule>(this, &CtorInitializerRule::A0Action));
        gendoc::parsing::NonterminalParser* memberInitializerListNonterminalParser = GetNonterminal(ToUtf32("MemberInitializerList"));
        memberInitializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<CtorInitializerRule>(this, &CtorInitializerRule::PreMemberInitializerList));
        memberInitializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CtorInitializerRule>(this, &CtorInitializerRule::PostMemberInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CtorInitializerNode(span, context->fromMemberInitializerList);
    }
    void PreMemberInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostMemberInitializerList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromMemberInitializerList_value = std::move(stack.top());
            context->fromMemberInitializerList = *static_cast<gendoc::parsing::ValueObject<Node*>*>(fromMemberInitializerList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), fromMemberInitializerList() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromMemberInitializerList;
    };
};

class Class::MemberInitializerListRule : public gendoc::parsing::Rule
{
public:
    MemberInitializerListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("list")));
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberInitializerListRule>(this, &MemberInitializerListRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberInitializerListRule>(this, &MemberInitializerListRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberInitializerListRule>(this, &MemberInitializerListRule::A2Action));
        gendoc::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberInitializerListRule>(this, &MemberInitializerListRule::Preleft));
        leftNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberInitializerListRule>(this, &MemberInitializerListRule::Postleft));
        gendoc::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberInitializerListRule>(this, &MemberInitializerListRule::Preright));
        rightNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberInitializerListRule>(this, &MemberInitializerListRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->list.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->list.reset(context->fromleft);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->list.reset(new MemberInitializerSequenceNode(span, context->list.release(), context->fromright));
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
            context->fromleft = *static_cast<gendoc::parsing::ValueObject<MemberInitializerNode*>*>(fromleft_value.get());
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
            context->fromright = *static_cast<gendoc::parsing::ValueObject<MemberInitializerNode*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): ctx(), value(), list(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> list;
        MemberInitializerNode* fromleft;
        MemberInitializerNode* fromright;
    };
};

class Class::MemberInitializerRule : public gendoc::parsing::Rule
{
public:
    MemberInitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("MemberInitializerNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("id")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("exprListSpan")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<MemberInitializerNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberInitializerRule>(this, &MemberInitializerRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberInitializerRule>(this, &MemberInitializerRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberInitializerRule>(this, &MemberInitializerRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberInitializerRule>(this, &MemberInitializerRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberInitializerRule>(this, &MemberInitializerRule::A4Action));
        gendoc::parsing::NonterminalParser* id1NonterminalParser = GetNonterminal(ToUtf32("id1"));
        id1NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberInitializerRule>(this, &MemberInitializerRule::Preid1));
        id1NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberInitializerRule>(this, &MemberInitializerRule::Postid1));
        gendoc::parsing::NonterminalParser* argsNonterminalParser = GetNonterminal(ToUtf32("args"));
        argsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberInitializerRule>(this, &MemberInitializerRule::Preargs));
        argsNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberInitializerRule>(this, &MemberInitializerRule::Postargs));
        gendoc::parsing::NonterminalParser* id2NonterminalParser = GetNonterminal(ToUtf32("id2"));
        id2NonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberInitializerRule>(this, &MemberInitializerRule::Preid2));
        id2NonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberInitializerRule>(this, &MemberInitializerRule::Postid2));
        gendoc::parsing::NonterminalParser* bracedInitializerListNonterminalParser = GetNonterminal(ToUtf32("BracedInitializerList"));
        bracedInitializerListNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberInitializerRule>(this, &MemberInitializerRule::PreBracedInitializerList));
        bracedInitializerListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberInitializerRule>(this, &MemberInitializerRule::PostBracedInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MemberInitializerNode(span, context->id.release(), new ExpressionListInitializerNode(context->exprListSpan, context->fromargs));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->id.reset(context->fromid1);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->exprListSpan = span;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MemberInitializerNode(span, context->id.release(), context->fromBracedInitializerList);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->id.reset(context->fromid2);
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
        Context(): ctx(), value(), id(), exprListSpan(), fromid1(), fromargs(), fromid2(), fromBracedInitializerList() {}
        ParsingContext* ctx;
        MemberInitializerNode* value;
        std::unique_ptr<Node> id;
        Span exprListSpan;
        Node* fromid1;
        Node* fromargs;
        Node* fromid2;
        BracedInitializerListNode* fromBracedInitializerList;
    };
};

class Class::MemberInitializerIdRule : public gendoc::parsing::Rule
{
public:
    MemberInitializerIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MemberInitializerIdRule>(this, &MemberInitializerIdRule::A0Action));
        gendoc::parsing::NonterminalParser* qualifiedIdNodeNonterminalParser = GetNonterminal(ToUtf32("QualifiedIdNode"));
        qualifiedIdNodeNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<MemberInitializerIdRule>(this, &MemberInitializerIdRule::PreQualifiedIdNode));
        qualifiedIdNodeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MemberInitializerIdRule>(this, &MemberInitializerIdRule::PostQualifiedIdNode));
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

void Class::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Declaration"));
    if (!grammar0)
    {
        grammar0 = gendoc::cppparser::Declaration::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Enumeration"));
    if (!grammar1)
    {
        grammar1 = gendoc::cppparser::Enumeration::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Template"));
    if (!grammar2)
    {
        grammar2 = gendoc::cppparser::Template::Create(pd);
    }
    AddGrammarReference(grammar2);
    gendoc::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Function"));
    if (!grammar3)
    {
        grammar3 = gendoc::cppparser::Function::Create(pd);
    }
    AddGrammarReference(grammar3);
    gendoc::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("gendoc.cppparser.TypeExpr"));
    if (!grammar4)
    {
        grammar4 = gendoc::cppparser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar4);
    gendoc::parsing::Grammar* grammar5 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Declarator"));
    if (!grammar5)
    {
        grammar5 = gendoc::cppparser::Declarator::Create(pd);
    }
    AddGrammarReference(grammar5);
    gendoc::parsing::Grammar* grammar6 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Identifier"));
    if (!grammar6)
    {
        grammar6 = gendoc::cppparser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar6);
    gendoc::parsing::Grammar* grammar7 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Expression"));
    if (!grammar7)
    {
        grammar7 = gendoc::cppparser::Expression::Create(pd);
    }
    AddGrammarReference(grammar7);
    gendoc::parsing::Grammar* grammar8 = pd->GetGrammar(ToUtf32("gendoc.cppparser.Literal"));
    if (!grammar8)
    {
        grammar8 = gendoc::cppparser::Literal::Create(pd);
    }
    AddGrammarReference(grammar8);
}

void Class::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("DeclSpecifiers"), this, ToUtf32("Declaration.DeclSpecifiers")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TemplateDeclaration"), this, ToUtf32("Template.TemplateDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("FunctionBody"), this, ToUtf32("Function.FunctionBody")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Declarator"), this, ToUtf32("Declarator.Declarator")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("BraceOrEqualInitializer"), this, ToUtf32("Declarator.BraceOrEqualInitializer")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("FunctionDefinition"), this, ToUtf32("Function.FunctionDefinition")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("TypedefDeclaration"), this, ToUtf32("Declaration.TypedefDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("UsingDeclaration"), this, ToUtf32("Declaration.UsingDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("SimpleTemplateId"), this, ToUtf32("Template.SimpleTemplateId")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("EnumDeclaration"), this, ToUtf32("Enumeration.EnumDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("AliasDeclaration"), this, ToUtf32("Declaration.AliasDeclaration")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("QualifiedIdNode"), this, ToUtf32("Identifier.QualifiedIdNode")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("PossiblyEmptyArgumentList"), this, ToUtf32("Expression.PossiblyEmptyArgumentList")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("BracedInitializerList"), this, ToUtf32("Declarator.BracedInitializerList")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("IntegerLiteral"), this, ToUtf32("Literal.IntegerLiteral")));
    AddRule(new ClassDeclarationRule(ToUtf32("ClassDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("ClassSpecifier"), ToUtf32("ClassSpecifier"), 1),
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new ForwardClassDeclarationRule(ToUtf32("ForwardClassDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("ClassKey"), ToUtf32("ClassKey"), 0),
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("ClassName"), ToUtf32("ClassName"), 1))),
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new ClassSpecifierRule(ToUtf32("ClassSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("ClassHead"), ToUtf32("ClassHead"), 1)),
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::CharParser('{'))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("MemberSpecifications"), ToUtf32("MemberSpecifications"), 2)),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                new gendoc::parsing::CharParser('}')))))))));
    AddRule(new MemberSpecificationsRule(ToUtf32("MemberSpecifications"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("MemberSpecification"), ToUtf32("MemberSpecification"), 2))));
    AddRule(new MemberSpecificationRule(ToUtf32("MemberSpecification"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::EmptyParser()),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("AccessSpecifier"), ToUtf32("AccessSpecifier"), 0),
                            new gendoc::parsing::CharParser(':')),
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("m1"), ToUtf32("MemberDeclaration"), 1))))),
                    new gendoc::parsing::PositiveParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("m2"), ToUtf32("MemberDeclaration"), 1)))))))));
    AddRule(new MemberDeclarationRule(ToUtf32("MemberDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                            new gendoc::parsing::SequenceParser(
                                                                new gendoc::parsing::SequenceParser(
                                                                    new gendoc::parsing::SequenceParser(
                                                                        new gendoc::parsing::SequenceParser(
                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("DeclSpecifiers"), ToUtf32("DeclSpecifiers"), 0),
                                                                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))),
                                                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 1))),
                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("VirtPureSpecifiers"), ToUtf32("VirtPureSpecifiers"), 1)),
                                                                new gendoc::parsing::OptionalParser(
                                                                    new gendoc::parsing::GroupingParser(
                                                                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("BraceOrEqualInitializer"), ToUtf32("BraceOrEqualInitializer"), 1))))),
                                                            new gendoc::parsing::CharParser(';')))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("SpecialMemberFunctionDeclaration"), ToUtf32("SpecialMemberFunctionDeclaration"), 1))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                                new gendoc::parsing::GroupingParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("FunctionDefinition"), ToUtf32("FunctionDefinition"), 1),
                                                        new gendoc::parsing::OptionalParser(
                                                            new gendoc::parsing::CharParser(';')))))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                            new gendoc::parsing::GroupingParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("SpecialMemberFunctionDefinition"), ToUtf32("SpecialMemberFunctionDefinition"), 1),
                                                    new gendoc::parsing::OptionalParser(
                                                        new gendoc::parsing::CharParser(';')))))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A7"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("UsingDeclaration"), ToUtf32("UsingDeclaration"), 1))),
                                new gendoc::parsing::ActionParser(ToUtf32("A8"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("TypedefDeclaration"), ToUtf32("TypedefDeclaration"), 1))),
                            new gendoc::parsing::ActionParser(ToUtf32("A9"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("TemplateDeclaration"), ToUtf32("TemplateDeclaration"), 1))),
                        new gendoc::parsing::ActionParser(ToUtf32("A10"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("ClassDeclaration"), ToUtf32("ClassDeclaration"), 1))),
                    new gendoc::parsing::ActionParser(ToUtf32("A11"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("EnumDeclaration"), ToUtf32("EnumDeclaration"), 1))),
                new gendoc::parsing::ActionParser(ToUtf32("A12"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("AliasDeclaration"), ToUtf32("AliasDeclaration"), 1))),
            new gendoc::parsing::ActionParser(ToUtf32("A13"),
                new gendoc::parsing::NonterminalParser(ToUtf32("ForwardClassDeclaration"), ToUtf32("ForwardClassDeclaration"), 1)))));
    AddRule(new SpecialMemberFunctionDeclarationRule(ToUtf32("SpecialMemberFunctionDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("DeclSpecifiers"), ToUtf32("DeclSpecifiers"), 0),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 1))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("VirtPureSpecifiers"), ToUtf32("VirtPureSpecifiers"), 1)),
                    new gendoc::parsing::CharParser(';'))))));
    AddRule(new ClassHeadRule(ToUtf32("ClassHead"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("ClassKey"), ToUtf32("ClassKey"), 0),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("ClassName"), ToUtf32("ClassName"), 1))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("ClassVirtSpecifiers"), ToUtf32("ClassVirtSpecifiers"), 0)),
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("BaseClause"), ToUtf32("BaseClause"), 1)))))));
    AddRule(new ClassKeyRule(ToUtf32("ClassKey"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::KeywordParser(ToUtf32("class"))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::KeywordParser(ToUtf32("struct")))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::KeywordParser(ToUtf32("union"))))));
    AddRule(new ClassNameRule(ToUtf32("ClassName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("QualifiedIdNode"), ToUtf32("QualifiedIdNode"), 1))));
    AddRule(new ClassVirtSpecifiersRule(ToUtf32("ClassVirtSpecifiers"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ClassVirtSpecifier"), ToUtf32("ClassVirtSpecifier"), 0))))));
    AddRule(new ClassVirtSpecifierRule(ToUtf32("ClassVirtSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::KeywordParser(ToUtf32("final"))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::KeywordParser(ToUtf32("explicit"))))));
    AddRule(new BaseClauseRule(ToUtf32("BaseClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser(':'),
                    new gendoc::parsing::NonterminalParser(ToUtf32("BaseClassSpecifierList"), ToUtf32("BaseClassSpecifierList"), 1))))));
    AddRule(new BaseClassSpecifierListRule(ToUtf32("BaseClassSpecifierList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("b1"), ToUtf32("BaseClassSpecifier"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser(','),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("b2"), ToUtf32("BaseClassSpecifier"), 1))))))))));
    AddRule(new BaseClassSpecifierRule(ToUtf32("BaseClassSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("BaseSpecifiers"), ToUtf32("BaseSpecifiers"), 0),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ClassName"), ToUtf32("ClassName"), 1))))));
    AddRule(new BaseSpecifiersRule(ToUtf32("BaseSpecifiers"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("BaseSpecifier"), ToUtf32("BaseSpecifier"), 0))))));
    AddRule(new BaseSpecifierRule(ToUtf32("BaseSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::KeywordParser(ToUtf32("virtual"))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("AccessSpecifier"), ToUtf32("AccessSpecifier"), 0)))));
    AddRule(new AccessSpecifierRule(ToUtf32("AccessSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::KeywordParser(ToUtf32("public"))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::KeywordParser(ToUtf32("protected")))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::KeywordParser(ToUtf32("private"))))));
    AddRule(new VirtPureSpecifiersRule(ToUtf32("VirtPureSpecifiers"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::KleeneStarParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("VirtSpecifier"), ToUtf32("VirtSpecifier"), 1)))),
            new gendoc::parsing::OptionalParser(
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("PureSpecifier"), ToUtf32("PureSpecifier"), 1)))))));
    AddRule(new VirtSpecifierRule(ToUtf32("VirtSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::KeywordParser(ToUtf32("override"))),
                new gendoc::parsing::ActionParser(ToUtf32("A1"),
                    new gendoc::parsing::KeywordParser(ToUtf32("final")))),
            new gendoc::parsing::ActionParser(ToUtf32("A2"),
                new gendoc::parsing::KeywordParser(ToUtf32("new"))))));
    AddRule(new PureSpecifierRule(ToUtf32("PureSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('='),
                    new gendoc::parsing::CharParser('0'))))));
    AddRule(new SpecialMemberFunctionDefinitionRule(ToUtf32("SpecialMemberFunctionDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("DeclSpecifiers"), ToUtf32("DeclSpecifiers"), 0),
                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 1))),
                        new gendoc::parsing::OptionalParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("CtorInitializer"), ToUtf32("CtorInitializer"), 1))))),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("FunctionBody"), ToUtf32("FunctionBody"), 1),
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::CharParser('='),
                                    new gendoc::parsing::GroupingParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                new gendoc::parsing::KeywordParser(ToUtf32("default"))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                new gendoc::parsing::KeywordParser(ToUtf32("delete")))))),
                                new gendoc::parsing::CharParser(';')))))))));
    AddRule(new CtorInitializerRule(ToUtf32("CtorInitializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser(':'),
                    new gendoc::parsing::NonterminalParser(ToUtf32("MemberInitializerList"), ToUtf32("MemberInitializerList"), 1))))));
    AddRule(new MemberInitializerListRule(ToUtf32("MemberInitializerList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("MemberInitializer"), 1)),
                    new gendoc::parsing::KleeneStarParser(
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::CharParser(','),
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("MemberInitializer"), 1))))))))));
    AddRule(new MemberInitializerRule(ToUtf32("MemberInitializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("id1"), ToUtf32("MemberInitializerId"), 1)),
                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::CharParser('('),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("args"), ToUtf32("PossiblyEmptyArgumentList"), 1)),
                                    new gendoc::parsing::CharParser(')'))))))),
            new gendoc::parsing::ActionParser(ToUtf32("A3"),
                new gendoc::parsing::GroupingParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::ActionParser(ToUtf32("A4"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("id2"), ToUtf32("MemberInitializerId"), 1)),
                        new gendoc::parsing::NonterminalParser(ToUtf32("BracedInitializerList"), ToUtf32("BracedInitializerList"), 1)))))));
    AddRule(new MemberInitializerIdRule(ToUtf32("MemberInitializerId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("QualifiedIdNode"), ToUtf32("QualifiedIdNode"), 1))));
}

} } // namespace gendoc.cppparser
