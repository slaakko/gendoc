// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_HTML_INCLUDED
#define GENDOC_HTML_INCLUDED
#include <gendoc/dom/Document.hpp>
#include <gendoc/cppast/SourceFile.hpp>
#include <gendoc/cppsym/SymbolTable.hpp>
#include <gendoc/syntax/ParserFileContent.hpp>

namespace gendoc { namespace html {

void MakeDirectories(const std::string& targetDir, std::string& htmlDir, std::string& contentDir, std::string& styleDir,
    const std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles);
void GenerateStyleSheet(const std::string& styleDir, std::string& styleDirName, std::string& styleFileName);
void GenerateIndexHtml(dom::Document* inputXml, const std::string& htmlDir, const std::string& styleDirName, const std::string& styleFileName,
    std::vector<dom::Element*>& componentElements, std::unordered_map<dom::Element*, std::string>& componentElementFileNameMap);
void GenerateComponentHtml(dom::Document* inputXml, dom::Document* contentXml, const std::string& htmlDir, const std::string& styleDirName, const std::string& styleFileName,
    const std::vector<dom::Element*>& componentElements, const std::unordered_map<dom::Element*, std::string>& componentElementFileNameMap, 
    const std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles, const std::vector<std::unique_ptr<gendoc::syntax::ParserFileContent>>& parserFileContents,
    const std::unordered_map<int, std::u32string>& parserProjectMap, std::vector<std::unique_ptr<gendoc::cppast::SourceFileNode>>& parserSourceFileNodes);
void GenerateContent(dom::Document* contentXml, const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName, 
    std::unordered_map<std::u32string, std::unique_ptr<dom::Element>>& inlineCodeMap);
void GenerateHtmlCodeFiles(const std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles, const std::string& styleDirName, const std::string& styleFileName,
    gendoc::cppsym::SymbolTable& symbolTable, int inlineCodeLimit, std::unordered_map<std::u32string, std::unique_ptr<dom::Element>>& inlineCodeMap);
void GenerateHtmlParserFiles(const std::vector<gendoc::cppast::SourceFileNode*>& parserSourceFiles, const std::string& styleDirName, const std::string& styleFileName);

} } // namespace gendoc::html

#endif // GENDOC_HTML_INCLUDED
