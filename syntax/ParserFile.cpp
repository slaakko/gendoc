#include "ParserFile.hpp"
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
#include <gendoc/parsing/Namespace.hpp>
#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/code/Declaration.hpp>
#include <gendoc/syntax/Element.hpp>
#include <gendoc/syntax/Grammar.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

ParserFile* ParserFile::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

ParserFile* ParserFile::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ParserFile* grammar(new ParserFile(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ParserFile::ParserFile(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("ParserFile"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.syntax")), parsingDomain_)
{
    SetOwner(0);
}

ParserFileContent* ParserFile::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, int id_, gendoc::parsing::ParsingDomain* parsingDomain_)
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
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<int>(id_)));
    stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<gendoc::parsing::ParsingDomain*>(parsingDomain_)));
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
    ParserFileContent* result = *static_cast<gendoc::parsing::ValueObject<ParserFileContent*>*>(value.get());
    stack.pop();
    return result;
}

class ParserFile::ParserFileRule : public gendoc::parsing::Rule
{
public:
    ParserFileRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("int"), ToUtf32("id_")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::parsing::ParsingDomain*"), ToUtf32("parsingDomain_")));
        SetValueTypeName(ToUtf32("ParserFileContent*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> parsingDomain__value = std::move(stack.top());
        context->parsingDomain_ = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::ParsingDomain*>*>(parsingDomain__value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> id__value = std::move(stack.top());
        context->id_ = *static_cast<gendoc::parsing::ValueObject<int>*>(id__value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParserFileContent*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ParserFileRule>(this, &ParserFileRule::A0Action));
        gendoc::parsing::NonterminalParser* includeDirectivesNonterminalParser = GetNonterminal(ToUtf32("IncludeDirectives"));
        includeDirectivesNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ParserFileRule>(this, &ParserFileRule::PreIncludeDirectives));
        gendoc::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ParserFileRule>(this, &ParserFileRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ParserFileContent(context->id_, context->parsingDomain_);
        context->value->SetFilePath(fileName);
    }
    void PreIncludeDirectives(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParserFileContent*>(context->value)));
    }
    void PreNamespaceContent(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParserFileContent*>(context->value)));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingDomain*>(context->parsingDomain_)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): id_(), parsingDomain_(), value() {}
        int id_;
        gendoc::parsing::ParsingDomain* parsingDomain_;
        ParserFileContent* value;
    };
};

class ParserFile::IncludeDirectivesRule : public gendoc::parsing::Rule
{
public:
    IncludeDirectivesRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<gendoc::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* includeDirectiveNonterminalParser = GetNonterminal(ToUtf32("IncludeDirective"));
        includeDirectiveNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<IncludeDirectivesRule>(this, &IncludeDirectivesRule::PreIncludeDirective));
    }
    void PreIncludeDirective(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParserFileContent*>(context->parserFileContent)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): parserFileContent() {}
        ParserFileContent* parserFileContent;
    };
};

class ParserFile::IncludeDirectiveRule : public gendoc::parsing::Rule
{
public:
    IncludeDirectiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<gendoc::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IncludeDirectiveRule>(this, &IncludeDirectiveRule::A0Action));
        gendoc::parsing::NonterminalParser* fileAttributeNonterminalParser = GetNonterminal(ToUtf32("FileAttribute"));
        fileAttributeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IncludeDirectiveRule>(this, &IncludeDirectiveRule::PostFileAttribute));
        gendoc::parsing::NonterminalParser* includeFileNameNonterminalParser = GetNonterminal(ToUtf32("IncludeFileName"));
        includeFileNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<IncludeDirectiveRule>(this, &IncludeDirectiveRule::PostIncludeFileName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->AddIncludeDirective(ToUtf8(context->fromIncludeFileName), ToUtf8(context->fromFileAttribute));
    }
    void PostFileAttribute(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFileAttribute_value = std::move(stack.top());
            context->fromFileAttribute = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromFileAttribute_value.get());
            stack.pop();
        }
    }
    void PostIncludeFileName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIncludeFileName_value = std::move(stack.top());
            context->fromIncludeFileName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromIncludeFileName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): parserFileContent(), fromFileAttribute(), fromIncludeFileName() {}
        ParserFileContent* parserFileContent;
        std::u32string fromFileAttribute;
        std::u32string fromIncludeFileName;
    };
};

class ParserFile::FileAttributeRule : public gendoc::parsing::Rule
{
public:
    FileAttributeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FileAttributeRule>(this, &FileAttributeRule::A0Action));
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

class ParserFile::IncludeFileNameRule : public gendoc::parsing::Rule
{
public:
    IncludeFileNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IncludeFileNameRule>(this, &IncludeFileNameRule::A0Action));
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

class ParserFile::NamespaceContentRule : public gendoc::parsing::Rule
{
public:
    NamespaceContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingDomain*"), ToUtf32("parsingDomain")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> parsingDomain_value = std::move(stack.top());
        context->parsingDomain = *static_cast<gendoc::parsing::ValueObject<ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<gendoc::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A3Action));
        gendoc::parsing::NonterminalParser* fileAttributeNonterminalParser = GetNonterminal(ToUtf32("FileAttribute"));
        fileAttributeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostFileAttribute));
        gendoc::parsing::NonterminalParser* usingDeclarationNonterminalParser = GetNonterminal(ToUtf32("UsingDeclaration"));
        usingDeclarationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostUsingDeclaration));
        gendoc::parsing::NonterminalParser* usingDirectiveNonterminalParser = GetNonterminal(ToUtf32("UsingDirective"));
        usingDirectiveNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostUsingDirective));
        gendoc::parsing::NonterminalParser* namespaceAliasDefinitionNonterminalParser = GetNonterminal(ToUtf32("NamespaceAliasDefinition"));
        namespaceAliasDefinitionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostNamespaceAliasDefinition));
        gendoc::parsing::NonterminalParser* grammarNonterminalParser = GetNonterminal(ToUtf32("Grammar"));
        grammarNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreGrammar));
        grammarNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostGrammar));
        gendoc::parsing::NonterminalParser* namespaceNonterminalParser = GetNonterminal(ToUtf32("Namespace"));
        namespaceNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreNamespace));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromUsingDeclaration->SetFileAttr(context->fromFileAttribute);
        context->parserFileContent->AddUsingObject(context->fromUsingDeclaration);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromUsingDirective->SetFileAttr(context->fromFileAttribute);
        context->parserFileContent->AddUsingObject(context->fromUsingDirective);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromNamespaceAliasDefinition->SetFileAttr(context->fromFileAttribute);
        context->parserFileContent->AddUsingObject(context->fromNamespaceAliasDefinition);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->AddGrammar(context->fromGrammar);
    }
    void PostFileAttribute(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFileAttribute_value = std::move(stack.top());
            context->fromFileAttribute = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromFileAttribute_value.get());
            stack.pop();
        }
    }
    void PostUsingDeclaration(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUsingDeclaration_value = std::move(stack.top());
            context->fromUsingDeclaration = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::UsingObject*>*>(fromUsingDeclaration_value.get());
            stack.pop();
        }
    }
    void PostUsingDirective(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUsingDirective_value = std::move(stack.top());
            context->fromUsingDirective = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::UsingObject*>*>(fromUsingDirective_value.get());
            stack.pop();
        }
    }
    void PostNamespaceAliasDefinition(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNamespaceAliasDefinition_value = std::move(stack.top());
            context->fromNamespaceAliasDefinition = *static_cast<gendoc::parsing::ValueObject<gendoc::codedom::UsingObject*>*>(fromNamespaceAliasDefinition_value.get());
            stack.pop();
        }
    }
    void PreGrammar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Scope*>(context->parserFileContent->CurrentScope())));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingDomain*>(context->parsingDomain)));
    }
    void PostGrammar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromGrammar_value = std::move(stack.top());
            context->fromGrammar = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Grammar*>*>(fromGrammar_value.get());
            stack.pop();
        }
    }
    void PreNamespace(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParserFileContent*>(context->parserFileContent)));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingDomain*>(context->parsingDomain)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): parserFileContent(), parsingDomain(), fromFileAttribute(), fromUsingDeclaration(), fromUsingDirective(), fromNamespaceAliasDefinition(), fromGrammar() {}
        ParserFileContent* parserFileContent;
        ParsingDomain* parsingDomain;
        std::u32string fromFileAttribute;
        gendoc::codedom::UsingObject* fromUsingDeclaration;
        gendoc::codedom::UsingObject* fromUsingDirective;
        gendoc::codedom::UsingObject* fromNamespaceAliasDefinition;
        gendoc::parsing::Grammar* fromGrammar;
    };
};

class ParserFile::NamespaceRule : public gendoc::parsing::Rule
{
public:
    NamespaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingDomain*"), ToUtf32("parsingDomain")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> parsingDomain_value = std::move(stack.top());
        context->parsingDomain = *static_cast<gendoc::parsing::ValueObject<ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
        std::unique_ptr<gendoc::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<gendoc::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A1Action));
        gendoc::parsing::NonterminalParser* nsNonterminalParser = GetNonterminal(ToUtf32("ns"));
        nsNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<NamespaceRule>(this, &NamespaceRule::Postns));
        gendoc::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<NamespaceRule>(this, &NamespaceRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->BeginNamespace(context->fromns);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->EndNamespace();
    }
    void Postns(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromns_value = std::move(stack.top());
            context->fromns = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromns_value.get());
            stack.pop();
        }
    }
    void PreNamespaceContent(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParserFileContent*>(context->parserFileContent)));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<ParsingDomain*>(context->parsingDomain)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): parserFileContent(), parsingDomain(), fromns() {}
        ParserFileContent* parserFileContent;
        ParsingDomain* parsingDomain;
        std::u32string fromns;
    };
};

void ParserFile::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.code.Declaration"));
    if (!grammar0)
    {
        grammar0 = gendoc::code::Declaration::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar1)
    {
        grammar1 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
    gendoc::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("gendoc.syntax.GrammarGrammar"));
    if (!grammar2)
    {
        grammar2 = gendoc::syntax::GrammarGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void ParserFile::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("UsingDirective"), this, ToUtf32("gendoc.code.Declaration.UsingDirective")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("gendoc.parsing.stdlib.spaces_and_comments")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("NamespaceAliasDefinition"), this, ToUtf32("gendoc.code.Declaration.NamespaceAliasDefinition")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("gendoc.parsing.stdlib.qualified_id")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("newline"), this, ToUtf32("gendoc.parsing.stdlib.newline")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("Grammar"), this, ToUtf32("GrammarGrammar.Grammar")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("UsingDeclaration"), this, ToUtf32("gendoc.code.Declaration.UsingDeclaration")));
    AddRule(new ParserFileRule(ToUtf32("ParserFile"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::EmptyParser()),
                new gendoc::parsing::OptionalParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("IncludeDirectives"), ToUtf32("IncludeDirectives"), 1))),
            new gendoc::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 2))));
    AddRule(new IncludeDirectivesRule(ToUtf32("IncludeDirectives"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("IncludeDirective"), ToUtf32("IncludeDirective"), 1))));
    AddRule(new IncludeDirectiveRule(ToUtf32("IncludeDirective"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::OptionalParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("FileAttribute"), ToUtf32("FileAttribute"), 0)),
                    new gendoc::parsing::CharParser('#')),
                new gendoc::parsing::KeywordParser(ToUtf32("include"))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::TokenParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("IncludeFileName"), ToUtf32("IncludeFileName"), 0),
                            new gendoc::parsing::KleeneStarParser(
                                new gendoc::parsing::CharSetParser(ToUtf32("\r\n"), true))),
                        new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0)))))));
    AddRule(new FileAttributeRule(ToUtf32("FileAttribute"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('['),
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::StringParser(ToUtf32("cpp")),
                            new gendoc::parsing::StringParser(ToUtf32("hpp")))))),
            new gendoc::parsing::CharParser(']'))));
    AddRule(new IncludeFileNameRule(ToUtf32("IncludeFileName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('<'),
                        new gendoc::parsing::PositiveParser(
                            new gendoc::parsing::CharSetParser(ToUtf32(">\r\n"), true))),
                    new gendoc::parsing::CharParser('>'))))));
    AddRule(new NamespaceContentRule(ToUtf32("NamespaceContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("FileAttribute"), ToUtf32("FileAttribute"), 0)),
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("UsingDeclaration"), ToUtf32("UsingDeclaration"), 0)),
                                        new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("UsingDirective"), ToUtf32("UsingDirective"), 0))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("NamespaceAliasDefinition"), ToUtf32("NamespaceAliasDefinition"), 0))))),
                        new gendoc::parsing::ActionParser(ToUtf32("A3"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("Grammar"), ToUtf32("Grammar"), 2))),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Namespace"), ToUtf32("Namespace"), 2))))));
    AddRule(new NamespaceRule(ToUtf32("Namespace"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::KeywordParser(ToUtf32("namespace")),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("ns"), ToUtf32("qualified_id"), 0)))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser('{'))),
                new gendoc::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 2)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser('}'))))));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace gendoc.syntax
