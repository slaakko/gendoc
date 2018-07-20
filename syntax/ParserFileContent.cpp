// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/syntax/ParserFileContent.hpp>
#include <gendoc/parsing/Grammar.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;

ParserFileContent::ParserFileContent(int id_, gendoc::parsing::ParsingDomain* parsingDomain_): id(id_), parsingDomain(parsingDomain_)
{
}

void ParserFileContent::AddIncludeDirective(const std::string& includeDirective_, const std::string& fileAttribute_)
{
    includeDirectives.push_back(std::make_pair(includeDirective_, fileAttribute_));
}

void ParserFileContent::AddUsingObject(gendoc::codedom::UsingObject* usingObjectSubject)
{
    gendoc::parsing::Scope* enclosingScope = parsingDomain->CurrentScope();
    UsingObject* usingObject(new UsingObject(usingObjectSubject, enclosingScope));
    usingObject->SetOwner(id);
    usingObjects.push_back(std::unique_ptr<UsingObject>(usingObject));
}

void ParserFileContent::AddGrammar(gendoc::parsing::Grammar* grammar)
{
    grammar->SetOwner(id);
    parsingDomain->AddGrammar(grammar);
}

void ParserFileContent::BeginNamespace(const std::u32string& ns)
{
    parsingDomain->BeginNamespace(ns);
}

void ParserFileContent::EndNamespace()
{
    parsingDomain->EndNamespace();
}

} } // namespace gendoc::syntax
