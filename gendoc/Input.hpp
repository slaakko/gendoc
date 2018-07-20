// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_INPUT_INCLUDED
#define GENDOC_INPUT_INCLUDED
#include <gendoc/dom/Document.hpp>

namespace gendoc { namespace input {

std::unique_ptr<dom::Document> ReadInputXml(const std::string& inputXmlFilePath);
std::string GetTargetDir(dom::Document* inputDoc);
std::string GetSolutionFilePath(dom::Document* inputDoc);
std::string GetSolutionXmlFilePath(dom::Document* inputDoc);
std::string GetContentXmlFilePath(dom::Document* inputDoc);
std::u32string GetSolutionDescription(dom::Document* inputDoc);

struct Tree
{
    Tree(const std::u32string& title_, const std::string& xmlFilePath_, const std::vector<std::u32string>& ns_, const std::string& outputFile_);
    std::u32string title;
    std::string xmlFilePath;
    std::vector<std::u32string> ns;
    std::string outputFile;
};

std::vector<Tree> GetTreeData(dom::Document* inputDoc);

} } // namespace gendoc::input

#endif // GENDOC_INPUT_INCLUDED
