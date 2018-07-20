// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CONTENT_INCLUDED
#define GENDOC_CONTENT_INCLUDED
#include <gendoc/dom/Document.hpp>
#include <gendoc/cppsym/SymbolTable.hpp>
#include <gendoc/cppast/SourceFile.hpp>
#include <gendoc/syntax/ParserFileContent.hpp>
#include <gendoc/parsing/ParsingDomain.hpp>

namespace gendoc { namespace content {

std::unique_ptr<gendoc::dom::Document> GenerateContentXml(dom::Document* inputXml, dom::Document* solutionXml, const std::string& contentXmlFilePath, bool verbose,
    gendoc::cppsym::SymbolTable& symbolTable, std::vector<std::unique_ptr<gendoc::cppast::Node>>& asts, std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles,
    gendoc::parsing::ParsingDomain* parsingDomain, std::vector<std::unique_ptr<gendoc::syntax::ParserFileContent>>& parserFileContents, 
    std::unordered_map<int, std::u32string>& parserProjectMap);
std::unique_ptr<gendoc::dom::Document> ReadContentXml(const std::string& contentXmlFilePath);

} } // namespace gendoc::content

#endif // GENDOC_CONTENT_INCLUDED
