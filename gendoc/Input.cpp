// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/gendoc/Input.hpp>
#include <gendoc/xpath/XPathEvaluate.hpp>
#include <gendoc/dom/Parser.hpp>
#include <gendoc/dom/Element.hpp>
#include <gendoc/util/Unicode.hpp>
#include <gendoc/util/Path.hpp>

namespace gendoc { namespace input {

using namespace gendoc::unicode;

std::unique_ptr<dom::Document> ReadInputXml(const std::string& inputXmlFilePath)
{
    return dom::ReadDocument(inputXmlFilePath);
}

std::string GetTargetDir(dom::Document* inputDoc)
{
    std::u32string targetDir = inputDoc->DocumentElement()->GetAttribute(U"targetDir");
    if (targetDir.empty())
    {
        throw std::runtime_error("solution.targetDir is empty");
    }
    return ToUtf8(targetDir);
}

std::string GetSolutionFilePath(dom::Document* inputDoc)
{
    std::u32string solutionFilePath = inputDoc->DocumentElement()->GetAttribute(U"solutionFilePath");
    if (solutionFilePath.empty())
    {
        throw std::runtime_error("solution.solutionFilePath is empty");
    }
    return ToUtf8(solutionFilePath);
}

std::string GetSolutionXmlFilePath(dom::Document* inputDoc)
{
    std::u32string targetDir = inputDoc->DocumentElement()->GetAttribute(U"targetDir");
    if (targetDir.empty())
    {
        throw std::runtime_error("solution.targetDir is empty");
    }
    std::u32string solutionName = inputDoc->DocumentElement()->GetAttribute(U"name");
    if (solutionName.empty())
    {
        throw std::runtime_error("solution.name is empty");
    }
    std::string solutionXmlFilePath = Path::Combine(ToUtf8(targetDir), ToUtf8(solutionName) + ".solution.xml");
    return solutionXmlFilePath;
}

std::string GetContentXmlFilePath(dom::Document* inputDoc)
{
    std::u32string targetDir = inputDoc->DocumentElement()->GetAttribute(U"targetDir");
    if (targetDir.empty())
    {
        throw std::runtime_error("solution.targetDir is empty");
    }
    std::u32string solutionName = inputDoc->DocumentElement()->GetAttribute(U"name");
    if (solutionName.empty())
    {
        throw std::runtime_error("solution.name is empty");
    }
    std::string contentXmlFilePath = Path::Combine(ToUtf8(targetDir), ToUtf8(solutionName) + ".content.xml");
    return contentXmlFilePath;
}

std::u32string GetSolutionDescription(dom::Document* inputDoc)
{
    std::u32string solutionDescription = inputDoc->DocumentElement()->GetAttribute(U"description");
    return solutionDescription;
}

Tree::Tree(const std::u32string& title_, const std::string& xmlFilePath_, const std::vector<std::u32string>& ns_, const std::string& outputFile_) :
    title(title_), xmlFilePath(xmlFilePath_), ns(ns_), outputFile(outputFile_)
{
}

std::vector<Tree> GetTreeData(dom::Document* inputDoc)
{
    std::vector<Tree> treeData;
    std::unique_ptr<xpath::XPathObject> trees = xpath::Evaluate(U"/solution/trees/tree", inputDoc);
    if (trees->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* treeNodeSet = static_cast<xpath::XPathNodeSet*>(trees.get());
        int n = treeNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* treeNode = (*treeNodeSet)[i];
            if (treeNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* treeElement = static_cast<dom::Element*>(treeNode);
                std::string xmlFilePath = ToUtf8(treeElement->GetAttribute(U"xmlFilePath"));
                std::string outputFile = ToUtf8(treeElement->GetAttribute(U"outputFile"));
                std::u32string title = treeElement->GetAttribute(U"title");
                std::vector<std::u32string> ns;
                std::unique_ptr<xpath::XPathObject> namespaces = xpath::Evaluate(U"namespaces/namespace", treeElement);
                if (namespaces->Type() == xpath::XPathObjectType::nodeSet)
                {
                    xpath::XPathNodeSet* namespaceNodeSet = static_cast<xpath::XPathNodeSet*>(namespaces.get());
                    int n = namespaceNodeSet->Length();
                    for (int i = 0; i < n; ++i)
                    {
                        dom::Node* namespaceNode = (*namespaceNodeSet)[i];
                        if (namespaceNode->GetNodeType() == dom::NodeType::elementNode)
                        {
                            dom::Element* nsElement = static_cast<dom::Element*>(namespaceNode);
                            ns.push_back(nsElement->GetAttribute(U"value"));
                        }
                    }
                }
                treeData.push_back(Tree(title, xmlFilePath, ns, outputFile));
            }
        }
    }
    return treeData;
}

} } // namespace gendoc::input
