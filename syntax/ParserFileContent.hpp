// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_SYNTAX_PARSERFILECONTENT_INCLUDED
#define GENDOC_SYNTAX_PARSERFILECONTENT_INCLUDED

#include <gendoc/parsing/ParsingDomain.hpp>
#include <gendoc/parsing/Namespace.hpp>

namespace gendoc { namespace syntax {

class ParserFileContent
{
public:
    ParserFileContent(int id_, gendoc::parsing::ParsingDomain* parsingDomain_);
    void SetFilePath(const std::string& filePath_) { filePath = filePath_; }
    void SetRelativeFilePath(const std::string& relativeFilePath_) { relativeFilePath = relativeFilePath_; }
    void AddIncludeDirective(const std::string& includeDirective_, const std::string& fileAttribute_);
    void AddUsingObject(gendoc::codedom::UsingObject* usingObjectSubject);
    void AddGrammar(gendoc::parsing::Grammar* grammar);
    void BeginNamespace(const std::u32string& ns);
    void EndNamespace();
    int Id() const { return id; }
    const std::string& FilePath() const { return filePath; }
    const std::string& RelativeFilePath() const { return relativeFilePath; }
    gendoc::parsing::ParsingDomain* ParsingDomain() const { return parsingDomain; }
    const std::vector<std::pair<std::string, std::string>>& IncludeDirectives() const { return includeDirectives; }
    const std::vector<std::unique_ptr<gendoc::parsing::UsingObject>>& UsingObjects() const { return usingObjects; }
    gendoc::parsing::Scope* CurrentScope() const { return parsingDomain->CurrentScope(); }
private:
    int id;
    std::string filePath;
    std::string relativeFilePath;
    gendoc::parsing::ParsingDomain* parsingDomain;
    std::vector<std::pair<std::string, std::string>> includeDirectives;
    std::vector<std::unique_ptr<gendoc::parsing::UsingObject>> usingObjects;
};

} } // namespace gendoc::syntax

#endif // GENDOC_SYNTAX_PARSERFILECONTENT_INCLUDED
