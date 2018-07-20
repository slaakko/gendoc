// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/gendoc/Html.hpp>
#include <gendoc/gendoc/HtmlSourceCodeGenerator.hpp>
#include <gendoc/gendoc/HtmlSourceCodeWriter.hpp>
#include <gendoc/gendoc/Input.hpp>
#include <gendoc/gendoc/ClassInheritanceDiagramCode.hpp>
#include <gendoc/dom/Document.hpp>
#include <gendoc/dom/Element.hpp>
#include <gendoc/dom/CharacterData.hpp>
#include <gendoc/xpath/XPathEvaluate.hpp>
#include <gendoc/cppparser/ParserTokenGrammar.hpp>
#include <gendoc/cppsym/Symbol.hpp>
#include <gendoc/parsing/Utility.hpp>
#include <gendoc/util/TextUtils.hpp>
#include <gendoc/util/Path.hpp>
#include <gendoc/util/Unicode.hpp>
#include <gendoc/util/Util.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <fstream>

namespace gendoc { namespace html {

using namespace gendoc::util;
using namespace gendoc::unicode;
using namespace gendoc::cppparser;
using namespace gendoc::cppsym;

void MakeDirectories(const std::string& targetDir, std::string& htmlDir, std::string& contentDir, std::string& styleDir,
    const std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles)
{
    htmlDir = GetFullPath(Path::Combine(targetDir, "html"));
    boost::filesystem::create_directories(htmlDir);
    contentDir = GetFullPath(Path::Combine(htmlDir, "content"));
    boost::filesystem::create_directories(contentDir);
    styleDir = GetFullPath(Path::Combine(htmlDir, "style"));
    boost::filesystem::create_directories(styleDir);
    if (!sourceFiles.empty())
    {
        std::unordered_set<std::string> directories;
        for (gendoc::cppast::SourceFileNode* sourceFile : sourceFiles)
        {
            std::string htmlFilePath = Path::Combine(contentDir, ToUtf8(sourceFile->Id()));
            sourceFile->SetHtmlSourceFilePath(htmlFilePath);
            std::string directory = Path::GetDirectoryName(htmlFilePath);
            if (directories.find(directory) == directories.cend())
            {
                directories.insert(directory);
                boost::filesystem::create_directories(directory);
            }
        }
    }
}

void GenerateStyleSheet(const std::string& styleDir, std::string& styleDirName, std::string& styleFileName)
{
    styleDirName = "style";
    styleFileName = "style.css";
    std::string styleFilePath = GetFullPath(Path::Combine(styleDir, styleFileName));
    std::ofstream styleSheetFile(styleFilePath);
    CodeFormatter formatter(styleSheetFile);
    formatter.WriteLine(".diagram");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("text-align: center;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".tree");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("margin: 20px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("h1, h2, h3, h4, h5, h6");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #005ab4;");
    formatter.WriteLine("font-family: sans-serif;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("table");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: sans-serif;");
    formatter.WriteLine("border-collapse: collapse;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("table, th, td");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("text-align: left;");
    formatter.WriteLine("border: 1px solid #dddddd;");
    formatter.WriteLine("margin: 20px;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("table.functionTable");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: sans-serif;");
    formatter.WriteLine("border-collapse: collapse;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("table.functionTable, td.functionTableSecondCol");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("text-align: left;");
    formatter.WriteLine("border: 1px solid #dddddd;");
    formatter.WriteLine("margin: 20px;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("td.functionTableFirstCol");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("text-align: right;");
    formatter.WriteLine("border: 1px solid #dddddd;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    //  grammars:
    formatter.WriteLine("table.grammar");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("background-color: FloralWhite;");
    formatter.WriteLine("margin: 20px;");
    formatter.WriteLine("padding: 20px;");
    formatter.WriteLine("max-width: 800px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("table.grammar, td.grammarThirdCol");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("vertical-align: top;");
    formatter.WriteLine("text-align: left;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("td.grammarFirstCol");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("vertical-align: top;");
    formatter.WriteLine("text-align: right;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("td.grammarSecondCol");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("vertical-align: top;");
    formatter.WriteLine("text-align: left;");
    formatter.WriteLine("font-family: serif;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".rule");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: serif;");
    formatter.WriteLine("font-style: italic;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleChar");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: Consolas;");
    formatter.WriteLine("color: #a31515;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleString");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: Consolas;");
    formatter.WriteLine("color: #a31515;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleCharSet");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: Consolas;");
    formatter.WriteLine("color: #a31515;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleKeyword");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #0000cc;");
    formatter.WriteLine("font-weight: bold;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleParams");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: black;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleOp");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("padding-left: 2pt;");
    formatter.WriteLine("padding-right: 2pt;");
    formatter.WriteLine("font-family: serif;"); 
    formatter.WriteLine("color: black;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleLink");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: serif;");
    formatter.WriteLine("font-style: italic;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    //  code:
    formatter.WriteLine(".code");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: Consolas;");
    formatter.WriteLine("font-size: small;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".lineNumber");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #2b91af;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".type");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #2b91af;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".pp");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #808080;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".kw");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #0000cc;");
    formatter.WriteLine("font-weight: bold;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".string");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #a31515;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".comment");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: green;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".identifier");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: black;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".other");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: black;");
    formatter.DecIndent();
    formatter.WriteLine("}");
}

std::u32string GetPrefix(dom::Element* element)
{
    std::u32string prefix;
    dom::ParentNode* parent = element->Parent();
    while (parent && parent->GetNodeType() == dom::NodeType::elementNode)
    {
        dom::Element* parentElement = static_cast<dom::Element*>(parent);
        if (parentElement->Name() == U"namespace" || parentElement->Name() == U"class")
        {
            std::u32string nsOrClassName = parentElement->GetAttribute(U"name");
            if (!nsOrClassName.empty())
            {
                if (!prefix.empty())
                {
                    prefix = nsOrClassName + U"::" + prefix;
                }
                else
                {
                    prefix = nsOrClassName;
                }
            }
        }
        parent = parent->Parent();
    }
    return prefix;
}

std::string MakeComponentFileName(const std::u32string& componentName)
{
    std::string componentFileName = "component_" + ToUtf8(componentName) + ".html";
    return componentFileName;
}

void GenerateIndexHtml(dom::Document* inputXml, const std::string& htmlDir, const std::string& styleDirName, const std::string& styleFileName,
    std::vector<dom::Element*>& componentElements, std::unordered_map<dom::Element*, std::string>& componentElementFileNameMap)
{
    std::string indexFilePath = GetFullPath(Path::Combine(htmlDir, "index.html"));
    dom::Document indexDocument;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    std::u32string title = gendoc::input::GetSolutionDescription(inputXml) + U" Reference";
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(Path::Combine(styleDirName, styleFileName));
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    std::unique_ptr<dom::Element> componentsElement(new dom::Element(U"h2"));
    componentsElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Components")));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(componentsElement.release()));
    std::unique_ptr<dom::Element> componentTableElement(new dom::Element(U"table"));
    std::unique_ptr<dom::Element> headerRowElement(new dom::Element(U"tr"));
    std::unique_ptr<dom::Element> nameElement(new dom::Element(U"th"));
    nameElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Component")));
    headerRowElement->AppendChild(std::unique_ptr<dom::Node>(nameElement.release()));
    std::unique_ptr<dom::Element> descriptionElement(new dom::Element(U"th"));
    descriptionElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Description")));
    headerRowElement->AppendChild(std::unique_ptr<dom::Node>(descriptionElement.release()));
    componentTableElement->AppendChild(std::unique_ptr<dom::Node>(headerRowElement.release()));
    std::unique_ptr<xpath::XPathObject> components = xpath::Evaluate(U"/solution/components/component", inputXml);
    if (components->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* componentsNodeSet = static_cast<xpath::XPathNodeSet*>(components.get());
        int n = componentsNodeSet->Length();
        for (int i = n - 1; i >= 0; --i)
        {
            dom::Node* componentNode = (*componentsNodeSet)[i];
            if (componentNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* componentElement = static_cast<dom::Element*>(componentNode);
                componentElements.push_back(componentElement);
                std::u32string componentName = componentElement->GetAttribute(U"name");
                std::u32string componentDescription = componentElement->GetAttribute(U"description");
                std::unique_ptr<dom::Element> componentRowElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> nameElement(new dom::Element(U"td"));
                std::unique_ptr<dom::Element> componentLinkElement(new dom::Element(U"a"));
                std::string componentFileName = MakeComponentFileName(componentName);
                componentElementFileNameMap[componentElement] = componentFileName;
                std::u32string componentDocUrl = ToUtf32(Path::Combine("content", componentFileName));
                componentLinkElement->SetAttribute(U"href", componentDocUrl);
                componentLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(componentName)));
                nameElement->AppendChild(std::unique_ptr<dom::Node>(componentLinkElement.release()));
                componentRowElement->AppendChild(std::unique_ptr<dom::Node>(nameElement.release()));
                std::unique_ptr<dom::Element> descriptionElement(new dom::Element(U"td"));
                descriptionElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(componentDescription)));
                componentRowElement->AppendChild(std::unique_ptr<dom::Node>(descriptionElement.release()));
                componentTableElement->AppendChild(std::unique_ptr<dom::Node>(componentRowElement.release()));
            }
        }
    }
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(componentTableElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    indexDocument.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    std::ofstream indexFile(indexFilePath);
    CodeFormatter formatter(indexFile);
    formatter.SetIndentSize(1);
    indexDocument.Write(formatter);
}

std::u32string MakeTitle(const std::u32string& name)
{
    if (!name.empty())
    {
        return std::u32string(1, ToUpper(name[0])) + name.substr(1);
    }
    else
    {
        return name;
    }
}

enum class Navigation
{
    header, footer
};

void GenerateNavigation(dom::Element* pageElement, Navigation navigation,
    const std::u32string& indexLink, const std::u32string& parentLink, const std::u32string& prevLink, const std::u32string& nextLink)
{
    if (!indexLink.empty())
    {
        std::unique_ptr<dom::Element> index(new dom::Element(U"a"));
        index->SetAttribute(U"href", indexLink);
        index->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"index")));
        pageElement->AppendChild(std::unique_ptr<dom::Node>(index.release()));
    }
    else
    {
        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"index")));
    }
    pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" | ")));
    if (!parentLink.empty())
    {
        std::unique_ptr<dom::Element> parent(new dom::Element(U"a"));
        parent->SetAttribute(U"href", parentLink);
        parent->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"up")));
        pageElement->AppendChild(std::unique_ptr<dom::Node>(parent.release()));
    }
    else
    {
        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"up")));
    }
    pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" | ")));
    if (!prevLink.empty())
    {
        std::unique_ptr<dom::Element> prev(new dom::Element(U"a"));
        prev->SetAttribute(U"href", prevLink);
        prev->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"prev")));
        pageElement->AppendChild(std::unique_ptr<dom::Node>(prev.release()));
    }
    else
    {
        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"prev")));
    }
    pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" | ")));
    if (!nextLink.empty())
    {
        std::unique_ptr<dom::Element> next(new dom::Element(U"a"));
        next->SetAttribute(U"href", nextLink);
        next->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"next")));
        pageElement->AppendChild(std::unique_ptr<dom::Node>(next.release()));
    }
    else
    {
        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"next")));
    }
    if (navigation == Navigation::header)
    {
        std::unique_ptr<dom::Element> hr(new dom::Element(U"hr"));
        pageElement->AppendChild(std::unique_ptr<dom::Node>(hr.release()));
    }
}

bool GenerateNamespaceNames(int level, dom::Element* namespaceTableElement, dom::Element* namespaceParentElement, const std::u32string& componentName, bool global);

bool GenerateNamespaceNames(int level, dom::Element* namespaceTableElement, dom::Element* namespaceParentElement, const std::u32string& componentName)
{
    return GenerateNamespaceNames(level, namespaceTableElement, namespaceParentElement, componentName, false);
}

bool GenerateNamespaceNames(int level, dom::Element* namespaceTableElement, dom::Element* namespaceParentElement, const std::u32string& componentName, bool global)
{
    bool rowAdded = false;
    std::unique_ptr<xpath::XPathObject> namespaces = xpath::Evaluate(U"namespaces/namespace", namespaceParentElement);
    if (namespaces->Type() == xpath::XPathObjectType::nodeSet || global)
    {
        xpath::XPathNodeSet* namespacesNodeSet = static_cast<xpath::XPathNodeSet*>(namespaces.get());
        int n = namespacesNodeSet->Length();
        if (global)
        {
            n = 1;
        }
        for (int i = 0; i < n; ++i)
        {
            dom::Node* nsNode = nullptr;
            if (global)
            {
                nsNode = namespaceParentElement;
            }
            else
            {
                nsNode = (*namespacesNodeSet)[i];
            }
            if (nsNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* nsElement = static_cast<dom::Element*>(nsNode);
                bool addRow = false;
                if (!componentName.empty())
                {
                    std::unique_ptr<xpath::XPathObject> project = xpath::Evaluate(U"projects/project", nsElement);
                    if (project->Type() == xpath::XPathObjectType::nodeSet)
                    {
                        xpath::XPathNodeSet* projectNodeSet = static_cast<xpath::XPathNodeSet*>(project.get());
                        int m = projectNodeSet->Length();
                        for (int k = 0; k < m; ++k)
                        {
                            dom::Node* projectNode = (*projectNodeSet)[k];
                            if (projectNode->GetNodeType() == dom::NodeType::elementNode)
                            {
                                dom::Element* projectElement = static_cast<dom::Element*>(projectNode);
                                std::u32string projectName = projectElement->GetAttribute(U"name");
                                if (projectName == componentName)
                                {
                                    addRow = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    addRow = true;
                }
                if (addRow)
                {
                    rowAdded = true;
                    std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                    for (int l = 0; l < level; ++l)
                    {
                        std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                        trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                    }
                    std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                    std::u32string nsName = nsElement->GetAttribute(U"name");
                    if (nsName.empty())
                    {
                        nsName = U"Global Namespace";
                    }
                    std::u32string nsId = nsElement->GetAttribute(U"id");
                    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                    linkElement->SetAttribute(U"href", nsId + U".html");
                    linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(nsName)));
                    tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                    trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                    namespaceTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                    bool subRowAdded = GenerateNamespaceNames(level + 1, namespaceTableElement, nsElement, componentName);
                    if (subRowAdded)
                    {
                        rowAdded = true;
                    }
                }
            }
        }
    }
    return rowAdded;
}

void GenerateNamespaceSection(dom::Element* parentNsElement, dom::Element* pageElement, dom::Document* contentXml, const std::u32string& componentName, bool global);

void GenerateNamespaceSection(dom::Element* parentNsElement, dom::Element* pageElement, dom::Document* contentXml, const std::u32string& componentName)
{
    GenerateNamespaceSection(parentNsElement, pageElement, contentXml, componentName, false);
}

void GenerateNamespaceSection(dom::Element* parentNsElement, dom::Element* pageElement, dom::Document* contentXml, const std::u32string& componentName, bool global)
{
    std::unique_ptr<dom::Element> namespaceTableElement(new dom::Element(U"table"));
    if (GenerateNamespaceNames(0, namespaceTableElement.get(), parentNsElement, componentName, global))
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Namespaces")));
        pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        pageElement->AppendChild(std::unique_ptr<dom::Node>(namespaceTableElement.release()));
    }
}

dom::Element* GetGlobalNsElement(dom::Document* contentXml)
{
    std::unique_ptr<xpath::XPathObject> globalNs = xpath::Evaluate(U"/solution/symbolTable/namespace", contentXml);
    if (globalNs->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* globalNsNodeSet = static_cast<xpath::XPathNodeSet*>(globalNs.get());
        if (globalNsNodeSet->Length() == 1)
        {
            dom::Node* globalNsNode = (*globalNsNodeSet)[0];
            if (globalNsNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* globalNsElement = static_cast<dom::Element*>(globalNsNode);
                return globalNsElement;
            }
        }
    }
    return nullptr;
}

void GenerateComponenNamespaceSection(dom::Element* pageElement, dom::Document* contentXml, const std::u32string& componentName)
{
    dom::Element* globalNsElement = GetGlobalNsElement(contentXml);
    if (globalNsElement)
    {
        GenerateNamespaceSection(globalNsElement, pageElement, contentXml, componentName, true);
        GenerateNamespaceSection(globalNsElement, pageElement, contentXml, componentName, false);
    }
}

struct ByRelativeFilePath
{
    bool operator()(SourceFileNode* left, SourceFileNode* right) const
    {
        return left->RelativeSourceFilePath() < right->RelativeSourceFilePath();
    }
};

void GenerateComponentFileSection(const std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles, 
    const std::vector<std::unique_ptr<gendoc::syntax::ParserFileContent>>& parserFileContents, const std::unordered_map<int, std::u32string>& parserProjectMap,
    std::vector<std::unique_ptr<SourceFileNode>>& parserSourceFileNodes,
    dom::Element* pageElement, const std::u32string& componentName)
{
    std::vector<SourceFileNode*> componentSourceFiles;
    int n = sourceFiles.size();
    for (int i = 0; i < n; ++i)
    {
        SourceFileNode* sourceFileNode = sourceFiles[i];
        if (sourceFileNode->ProjectName() == componentName)
        {
            componentSourceFiles.push_back(sourceFileNode);
        }
    }
    for (const std::unique_ptr<gendoc::syntax::ParserFileContent>& parserFileContent : parserFileContents)
    {
        int projectId = parserFileContent->Id();
        auto it = parserProjectMap.find(projectId);
        if (it != parserProjectMap.cend())
        {
            const std::u32string& projectName = it->second;
            if (projectName == componentName)
            {
                std::unique_ptr<SourceFileNode> sourceFileNode(new SourceFileNode(Span(), parserFileContent->FilePath(), parserFileContent->RelativeFilePath(), projectName));
                componentSourceFiles.push_back(sourceFileNode.get());
                parserSourceFileNodes.push_back(std::move(sourceFileNode));
            }
        }
    }
    int m = componentSourceFiles.size();
    if (m > 0)
    {
        std::unique_ptr<dom::Element> fileTableElement(new dom::Element(U"table"));
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Files")));
        pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::sort(componentSourceFiles.begin(), componentSourceFiles.end(), ByRelativeFilePath());
        for (int i = 0; i < m; ++i)
        {
            SourceFileNode* sourceFile = componentSourceFiles[i];
            std::u32string sourceFileName = ToUtf32(sourceFile->RelativeSourceFilePath());
            std::u32string sourceFileId = sourceFile->Id();
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", sourceFileId);
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(sourceFileName)));
            tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            fileTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        pageElement->AppendChild(std::unique_ptr<dom::Node>(fileTableElement.release()));
    }
}

void GenerateConstructorSection(dom::Element* pageElement, dom::Element* parentElement, const std::string& contentDir,
    const std::string& styleDirName, const std::string& styleFileName, dom::Document* contentXml, std::vector<dom::Element*>& constructorElements);

void GenerateFunctionSection(const std::u32string& title, dom::Element* pageElement, dom::Element* parentElement, const std::string& contentDir,
    const std::string& styleDirName, const std::string& styleFileName, dom::Document* contentXml, std::vector<dom::Element*>& functionElements);

void GenerateClassSection(dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml,
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName, 
    std::unordered_map<std::u32string, std::unique_ptr<gendoc::dom::Element>>& inlineCodeMap);

void GenerateEnumSection(dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml,
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName);

void GenerateVariableSection(const std::u32string& title, dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml,
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName);

void GenerateTypedefSection(dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml,
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName);

void GenerateGrammarSection(dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml,
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName);

void GenerateRuleSection(dom::Element* pageElement, dom::Element* parentElement, const std::string& contentDir,
    const std::string& styleDirName, const std::string& styleFileName, dom::Document* contentXml);

void GenerateFunctionDetailSection(const std::u32string& title, const std::u32string& functionTitle, dom::Element* pageElement, dom::Element* parentElement, 
    std::vector<dom::Element*>& functionElements, dom::Document* contentXml, std::unordered_map<std::u32string, std::unique_ptr<gendoc::dom::Element>>& inlineCodeMap);

std::u32string MakeClassName(dom::Element* classElement)
{
    std::u32string className = classElement->GetAttribute(U"name");
    std::unique_ptr<xpath::XPathObject> templateParameters = xpath::Evaluate(U"templateParameters/templateParameter", classElement);
    if (templateParameters->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* templateParameterNodeSet = static_cast<xpath::XPathNodeSet*>(templateParameters.get());
        int n = templateParameterNodeSet->Length();
        if (n > 0)
        {
            className.append(1, '<');
            for (int i = 0; i < n; ++i)
            {
                if (i > 0)
                {
                    className.append(U", ");
                }
                dom::Node* templateParameterNode = (*templateParameterNodeSet)[i];
                if (templateParameterNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* templateParameterElement = static_cast<dom::Element*>(templateParameterNode);
                    className.append(templateParameterElement->GetAttribute(U"name"));
                }
            }
            className.append(1, '>');
        }
    }
    return className;
}

std::u32string MakeFullClassName(dom::Element* classElement)
{
    std::u32string className = MakeClassName(classElement);
    std::u32string prefix = GetPrefix(classElement);
    if (!prefix.empty())
    {
        return prefix + U"::" + className;
    }
    else
    {
        return className;
    }
}

struct Class
{
    Class(const std::u32string& name_, int index_, int level_, bool subject_, bool hasDerivedClasses_, const std::u32string& link_);
    void Write(CodeFormatter& formatter);
    std::u32string name;
    int index;
    int level;
    bool subject;
    bool hasDerivedClasses;
    std::u32string link;
};

Class::Class(const std::u32string& name_, int index_, int level_, bool subject_, bool hasDerivedClasses_, const std::u32string& link_) :
    name(name_), index(index_), level(level_), subject(subject_), hasDerivedClasses(hasDerivedClasses_), link(link_)
{
}

void Class::Write(CodeFormatter& formatter)
{
    formatter.Write("{ ");
    formatter.Write("name: \"");
    formatter.Write(gendoc::parsing::XmlEscape(ToUtf8(name)));
    formatter.Write("\", ");
    formatter.Write("id: \"");
    formatter.Write("class_" + std::to_string(index));
    formatter.Write("\", ");
    formatter.Write("level: ");
    formatter.Write(std::to_string(level));
    formatter.Write(", ");
    formatter.Write("subject: ");
    formatter.Write(subject ? "true" : "false");
    formatter.Write(", ");
    formatter.Write("hasDerivedClasses: ");
    formatter.Write(hasDerivedClasses ? "true" : "false");
    formatter.Write(", ");
    formatter.Write("link: \"");
    formatter.Write(ToUtf8(link));
    formatter.Write("\"");
    formatter.Write(" }");
}

void GenerateClassData(std::vector<Class>& classes, dom::Element* classElement, dom::Document* contentXml, int& depth, int& level)
{
    int numBaseClasses = 0;
    std::unique_ptr<xpath::XPathObject> baseClasses = xpath::Evaluate(U"baseClasses/baseClass", classElement);
    if (baseClasses->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* baseClassNodeSet = static_cast<xpath::XPathNodeSet*>(baseClasses.get());
        int n = baseClassNodeSet->Length();
        if (n > 0)
        {
            ++depth;
            --level;
            for (int i = 0; i < n; ++i)
            {
                dom::Node* baseClassNode = (*baseClassNodeSet)[i];
                if (baseClassNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* baseClassIdElement = static_cast<dom::Element*>(baseClassNode);
                    std::u32string baseClassId = baseClassIdElement->GetAttribute(U"baseClassId");
                    dom::Element* baseClassElement = contentXml->GetElementById(baseClassId);
                    if (baseClassElement)
                    {
                        if (baseClassElement->Name() == U"specialization")
                        {
                            std::u32string primaryTypeId = baseClassElement->GetAttribute(U"primaryTypeId");
                            baseClassElement = contentXml->GetElementById(primaryTypeId);
                        }
                        if (baseClassElement)
                        {
                            if (baseClassElement->Name() == U"class")
                            {
                                GenerateClassData(classes, baseClassElement, contentXml, depth, level);
                                ++numBaseClasses;
                            }
                        }
                    }
                }
            }
            ++level;
        }
    }
    std::unique_ptr<xpath::XPathObject> derivedClasses = xpath::Evaluate(U"derivedClasses/derivedClass", classElement);
    if (derivedClasses->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* derivedClassNodeSet = static_cast<xpath::XPathNodeSet*>(derivedClasses.get());
        int numDerivedClasses = derivedClassNodeSet->Length();
        if (level == 0)
        {
            if (numBaseClasses > 0 || numDerivedClasses > 0)
            {
                std::u32string link = classElement->GetAttribute(U"id") + U".html";
                classes.push_back(Class(MakeFullClassName(classElement), classes.size(), level, true, numDerivedClasses > 0, link));
                if (numDerivedClasses > 0)
                {
                    ++level;
                    for (int i = 0; i < numDerivedClasses; ++i)
                    {
                        dom::Node* derivedClassNode = (*derivedClassNodeSet)[i];
                        if (derivedClassNode->GetNodeType() == dom::NodeType::elementNode)
                        {
                            dom::Element* derivedClassIdElement = static_cast<dom::Element*>(derivedClassNode);
                            std::u32string derivedClassId = derivedClassIdElement->GetAttribute(U"derivedClassId");
                            dom::Element* derivedClassElement = contentXml->GetElementById(derivedClassId);
                            if (derivedClassElement)
                            {
                                if (derivedClassElement->Name() == U"specialization")
                                {
                                    std::u32string primaryTypeId = derivedClassElement->GetAttribute(U"primaryTypeId");
                                    derivedClassElement = contentXml->GetElementById(primaryTypeId);
                                }
                                if (derivedClassElement)
                                {
                                    if (derivedClassElement->Name() == U"class")
                                    {
                                        std::u32string link = derivedClassElement->GetAttribute(U"id") + U".html";
                                        bool hasDerivedClasses = false;
                                        std::unique_ptr<xpath::XPathObject> descendantClasses = xpath::Evaluate(U"derivedClasses/derivedClass", derivedClassElement);
                                        if (descendantClasses->Type() == xpath::XPathObjectType::nodeSet)
                                        {
                                            xpath::XPathNodeSet* descendantClassNodeSet = static_cast<xpath::XPathNodeSet*>(descendantClasses.get());
                                            int numDescendantClasses = descendantClassNodeSet->Length();
                                            if (numDescendantClasses > 0)
                                            {
                                                hasDerivedClasses = true;
                                            }
                                        }
                                        classes.push_back(Class(MakeFullClassName(derivedClassElement), classes.size(), level, false, hasDerivedClasses, link));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            std::u32string link = classElement->GetAttribute(U"id") + U".html";
            classes.push_back(Class(MakeFullClassName(classElement), classes.size(), level, false, numDerivedClasses > 0, link));
        }
    }
}

struct ByLevelAndIndex
{
    bool operator()(const Class& left, const Class& right) const
    {
        if (left.level < right.level) return true;
        if (left.level > right.level) return false;
        return left.index < right.index;
    }
};

bool GenerateClassInheritanceDiagramFile(const std::string& contentDir, dom::Element* classElement, dom::Document* contentXml, std::string& inheritanceDiagramFileName)
{
    int depth = 0;
    int level = 0;
    std::vector<Class> classes;
    GenerateClassData(classes, classElement, contentXml, depth, level);
    if (!classes.empty())
    {
        for (Class& cls : classes)
        {
            cls.level += depth;
        }
        std::sort(classes.begin(), classes.end(), ByLevelAndIndex());
        inheritanceDiagramFileName = ToUtf8(classElement->GetAttribute(U"id")) + "_inheritance.js";
        std::string inheritanceDiagramFilePath = Path::Combine(contentDir, inheritanceDiagramFileName);
        std::ofstream inheritanceDiagramFile(inheritanceDiagramFilePath);
        CodeFormatter formatter(inheritanceDiagramFile);
        GenerateClassInheritanceDiagramCode(formatter);
        formatter.WriteLine("function drawClassInheritanceDiagram() {");
        formatter.IncIndent();
        formatter.WriteLine("var classes = [");
        formatter.IncIndent();
        bool first = true;
        for (Class& cls : classes)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                formatter.WriteLine(",");
            }
            cls.Write(formatter);
        }
        formatter.WriteLine("];");
        formatter.DecIndent();
        formatter.WriteLine("drawDiagram(classes);");
        formatter.DecIndent();
        formatter.WriteLine("}");
        formatter.WriteLine();
        return true;
    }
    else
    {
        return false;
    }
}

void GenerateClassContent(dom::Element* classElement, dom::Document* contentXml, const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName,
    const std::u32string& indexLink, const std::u32string& parentLink, const std::u32string& prevLink, std::u32string& nextLink,
    std::unordered_map<std::u32string, std::unique_ptr<gendoc::dom::Element>>& inlineCodeMap)
{
    std::vector<dom::Element*> constructorElements;
    std::vector<dom::Element*> functionElements;
    std::u32string className = MakeClassName(classElement);
    std::u32string classId = classElement->GetAttribute(U"id");
    std::u32string classKey = classElement->GetAttribute(U"key");
    std::string classFileName = ToUtf8(classId) + ".html";
    std::string classFilePath = Path::Combine(contentDir, classFileName);
    std::ofstream classFile(classFilePath);
    CodeFormatter formatter(classFile);
    dom::Document classDocument;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    std::u32string abstract_;
    if (classElement->GetAttribute(U"abstract") == U"true")
    {
        abstract_ = U"Abstract ";
    }
    std::u32string title = className + U" " + abstract_ + MakeTitle(classKey);
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(Path::Combine(Path::Combine("..", styleDirName), styleFileName));
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    std::string inheritanceDiagramFileName;
    bool classInheritanceDiagramGenerated = GenerateClassInheritanceDiagramFile(contentDir, classElement, contentXml, inheritanceDiagramFileName);
    if (classInheritanceDiagramGenerated)
    {
        std::unique_ptr<dom::Element> scriptElement(new dom::Element(U"script"));
        scriptElement->SetAttribute(U"type", U"text/javascript");
        scriptElement->SetAttribute(U"src", ToUtf32(inheritanceDiagramFileName));
        scriptElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
        headElement->AppendChild(std::unique_ptr<dom::Node>(scriptElement.release()));
    }
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    GenerateNavigation(bodyElement.get(), Navigation::header, indexLink, parentLink, prevLink, nextLink);
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    if (classInheritanceDiagramGenerated)
    {
        bodyElement->SetAttribute(U"onload", U"drawClassInheritanceDiagram()");
        std::unique_ptr<dom::Element> divElement(new dom::Element(U"div"));
        divElement->SetAttribute(U"class", U"diagram");
        std::unique_ptr<dom::Element> svgElement(new dom::Element(U"svg"));
        svgElement->SetAttribute(U"width", U"0");
        svgElement->SetAttribute(U"height", U"0");
        svgElement->SetAttribute(U"id", U"classInheritanceDiagram");
        svgElement->SetAttribute(U"xmlns", U"http://www.w3.org/2000/svg");
        svgElement->SetAttribute(U"version", U"2.0");
        divElement->AppendChild(std::unique_ptr<dom::Node>(svgElement.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(divElement.release()));
    }
    std::u32string definitionFileId = classElement->GetAttribute(U"definitionFileId");
    if (!definitionFileId.empty())
    {
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Definition at line ")));
        std::unique_ptr<dom::Element> lineLink(new dom::Element(U"a"));
        std::u32string line = classElement->GetAttribute(U"definitionLine");
        lineLink->SetAttribute(U"href", definitionFileId + U"#" + line);
        lineLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(line)));
        span->AppendChild(std::unique_ptr<dom::Node>(lineLink.release()));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" of ")));
        std::unique_ptr<dom::Element> fileLink(new dom::Element(U"a"));
        fileLink->SetAttribute(U"href", definitionFileId);
        fileLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(classElement->GetAttribute(U"definitionFile"))));
        span->AppendChild(std::unique_ptr<dom::Node>(fileLink.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
    }
    GenerateClassSection(bodyElement.get(), classElement, contentXml, contentDir, styleDirName, styleFileName, inlineCodeMap);
    GenerateConstructorSection(bodyElement.get(), classElement, contentDir, styleDirName, styleFileName, contentXml, constructorElements);
    GenerateFunctionSection(U"Member Functions", bodyElement.get(), classElement, contentDir, styleDirName, styleFileName, contentXml, functionElements);
    GenerateEnumSection(bodyElement.get(), classElement, contentXml, contentDir, styleDirName, styleFileName);
    GenerateTypedefSection(bodyElement.get(), classElement, contentXml, contentDir, styleDirName, styleFileName);
    GenerateVariableSection(U"Member Variables", bodyElement.get(), classElement, contentXml, contentDir, styleDirName, styleFileName);
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"hr")));
    GenerateFunctionDetailSection(U"Constructor Details", U"Constructor", bodyElement.get(), classElement, constructorElements, contentXml, inlineCodeMap);
    GenerateFunctionDetailSection(U"Member Function Details", U"Member Function", bodyElement.get(), classElement, functionElements, contentXml, inlineCodeMap);
    GenerateNavigation(bodyElement.get(), Navigation::footer, indexLink, parentLink, prevLink, nextLink);
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    classDocument.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    classDocument.Write(formatter);
}

void GenerateGrammarContent(dom::Element* grammarElement, dom::Document* contentXml, const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName,
    const std::u32string& indexLink, const std::u32string& parentLink, const std::u32string& prevLink, std::u32string& nextLink)
{
    std::u32string grammarName = grammarElement->GetAttribute(U"name");
    std::u32string grammarId = grammarElement->GetAttribute(U"id");
    std::string grammarFileName = ToUtf8(grammarId) + ".html";
    std::string grammarFilePath = Path::Combine(contentDir, grammarFileName);
    std::ofstream grammarFile(grammarFilePath);
    CodeFormatter formatter(grammarFile);
    dom::Document grammarDocument;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    std::u32string title = grammarName + U" Grammar";
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(Path::Combine(Path::Combine("..", styleDirName), styleFileName));
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    GenerateNavigation(bodyElement.get(), Navigation::header, indexLink, parentLink, prevLink, nextLink);
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    std::u32string definitionFileId = grammarElement->GetAttribute(U"definitionFileId");
    if (!definitionFileId.empty())
    {
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Definition at line ")));
        std::unique_ptr<dom::Element> lineLink(new dom::Element(U"a"));
        std::u32string line = grammarElement->GetAttribute(U"definitionLine");
        lineLink->SetAttribute(U"href", definitionFileId + U"#" + line);
        lineLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(line)));
        span->AppendChild(std::unique_ptr<dom::Node>(lineLink.release()));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" of ")));
        std::unique_ptr<dom::Element> fileLink(new dom::Element(U"a"));
        fileLink->SetAttribute(U"href", definitionFileId);
        fileLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(grammarElement->GetAttribute(U"definitionFile"))));
        span->AppendChild(std::unique_ptr<dom::Node>(fileLink.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
    }
    GenerateRuleSection(bodyElement.get(), grammarElement, contentDir, styleDirName, styleFileName, contentXml);
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"hr")));
    GenerateNavigation(bodyElement.get(), Navigation::footer, indexLink, parentLink, prevLink, nextLink);
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    grammarDocument.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    grammarDocument.Write(formatter);
}

void GenerateClassSection(dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml, 
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName,
    std::unordered_map<std::u32string, std::unique_ptr<gendoc::dom::Element>>& inlineCodeMap)
{
    std::unique_ptr<dom::Element> classTableElement(new dom::Element(U"table"));
    std::unique_ptr<xpath::XPathObject> classes = xpath::Evaluate(U"classes/class", parentElement);
    if (classes->Type() == xpath::XPathObjectType::nodeSet)
    {
        std::u32string indexLink = U"../index.html";
        std::u32string parentLink = parentElement->GetAttribute(U"id") + U".html";
        std::u32string prevLink;
        std::u32string nextLink;
        xpath::XPathNodeSet* classNodeSet = static_cast<xpath::XPathNodeSet*>(classes.get());
        int n = classNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Classes")));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        }
        for (int i = 0; i < n; ++i)
        {
            nextLink.clear();
            if (i < n - 1)
            {
                dom::Node* nextClassNode = (*classNodeSet)[i + 1];
                if (nextClassNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* nextClassElement = static_cast<dom::Element*>(nextClassNode);
                    nextLink = nextClassElement->GetAttribute(U"id") + U".html";
                }
            }
            dom::Node* classNode = (*classNodeSet)[i];
            if (classNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* classElement = static_cast<dom::Element*>(classNode);
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                std::u32string classKey = classElement->GetAttribute(U"key");
                std::u32string className = classElement->GetAttribute(U"name");
                tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(classKey + U" ")));
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                std::u32string classLink = classElement->GetAttribute(U"id") + U".html";
                linkElement->SetAttribute(U"href", classLink);
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(className)));
                tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                classTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                GenerateClassContent(classElement, contentXml, contentDir, styleDirName, styleFileName, indexLink, parentLink, prevLink, nextLink, inlineCodeMap);
                prevLink = classLink;
            }
        }
        if (n > 0)
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(classTableElement.release()));
        }
    }
}

void GenerateEnumContent(dom::Element* enumTypeElement, dom::Document* contentXml, const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName,
    const std::u32string& indexLink, const std::u32string& parentLink, const std::u32string& prevLink, std::u32string& nextLink)
{
    std::u32string enumTypeName = enumTypeElement->GetAttribute(U"name");
    std::u32string enumTypeId = enumTypeElement->GetAttribute(U"id");
    std::u32string enumKey = enumTypeElement->GetAttribute(U"key");
    std::string enumFileName = ToUtf8(enumTypeId) + ".html";
    std::string enumFilePath = Path::Combine(contentDir, enumFileName);
    std::ofstream enumFile(enumFilePath);
    CodeFormatter formatter(enumFile);
    dom::Document enumDocument;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    std::u32string title = enumTypeName + U" Enumeration";
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(Path::Combine(Path::Combine("..", styleDirName), styleFileName));
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    GenerateNavigation(bodyElement.get(), Navigation::header, indexLink, parentLink, prevLink, nextLink);
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    std::u32string definitionFileId = enumTypeElement->GetAttribute(U"definitionFileId");
    if (!definitionFileId.empty())
    {
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Definition at line ")));
        std::unique_ptr<dom::Element> lineLink(new dom::Element(U"a"));
        std::u32string line = enumTypeElement->GetAttribute(U"definitionLine");
        lineLink->SetAttribute(U"href", definitionFileId + U"#" + line);
        lineLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(line)));
        span->AppendChild(std::unique_ptr<dom::Node>(lineLink.release()));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" of ")));
        std::unique_ptr<dom::Element> fileLink(new dom::Element(U"a"));
        fileLink->SetAttribute(U"href", definitionFileId);
        fileLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(enumTypeElement->GetAttribute(U"definitionFile"))));
        span->AppendChild(std::unique_ptr<dom::Node>(fileLink.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
    }
    std::unique_ptr<xpath::XPathObject> enumerators = xpath::Evaluate(U"enumerators/enumerator", enumTypeElement);
    if (enumerators->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* enumeratorNodeSet = static_cast<xpath::XPathNodeSet*>(enumerators.get());
        int n = enumeratorNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Enumerators")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> enumeratorTableElement(new dom::Element(U"table"));
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> nameThElement(new dom::Element(U"th"));
            nameThElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Name")));
            trElement->AppendChild(std::unique_ptr<dom::Node>(nameThElement.release()));
            std::unique_ptr<dom::Element> valueThElement(new dom::Element(U"th"));
            valueThElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Value")));
            trElement->AppendChild(std::unique_ptr<dom::Node>(valueThElement.release()));
            enumeratorTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            for (int i = 0; i < n; ++i)
            {
                dom::Node* node = (*enumeratorNodeSet)[i];
                if (node->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* enumeratorElement = static_cast<dom::Element*>(node);
                    std::u32string enumeratorName = enumeratorElement->GetAttribute(U"name");
                    std::u32string enumeratorValue = enumeratorElement->GetAttribute(U"value");
                    std::u32string enumeratorId = enumeratorElement->GetAttribute(U"id");
                    std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                    trElement->SetAttribute(U"id", enumeratorId);
                    std::unique_ptr<dom::Element> nameTdElement(new dom::Element(U"td"));
                    nameTdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(enumeratorName)));
                    trElement->AppendChild(std::unique_ptr<dom::Node>(nameTdElement.release()));
                    std::unique_ptr<dom::Element> valueTdElement(new dom::Element(U"td"));
                    valueTdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(enumeratorValue)));
                    trElement->AppendChild(std::unique_ptr<dom::Node>(valueTdElement.release()));
                    enumeratorTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                }
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(enumeratorTableElement.release()));
        }
    }
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"hr")));
    GenerateNavigation(bodyElement.get(), Navigation::footer, indexLink, parentLink, prevLink, nextLink);
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    enumDocument.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    enumDocument.Write(formatter);
}

void GenerateEnumSection(dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml,
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName)
{
    std::unique_ptr<dom::Element> enumTableElement(new dom::Element(U"table"));
    std::unique_ptr<xpath::XPathObject> enumTypes = xpath::Evaluate(U"enumTypes/enumType", parentElement);
    if (enumTypes->Type() == xpath::XPathObjectType::nodeSet)
    {
        std::u32string indexLink = U"../index.html";
        std::u32string parentLink = parentElement->GetAttribute(U"id") + U".html";
        std::u32string prevLink;
        std::u32string nextLink;
        xpath::XPathNodeSet* enumTypeNodeSet = static_cast<xpath::XPathNodeSet*>(enumTypes.get());
        int n = enumTypeNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Enumerations")));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        }
        for (int i = 0; i < n; ++i)
        {
            nextLink.clear();
            if (i < n - 1)
            {
                dom::Node* nextEnumTypeNode = (*enumTypeNodeSet)[i + 1];
                if (nextEnumTypeNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* nextEnumTypeElement = static_cast<dom::Element*>(nextEnumTypeNode);
                    nextLink = nextEnumTypeElement->GetAttribute(U"id") + U".html";
                }
            }
            dom::Node* enumTypeNode = (*enumTypeNodeSet)[i];
            if (enumTypeNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* enumTypeElement = static_cast<dom::Element*>(enumTypeNode);
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                std::u32string enumKey = enumTypeElement->GetAttribute(U"key");
                std::u32string enumTypeName = enumTypeElement->GetAttribute(U"name");
                tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(enumKey + U" ")));
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                std::u32string enumTypeLink = enumTypeElement->GetAttribute(U"id") + U".html";
                linkElement->SetAttribute(U"href", enumTypeLink);
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(enumTypeName)));
                tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                enumTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                GenerateEnumContent(enumTypeElement, contentXml, contentDir, styleDirName, styleFileName, indexLink, parentLink, prevLink, nextLink);
                prevLink = enumTypeLink;
            }
        }
        if (n > 0)
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(enumTableElement.release()));
        }
    }
}

void GenerateGrammarSection(dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml,
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName)
{
    std::unique_ptr<dom::Element> grammarTableElement(new dom::Element(U"table"));
    std::unique_ptr<xpath::XPathObject> grammars = xpath::Evaluate(U"grammars/grammar", parentElement);
    if (grammars->Type() == xpath::XPathObjectType::nodeSet)
    {
        std::u32string indexLink = U"../index.html";
        std::u32string parentLink = parentElement->GetAttribute(U"id") + U".html";
        std::u32string prevLink;
        std::u32string nextLink;
        xpath::XPathNodeSet* grammarNodeSet = static_cast<xpath::XPathNodeSet*>(grammars.get());
        int n = grammarNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Grammars")));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        }
        for (int i = 0; i < n; ++i)
        {
            nextLink.clear();
            if (i < n - 1)
            {
                dom::Node* nextGrammarNode = (*grammarNodeSet)[i + 1];
                if (nextGrammarNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* nextGrammarElement = static_cast<dom::Element*>(nextGrammarNode);
                    nextLink = nextGrammarElement->GetAttribute(U"id") + U".html";
                }
            }
            dom::Node* grammarNode = (*grammarNodeSet)[i];
            if (grammarNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* grammarElement = static_cast<dom::Element*>(grammarNode);
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                std::u32string grammarName = grammarElement->GetAttribute(U"name");
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                std::u32string grammarLink = grammarElement->GetAttribute(U"id") + U".html";
                linkElement->SetAttribute(U"href", grammarLink);
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(grammarName)));
                tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                grammarTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                GenerateGrammarContent(grammarElement, contentXml, contentDir, styleDirName, styleFileName, indexLink, parentLink, prevLink, nextLink);
                prevLink = grammarLink;
            }
        }
        if (n > 0)
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(grammarTableElement.release()));
        }
    }
}

void GenerateRuleSection(dom::Element* pageElement, dom::Element* parentElement, const std::string& contentDir, 
    const std::string& styleDirName, const std::string& styleFileName, dom::Document* contentXml)
{
    std::unique_ptr<dom::Element> ruleTableElement(new dom::Element(U"table"));
    ruleTableElement->SetAttribute(U"class", U"grammar");
    std::unique_ptr<xpath::XPathObject> rules = xpath::Evaluate(U"rules/rule", parentElement);
    if (rules->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* ruleNodeSet = static_cast<xpath::XPathNodeSet*>(rules.get());
        int n = ruleNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Rules")));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        }
        for (int i = 0; i < n; ++i)
        {
            dom::Node* ruleNode = (*ruleNodeSet)[i];
            if (ruleNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* ruleElement = static_cast<dom::Element*>(ruleNode);
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::u32string ruleId = ruleElement->GetAttribute(U"id");
                trElement->SetAttribute(U"id", ruleId);
                std::unique_ptr<dom::Element> ruleNameTdElement(new dom::Element(U"td"));
                ruleNameTdElement->SetAttribute(U"class", U"grammarFirstCol");
                std::u32string ruleName = ruleElement->GetAttribute(U"name");
                std::unique_ptr<dom::Element> ruleSpan(new dom::Element(U"span"));
                ruleSpan->SetAttribute(U"class", U"rule");
                ruleSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(ruleName)));
                ruleNameTdElement->AppendChild(std::unique_ptr<dom::Node>(ruleSpan.release()));
                trElement->AppendChild(std::unique_ptr<dom::Node>(ruleNameTdElement.release()));
                std::unique_ptr<dom::Element> arrowTdElement(new dom::Element(U"td"));
                arrowTdElement->SetAttribute(U"class", U"grammarSecondCol");
                arrowTdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"rarr")));
                trElement->AppendChild(std::unique_ptr<dom::Node>(arrowTdElement.release()));
                std::unique_ptr<dom::Element> ruleBodyTdElement(new dom::Element(U"td"));
                ruleBodyTdElement->SetAttribute(U"class", U"grammarThirdCol");
                ruleBodyTdElement->SetAttribute(U"xml:space", U"preserve");
                ruleBodyTdElement->AppendChild(std::unique_ptr<dom::Node>(ruleElement->FirstChild()->CloneNode(true)));
                trElement->AppendChild(std::unique_ptr<dom::Node>(ruleBodyTdElement.release()));
                ruleTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
        }
        if (n > 0)
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(ruleTableElement.release()));
        }
    }
}

void AppendTypeName(dom::Element* pageElement, dom::Element* parentElement, const std::u32string& typeId, dom::Document* contentXml)
{
    dom::Element* typeElement = contentXml->GetElementById(typeId);
    if (typeElement)
    {
        std::u32string typeElementName = typeElement->Name();
        if (typeElementName == U"simpleType")
        {
            std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
            span->SetAttribute(U"class", U"kw");
            span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typeElement->GetAttribute(U"name"))));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        }
        else if (typeElementName == U"externalType")
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typeElement->GetAttribute(U"name"))));
        }
        else if (typeElementName == U"integerLiteralType")
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typeElement->GetAttribute(U"name"))));
        }
        else if (typeElementName == U"derivedType")
        {
            std::unique_ptr<xpath::XPathObject> derivations = xpath::Evaluate(U"derivations/derivation", typeElement);
            if (derivations->Type() == xpath::XPathObjectType::nodeSet)
            {
                xpath::XPathNodeSet* derivationNodeSet = static_cast<xpath::XPathNodeSet*>(derivations.get());
                int n = derivationNodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* derivationNode = (*derivationNodeSet)[i];
                    if (derivationNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* derivationElement = static_cast<dom::Element*>(derivationNode);
                        std::u32string derivationKind = derivationElement->GetAttribute(U"kind");
                        if (derivationKind == U"base")
                        {
                            std::u32string typeId = derivationElement->GetAttribute(U"typeId");
                            AppendTypeName(pageElement, parentElement, typeId, contentXml);
                        }
                        else
                        {
                            std::u32string derivationStr = derivationElement->GetAttribute(U"str");
                            if (derivationKind == U"specifier")
                            {
                                std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                                span->SetAttribute(U"class", U"kw");
                                span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(derivationStr)));
                                pageElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
                            }
                            else
                            {
                                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(derivationStr)));
                            }
                        }
                    }
                }
            }
        }
        else if (typeElementName == U"specialization")
        {
            std::u32string primaryTypeId = typeElement->GetAttribute(U"primaryTypeId");
            AppendTypeName(pageElement, parentElement, primaryTypeId, contentXml);
            pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"<")));
            std::unique_ptr<xpath::XPathObject> templateArguments = xpath::Evaluate(U"templateArguments/templateArgument", typeElement);
            if (templateArguments->Type() == xpath::XPathObjectType::nodeSet)
            {
                xpath::XPathNodeSet* templateArgumentNodeSet = static_cast<xpath::XPathNodeSet*>(templateArguments.get());
                int n = templateArgumentNodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    if (i > 0)
                    {
                        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
                    }
                    dom::Node* templateArgumentNode = (*templateArgumentNodeSet)[i];
                    if (templateArgumentNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* templateArgumentElement = static_cast<dom::Element*>(templateArgumentNode);
                        std::u32string templateArgumentTypeId = templateArgumentElement->GetAttribute(U"typeId");
                        AppendTypeName(pageElement, parentElement, templateArgumentTypeId, contentXml);
                    }
                }
            }
            pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U">")));
        }
        else if (typeElementName == U"typedef")
        {
            std::u32string parentId;
            dom::ParentNode* parent = typeElement->Parent();
            while (parent)
            {
                if (parent->Name() == U"class" || parent->Name() == U"namespace")
                {
                    if (parent->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* parentElement = static_cast<dom::Element*>(parent);
                        parentId = parentElement->GetAttribute(U"id");
                        break;
                    }
                }
                parent = parent->Parent();
            }
            std::u32string link = parentId + U".html#" + typeId;
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", link);
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typeElement->GetAttribute(U"name"))));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
        }
        else
        {
            std::u32string link = typeId + U".html";
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", link);
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typeElement->GetAttribute(U"name"))));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
        }
    }
    else
    {
        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"@TYPE_NOT_FOUND")));
    }
}

void AppendParameter(dom::Element* pageElement, dom::Element* parentElement, dom::Element* parameterElement, dom::Document* contentXml)
{
    std::u32string typeId = parameterElement->GetAttribute(U"type");
    AppendTypeName(pageElement, parentElement, typeId, contentXml);
    std::u32string name = parameterElement->GetAttribute(U"name");
    if (!name.empty())
    {
        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" " + name)));
    }
}

std::u32string MakeConstructorName(dom::Element* constructorElement, bool fullName);

std::u32string MakeConstructorName(dom::Element* constructorElement)
{
    return MakeConstructorName(constructorElement, false);
}

std::u32string MakeConstructorName(dom::Element* constructorElement, bool fullName)
{
    std::u32string contructorName = constructorElement->GetAttribute(U"name");
    if (fullName)
    {
        std::u32string prefix = GetPrefix(constructorElement);
        if (!prefix.empty())
        {
            return prefix + U"::" + contructorName;
        }
        else
        {
            return contructorName;
        }
    }
    else
    {
        return contructorName;
    }
}

std::u32string MakeFunctionName(dom::Element* functionElement, bool fullName);

std::u32string MakeFunctionName(dom::Element* functionElement)
{
    return MakeFunctionName(functionElement, false);
}

std::u32string MakeFunctionName(dom::Element* functionElement, bool fullName)
{
    std::u32string functionName = functionElement->GetAttribute(U"name");
    if (fullName)
    {
        std::u32string prefix = GetPrefix(functionElement);
        if (!prefix.empty())
        {
            functionName = prefix + U"::" + functionName;
        }
    }
    std::unique_ptr<xpath::XPathObject> templateParameters = xpath::Evaluate(U"templateParameters/templateParameter", functionElement);
    if (templateParameters->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* templateParameterNodeSet = static_cast<xpath::XPathNodeSet*>(templateParameters.get());
        int n = templateParameterNodeSet->Length();
        if (n > 0)
        {
            functionName.append(1, '<');
            for (int i = 0; i < n; ++i)
            {
                if (i > 0)
                {
                    functionName.append(U", ");
                }
                dom::Node* templateParameterNode = (*templateParameterNodeSet)[i];
                if (templateParameterNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* templateParameterElement = static_cast<dom::Element*>(templateParameterNode);
                    functionName.append(templateParameterElement->GetAttribute(U"name"));
                }
            }
            functionName.append(1, '>');
        }
    }
    return functionName;
}

void AppendParameterList(dom::Element* pageElement, dom::Element* parentElement, dom::Element* functionElement, dom::Document* contentXml)
{
    std::unique_ptr<xpath::XPathObject> parameters = xpath::Evaluate(U"parameters/parameter", functionElement);
    if (parameters->Type() == xpath::XPathObjectType::nodeSet)
    {
        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"(")));
        xpath::XPathNodeSet* parameterNodeSet = static_cast<xpath::XPathNodeSet*>(parameters.get());
        int n = parameterNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
            }
            dom::Node* parameterNode = (*parameterNodeSet)[i];
            if (parameterNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* parameterElement = static_cast<dom::Element*>(parameterNode);
                AppendParameter(pageElement, parentElement, parameterElement, contentXml);
            }
        }
        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U")")));
    }
}

void GenerateVariableSection(const std::u32string& title, dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml,
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName)
{
    std::unique_ptr<dom::Element> variableTableElement(new dom::Element(U"table"));
    std::unique_ptr<xpath::XPathObject> variables = xpath::Evaluate(U"variables/variable", parentElement);
    if (variables->Type() == xpath::XPathObjectType::nodeSet)
    {
        std::u32string indexLink = U"../index.html";
        std::u32string parentLink = parentElement->GetAttribute(U"id") + U".html";
        std::u32string prevLink;
        std::u32string nextLink;
        xpath::XPathNodeSet* variableNodeSet = static_cast<xpath::XPathNodeSet*>(variables.get());
        int n = variableNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        }
        for (int i = 0; i < n; ++i)
        {
            dom::Node* variableNode = (*variableNodeSet)[i];
            if (variableNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* variableElement = static_cast<dom::Element*>(variableNode);
                std::u32string variableTypeId = variableElement->GetAttribute(U"type");
                std::u32string variableName = variableElement->GetAttribute(U"name");
                std::u32string specifiers = variableElement->GetAttribute(U"specifiers");
                std::u32string variableId = variableElement->GetAttribute(U"id");
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                trElement->SetAttribute(U"id", variableId);
                std::unique_ptr<dom::Element> typeTdElement(new dom::Element(U"td"));
                AppendTypeName(typeTdElement.get(), parentElement, variableTypeId, contentXml);
                typeTdElement->SetAttribute(U"class", U"functionTableFirstCol");
                typeTdElement->SetAttribute(U"xml:space", U"preserve");
                trElement->AppendChild(std::unique_ptr<dom::Node>(typeTdElement.release()));
                std::unique_ptr<dom::Element> nameTdElement(new dom::Element(U"td"));
                nameTdElement->SetAttribute(U"class", U"functionTableSecondCol");
                nameTdElement->SetAttribute(U"xml:space", U"preserve");
                nameTdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(variableName)));
                if (!specifiers.empty())
                {
                    nameTdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
                    std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                    span->SetAttribute(U"class", U"kw");
                    span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(specifiers)));
                    nameTdElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
                }
                trElement->AppendChild(std::unique_ptr<dom::Node>(nameTdElement.release()));
                variableTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
        }
        if (n > 0)
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(variableTableElement.release()));
        }
    }
}

void GenerateTypedefSection(dom::Element* pageElement, dom::Element* parentElement, dom::Document* contentXml,
    const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName)
{
    std::unique_ptr<dom::Element> typedefTableElement(new dom::Element(U"table"));
    std::unique_ptr<xpath::XPathObject> typedefs = xpath::Evaluate(U"typedefs/typedef", parentElement);
    if (typedefs->Type() == xpath::XPathObjectType::nodeSet)
    {
        std::u32string indexLink = U"../index.html";
        std::u32string parentLink = parentElement->GetAttribute(U"id") + U".html";
        std::u32string prevLink;
        std::u32string nextLink;
        xpath::XPathNodeSet* typedefNodeSet = static_cast<xpath::XPathNodeSet*>(typedefs.get());
        int n = typedefNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Typedefs")));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        }
        for (int i = 0; i < n; ++i)
        {
            dom::Node* typedefNode = (*typedefNodeSet)[i];
            if (typedefNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* typedefElement = static_cast<dom::Element*>(typedefNode);
                std::u32string typedefTypeId = typedefElement->GetAttribute(U"type");
                std::u32string typedefName = typedefElement->GetAttribute(U"name");
                std::u32string typedefId = typedefElement->GetAttribute(U"id");
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                trElement->SetAttribute(U"id", typedefId);
                std::unique_ptr<dom::Element> typeTdElement(new dom::Element(U"td"));
                AppendTypeName(typeTdElement.get(), parentElement, typedefTypeId, contentXml);
                typeTdElement->SetAttribute(U"class", U"functionTableFirstCol");
                typeTdElement->SetAttribute(U"xml:space", U"preserve");
                trElement->AppendChild(std::unique_ptr<dom::Node>(typeTdElement.release()));
                std::unique_ptr<dom::Element> nameTdElement(new dom::Element(U"td"));
                nameTdElement->SetAttribute(U"class", U"functionTableSecondCol");
                nameTdElement->SetAttribute(U"xml:space", U"preserve");
                nameTdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typedefName)));
                trElement->AppendChild(std::unique_ptr<dom::Node>(nameTdElement.release()));
                typedefTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
        }
        if (n > 0)
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(typedefTableElement.release()));
        }
    }
}

void GenerateConstructorSection(dom::Element* pageElement, dom::Element* parentElement, const std::string& contentDir,
    const std::string& styleDirName, const std::string& styleFileName, dom::Document* contentXml, std::vector<dom::Element*>& constructorElements)
{
    std::unique_ptr<dom::Element> constructorTableElement(new dom::Element(U"table"));
    constructorTableElement->SetAttribute(U"class", U"constructorTable");
    std::unique_ptr<xpath::XPathObject> constructors = xpath::Evaluate(U"constructors/constructor", parentElement);
    if (constructors->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* constructorNodeSet = static_cast<xpath::XPathNodeSet*>(constructors.get());
        int n = constructorNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Constructors")));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        }
        for (int i = 0; i < n; ++i)
        {
            dom::Node* constructorNode = (*constructorNodeSet)[i];
            if (constructorNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* constructorElement = static_cast<dom::Element*>(constructorNode);
                constructorElements.push_back(constructorElement);
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"class", U"constructorTableFirstCol");
                tdElement->SetAttribute(U"xml:space", U"preserve");
                std::u32string constuctorName = MakeConstructorName(constructorElement);
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                std::u32string constructorLink = parentElement->GetAttribute(U"id");
                constructorLink.append(U".html#").append(constructorElement->GetAttribute(U"id"));
                linkElement->SetAttribute(U"href", constructorLink);
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(constuctorName)));
                tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                AppendParameterList(tdElement.get(), parentElement, constructorElement, contentXml);
                std::u32string specifiers = constructorElement->GetAttribute(U"specifiers");
                if (!specifiers.empty())
                {
                    tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
                    std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                    span->SetAttribute(U"class", U"kw");
                    span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(specifiers)));
                    tdElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
                }
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                constructorTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
        }
        if (n > 0)
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(constructorTableElement.release()));
        }
    }
}

void GenerateFunctionSection(const std::u32string& title, dom::Element* pageElement, dom::Element* parentElement, const std::string& contentDir, 
    const std::string& styleDirName, const std::string& styleFileName, dom::Document* contentXml, std::vector<dom::Element*>& functionElements)
{
    std::unique_ptr<dom::Element> functionTableElement(new dom::Element(U"table"));
    functionTableElement->SetAttribute(U"class", U"functionTable");
    std::unique_ptr<xpath::XPathObject> functions = xpath::Evaluate(U"functions/function", parentElement);
    if (functions->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* functionNodeSet = static_cast<xpath::XPathNodeSet*>(functions.get());
        int n = functionNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        }
        for (int i = 0; i < n; ++i)
        {
            dom::Node* functionNode = (*functionNodeSet)[i];
            if (functionNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* functionElement = static_cast<dom::Element*>(functionNode);
                functionElements.push_back(functionElement);
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::u32string returnTypeId = functionElement->GetAttribute(U"returnType");
                if (returnTypeId.empty())
                {
                    std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                    tdElement->SetAttribute(U"class", U"functionTableFirstCol");
                    tdElement->SetAttribute(U"xml:space", U"preserve");
                    trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                }
                else
                {
                    std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                    tdElement->SetAttribute(U"class", U"functionTableFirstCol");
                    tdElement->SetAttribute(U"xml:space", U"preserve");
                    AppendTypeName(tdElement.get(), parentElement, returnTypeId, contentXml);
                    trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                }
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"class", U"functionTableSecondCol");
                tdElement->SetAttribute(U"xml:space", U"preserve");
                std::u32string functionName = MakeFunctionName(functionElement);
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                std::u32string functionLink = parentElement->GetAttribute(U"id");
                functionLink.append(U".html#").append(functionElement->GetAttribute(U"id"));
                linkElement->SetAttribute(U"href", functionLink);
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(functionName)));
                tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                AppendParameterList(tdElement.get(), parentElement, functionElement, contentXml);
                std::u32string specifiers = functionElement->GetAttribute(U"specifiers");
                if (!specifiers.empty())
                {
                    tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
                    std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                    span->SetAttribute(U"class", U"kw");
                    span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(specifiers)));
                    tdElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
                }
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                functionTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
        }
        if (n > 0)
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(functionTableElement.release()));
        }
    }
}

struct ByFunctionName
{
    bool operator()(const std::pair<std::u32string, std::unique_ptr<dom::Element>>& left, const std::pair<std::u32string, std::unique_ptr<dom::Element>>& right) const
    {
        return left.first < right.first;
    }
};

void GenerateFunctionDetailSection(const std::u32string& title, const std::u32string& functionTitle, dom::Element* pageElement, dom::Element* parentElement,
    std::vector<dom::Element*>& functionElements, dom::Document* contentXml, std::unordered_map<std::u32string, std::unique_ptr<gendoc::dom::Element>>& inlineCodeMap)
{
    if (functionElements.empty()) return;
    std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
    h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    pageElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
    int n = functionElements.size();
    for (int i = 0; i < n; ++i)
    {
        dom::Element* functionElement = functionElements[i];
        std::unique_ptr<dom::Element> h3Element(new dom::Element(U"h3"));
        h3Element->SetAttribute(U"id", functionElement->GetAttribute(U"id"));
        std::u32string functionText = functionElement->GetAttribute(U"name");
        functionText.append(1, ' ').append(functionTitle);
        h3Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(functionText)));
        pageElement->AppendChild(std::unique_ptr<dom::Node>(h3Element.release()));
        std::unique_ptr<dom::Element> h4Element(new dom::Element(U"h4"));
        std::u32string returnTypeId = functionElement->GetAttribute(U"returnType");
        if (!returnTypeId.empty())
        {
            AppendTypeName(h4Element.get(), parentElement, returnTypeId, contentXml);
            h4Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
        }
        std::u32string functionName;
        if (functionElement->Name() == U"function")
        {
            functionName = MakeFunctionName(functionElement, true);
        }
        else if (functionElement->Name() == U"constructor")
        {
            functionName = MakeConstructorName(functionElement, true);
        }
        h4Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(functionName)));
        AppendParameterList(h4Element.get(), parentElement, functionElement, contentXml);
        std::u32string specifiers = functionElement->GetAttribute(U"specifiers");
        if (!specifiers.empty())
        {
            h4Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
            std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
            span->SetAttribute(U"class", U"kw");
            span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(specifiers)));
            h4Element->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        }
        pageElement->AppendChild(std::unique_ptr<dom::Node>(h4Element.release()));
        bool codeAdded = false;
        std::u32string definitionFileId = functionElement->GetAttribute(U"definitionFileId");
        if (!definitionFileId.empty())
        {
            std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
            span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Definition at line ")));
            std::unique_ptr<dom::Element> lineLink(new dom::Element(U"a"));
            std::u32string line = functionElement->GetAttribute(U"definitionLine");
            lineLink->SetAttribute(U"href", definitionFileId + U"#" + line);
            lineLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(line)));
            span->AppendChild(std::unique_ptr<dom::Node>(lineLink.release()));
            span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" of ")));
            std::unique_ptr<dom::Element> fileLink(new dom::Element(U"a"));
            fileLink->SetAttribute(U"href", definitionFileId);
            fileLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(functionElement->GetAttribute(U"definitionFile"))));
            span->AppendChild(std::unique_ptr<dom::Node>(fileLink.release()));
            auto it = inlineCodeMap.find(functionElement->GetAttribute(U"id"));
            if (it != inlineCodeMap.cend())
            {
                dom::Element* inlineCode = it->second.get();
                if (inlineCode)
                {
                    span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U":")));
                    span->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
                    std::unique_ptr<dom::Element> inlineCode = std::move(it->second);
                    span->AppendChild(std::unique_ptr<dom::Node>(inlineCode->CloneNode(true)));
                    span->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
                    codeAdded = true;
                }
            }
            pageElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
            if (!codeAdded)
            {
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            }
        }
        std::u32string declarationFileId = functionElement->GetAttribute(U"declarationFileId");
        if (!declarationFileId.empty())
        {
            if (codeAdded)
            {
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            }
            std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
            span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Declaration at line ")));
            std::unique_ptr<dom::Element> lineLink(new dom::Element(U"a"));
            std::u32string line = functionElement->GetAttribute(U"declarationLine");
            lineLink->SetAttribute(U"href", declarationFileId + U"#" + line);
            lineLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(line)));
            span->AppendChild(std::unique_ptr<dom::Node>(lineLink.release()));
            span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" of ")));
            std::unique_ptr<dom::Element> fileLink(new dom::Element(U"a"));
            fileLink->SetAttribute(U"href", declarationFileId);
            fileLink->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(functionElement->GetAttribute(U"declarationFile"))));
            span->AppendChild(std::unique_ptr<dom::Node>(fileLink.release()));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
            pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
        }
        bool hasOverriddenFunctions = false;
        std::unique_ptr<xpath::XPathObject> overriddenFunctions = xpath::Evaluate(U"overriddenFunctions/overriddenFunction", functionElement);
        if (overriddenFunctions->Type() == xpath::XPathObjectType::nodeSet)
        {
            xpath::XPathNodeSet* overriddenNodeSet = static_cast<xpath::XPathNodeSet*>(overriddenFunctions.get());
            int n = overriddenNodeSet->Length();
            if (n > 0)
            {
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
                hasOverriddenFunctions = true;
                std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Base class overridden functions: ")));
                std::vector<std::pair<std::u32string, std::unique_ptr<dom::Element>>> overriddenFunctionsVec;
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* overriddenNode = (*overriddenNodeSet)[i];
                    if (overriddenNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* overriddenIdElement = static_cast<dom::Element*>(overriddenNode);
                        std::u32string overriddenId = overriddenIdElement->GetAttribute(U"functionId");
                        if (!overriddenId.empty())
                        {
                            dom::Element* overriddenElement = contentXml->GetElementById(overriddenId);
                            dom::ParentNode* overriddenParent = overriddenElement->Parent();
                            if (overriddenParent->GetNodeType() == dom::NodeType::elementNode)
                            {
                                dom::Element* overriddenParentElement = static_cast<dom::Element*>(overriddenParent);
                                if (overriddenParentElement->Name() == U"functions")
                                {
                                    dom::ParentNode* functionsParent = overriddenParentElement->Parent();
                                    if (functionsParent->GetNodeType() == dom::NodeType::elementNode)
                                    {
                                        dom::Element* classElement = static_cast<dom::Element*>(functionsParent);
                                        if (classElement->Name() == U"class")
                                        {
                                            std::u32string classId = classElement->GetAttribute(U"id");
                                            if (!classId.empty())
                                            {
                                                std::u32string classLink = classId + U".html";
                                                std::u32string overriddenLink = classLink + U"#" + overriddenId;
                                                std::u32string overriddenFunctionName = MakeFunctionName(overriddenElement, true);
                                                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                                                linkElement->SetAttribute(U"href", overriddenLink);
                                                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(overriddenFunctionName)));
                                                overriddenFunctionsVec.push_back(std::make_pair(overriddenFunctionName, std::move(linkElement)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                std::sort(overriddenFunctionsVec.begin(), overriddenFunctionsVec.end(), ByFunctionName());
                int n = overriddenFunctionsVec.size();
                for (int i = 0; i < n; ++i)
                {
                    if (i > 0)
                    {
                        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
                    }
                    std::pair<std::u32string, std::unique_ptr<dom::Element>>& p = overriddenFunctionsVec[i];
                    std::unique_ptr<dom::Element> linkElement = std::move(p.second);
                    span->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                }
                pageElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            }
        }
        bool hasOverrides = false;
        std::unique_ptr<xpath::XPathObject> overrides = xpath::Evaluate(U"overrides/override", functionElement);
        if (overrides->Type() == xpath::XPathObjectType::nodeSet)
        {
            xpath::XPathNodeSet* overrideNodeSet = static_cast<xpath::XPathNodeSet*>(overrides.get());
            int n = overrideNodeSet->Length();
            if (n > 0)
            {
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
                hasOverrides = true;
                std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Derived class overrides: ")));
                std::vector<std::pair<std::u32string, std::unique_ptr<dom::Element>>> overrideVec;
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* overrideNode = (*overrideNodeSet)[i];
                    if (overrideNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* overrideIdElement = static_cast<dom::Element*>(overrideNode);
                        std::u32string overrideId = overrideIdElement->GetAttribute(U"functionId");
                        if (!overrideId.empty())
                        {
                            dom::Element* overrideElement = contentXml->GetElementById(overrideId);
                            dom::ParentNode* overrideParent = overrideElement->Parent();
                            if (overrideParent->GetNodeType() == dom::NodeType::elementNode)
                            {
                                dom::Element* overrideParentElement = static_cast<dom::Element*>(overrideParent);
                                if (overrideParentElement->Name() == U"functions")
                                {
                                    dom::ParentNode* functionsParent = overrideParentElement->Parent();
                                    if (functionsParent->GetNodeType() == dom::NodeType::elementNode)
                                    {
                                        dom::Element* classElement = static_cast<dom::Element*>(functionsParent);
                                        if (classElement->Name() == U"class")
                                        {
                                            std::u32string classId = classElement->GetAttribute(U"id");
                                            if (!classId.empty())
                                            {
                                                std::u32string classLink = classId + U".html";
                                                std::u32string overrideLink = classLink + U"#" + overrideId;
                                                std::u32string overrideFunctionName = MakeFunctionName(overrideElement, true);
                                                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                                                linkElement->SetAttribute(U"href", overrideLink);
                                                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(overrideFunctionName)));
                                                overrideVec.push_back(std::make_pair(overrideFunctionName, std::move(linkElement)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                std::sort(overrideVec.begin(), overrideVec.end(), ByFunctionName());
                int n = overrideVec.size();
                for (int i = 0; i < n; ++i)
                {
                    if (i > 0)
                    {
                        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
                    }
                    std::pair<std::u32string, std::unique_ptr<dom::Element>>& p = overrideVec[i];
                    std::unique_ptr<dom::Element> linkElement = std::move(p.second);
                    span->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                }
                pageElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            }
        }
        bool hasCalls = false;
        std::unique_ptr<xpath::XPathObject> calls = xpath::Evaluate(U"calls/call", functionElement);
        if (calls->Type() == xpath::XPathObjectType::nodeSet)
        {
            xpath::XPathNodeSet* callNodeSet = static_cast<xpath::XPathNodeSet*>(calls.get());
            int n = callNodeSet->Length();
            if (n > 0)
            {
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
                hasCalls = true;
                std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Calls: ")));
                std::vector<std::pair<std::u32string, std::unique_ptr<dom::Element>>> callVec;
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* callNode = (*callNodeSet)[i];
                    if (callNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* callIdElement = static_cast<dom::Element*>(callNode);
                        std::u32string callId = callIdElement->GetAttribute(U"functionId");
                        if (!callId.empty())
                        {
                            dom::Element* callElement = contentXml->GetElementById(callId);
                            dom::ParentNode* callParent = callElement->Parent();
                            if (callParent->GetNodeType() == dom::NodeType::elementNode)
                            {
                                dom::Element* callParentElement = static_cast<dom::Element*>(callParent);
                                if (callParentElement->Name() == U"functions")
                                {
                                    dom::ParentNode* functionsParent = callParentElement->Parent();
                                    if (functionsParent->GetNodeType() == dom::NodeType::elementNode)
                                    {
                                        dom::Element* parentElement = static_cast<dom::Element*>(functionsParent);
                                        if (parentElement->Name() == U"class" || parentElement->Name() == U"namespace")
                                        {
                                            std::u32string parentId = parentElement->GetAttribute(U"id");
                                            if (!parentId.empty())
                                            {
                                                std::u32string parentLink = parentId + U".html";
                                                std::u32string callLink = parentLink + U"#" + callId;
                                                std::u32string callFunctionName = MakeFunctionName(callElement, true);
                                                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                                                linkElement->SetAttribute(U"href", callLink);
                                                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(callFunctionName)));
                                                callVec.push_back(std::make_pair(callFunctionName, std::move(linkElement)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                std::sort(callVec.begin(), callVec.end(), ByFunctionName());
                int n = callVec.size();
                for (int i = 0; i < n; ++i)
                {
                    if (i > 0)
                    {
                        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
                    }
                    std::pair<std::u32string, std::unique_ptr<dom::Element>>& p = callVec[i];
                    std::unique_ptr<dom::Element> linkElement = std::move(p.second);
                    span->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                }
                pageElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            }
        }
        bool hasCalledBy = false;
        std::unique_ptr<xpath::XPathObject> calledBy = xpath::Evaluate(U"calledByFunctions/calledByFunction", functionElement);
        if (calledBy->Type() == xpath::XPathObjectType::nodeSet)
        {
            xpath::XPathNodeSet* calledByNodeSet = static_cast<xpath::XPathNodeSet*>(calledBy.get());
            int n = calledByNodeSet->Length();
            if (n > 0)
            {
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
                hasCalledBy = true;
                std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Called by: ")));
                std::vector<std::pair<std::u32string, std::unique_ptr<dom::Element>>> calledByVec;
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* calledByNode = (*calledByNodeSet)[i];
                    if (calledByNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* calledByIdElement = static_cast<dom::Element*>(calledByNode);
                        std::u32string calledById = calledByIdElement->GetAttribute(U"functionId");
                        if (!calledById.empty())
                        {
                            dom::Element* calledByElement = contentXml->GetElementById(calledById);
                            dom::ParentNode* calledByParent = calledByElement->Parent();
                            if (calledByParent->GetNodeType() == dom::NodeType::elementNode)
                            {
                                dom::Element* calledByParentElement = static_cast<dom::Element*>(calledByParent);
                                if (calledByParentElement->Name() == U"functions")
                                {
                                    dom::ParentNode* functionsParent = calledByParentElement->Parent();
                                    if (functionsParent->GetNodeType() == dom::NodeType::elementNode)
                                    {
                                        dom::Element* parentElement = static_cast<dom::Element*>(functionsParent);
                                        if (parentElement->Name() == U"class" || parentElement->Name() == U"namespace")
                                        {
                                            std::u32string parentId = parentElement->GetAttribute(U"id");
                                            if (!parentId.empty())
                                            {
                                                std::u32string parentLink = parentId + U".html";
                                                std::u32string calledByLink = parentLink + U"#" + calledById;
                                                std::u32string calledByFunctionName = MakeFunctionName(calledByElement, true);
                                                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                                                linkElement->SetAttribute(U"href", calledByLink);
                                                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(calledByFunctionName)));
                                                calledByVec.push_back(std::make_pair(calledByFunctionName, std::move(linkElement)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                std::sort(calledByVec.begin(), calledByVec.end(), ByFunctionName());
                int n = calledByVec.size();
                for (int i = 0; i < n; ++i)
                {
                    if (i > 0)
                    {
                        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
                    }
                    std::pair<std::u32string, std::unique_ptr<dom::Element>>& p = calledByVec[i];
                    std::unique_ptr<dom::Element> linkElement = std::move(p.second);
                    span->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                }
                pageElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
                pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            }
        }
        if (!definitionFileId.empty() || !declarationFileId.empty() || hasOverrides || hasOverriddenFunctions || hasCalls || hasCalledBy)
        {
            pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
        }
        pageElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"hr")));
    }
}

void GenerateContent(dom::Document* contentXml, const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName,
    std::unordered_map<std::u32string, std::unique_ptr<gendoc::dom::Element>>& inlineCodeMap)
{
    std::unique_ptr<xpath::XPathObject> namespaces = xpath::Evaluate(U"//namespace", contentXml);
    if (namespaces->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* namespaceNodeSet = static_cast<xpath::XPathNodeSet*>(namespaces.get());
        std::u32string indexLink = U"../index.html";
        std::u32string parentLink = indexLink;
        std::u32string prevLink;
        std::u32string nextLink;
        int n = namespaceNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            nextLink.clear();
            dom::Node* nsNode = (*namespaceNodeSet)[i];
            if (nsNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* nsElement = static_cast<dom::Element*>(nsNode);
                if (i < n - 1)
                {
                    dom::Node* nextNsNode = (*namespaceNodeSet)[i + 1];
                    if (nextNsNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* nextNsElement = static_cast<dom::Element*>(nextNsNode);
                        std::u32string nextNsId = nextNsElement->GetAttribute(U"id");
                        std::string nextNsFileName = ToUtf8(nextNsId) + ".html";
                        nextLink = ToUtf32(nextNsFileName);
                    }
                }
                std::vector<dom::Element*> functionElements;
                std::u32string nsName = nsElement->GetAttribute(U"name");
                std::u32string nsId = nsElement->GetAttribute(U"id");
                std::string nsFileName = ToUtf8(nsId) + ".html";
                std::string nsFilePath = Path::Combine(contentDir, nsFileName);
                std::ofstream nsFile(nsFilePath);
                CodeFormatter formatter(nsFile);
                dom::Document nsDocument;
                std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
                std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
                std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
                metaElement->SetAttribute(U"charset", U"utf-8");
                headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
                std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
                std::u32string title;
                if (nsName.empty())
                {
                    title = U"Global Namespace";
                }
                else
                {
                    title = MakeTitle(nsName) + U" Namespace";
                }
                titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
                headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
                linkElement->SetAttribute(U"rel", U"stylesheet");
                linkElement->SetAttribute(U"type", U"text/css");
                std::u32string relativeStyleFilePath = ToUtf32(Path::Combine(Path::Combine("..", styleDirName), styleFileName));
                linkElement->SetAttribute(U"href", relativeStyleFilePath);
                headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
                std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
                GenerateNavigation(bodyElement.get(), Navigation::header, indexLink, parentLink, prevLink, nextLink);
                std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
                h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
                bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
                GenerateNamespaceSection(nsElement, bodyElement.get(), contentXml, std::u32string());
                GenerateGrammarSection(bodyElement.get(), nsElement, contentXml, contentDir, styleDirName, styleFileName);
                GenerateClassSection(bodyElement.get(), nsElement, contentXml, contentDir, styleDirName, styleFileName, inlineCodeMap);
                GenerateFunctionSection(U"Functions", bodyElement.get(), nsElement, contentDir, styleDirName, styleFileName, contentXml, functionElements);
                GenerateEnumSection(bodyElement.get(), nsElement, contentXml, contentDir, styleDirName, styleFileName);
                GenerateTypedefSection(bodyElement.get(), nsElement, contentXml, contentDir, styleDirName, styleFileName);
                GenerateVariableSection(U"Variables", bodyElement.get(), nsElement, contentXml, contentDir, styleDirName, styleFileName);
                bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"hr")));
                GenerateFunctionDetailSection(U"Function Details", U"Function", bodyElement.get(), nsElement, functionElements, contentXml, inlineCodeMap);
                GenerateNavigation(bodyElement.get(), Navigation::footer, indexLink, parentLink, prevLink, nextLink);
                htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
                nsDocument.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
                nsDocument.Write(formatter);
                prevLink = ToUtf32(nsFileName);
            }
        }
    }
}

void GenerateComponentHtml(dom::Document* inputXml, dom::Document* contentXml, const std::string& contentDir, const std::string& styleDirName, const std::string& styleFileName,
    const std::vector<dom::Element*>& componentElements, const std::unordered_map<dom::Element*, std::string>& componentElementFileNameMap, 
    const std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles, const std::vector<std::unique_ptr<gendoc::syntax::ParserFileContent>>& parserFileContents,
    const std::unordered_map<int, std::u32string>& parserProjectMap, std::vector<std::unique_ptr<SourceFileNode>>& parserSourceFileNodes)
{
    std::u32string indexLink = U"../index.html";
    std::u32string parentLink = indexLink;
    std::u32string prevLink;
    std::u32string nextLink;
    int n = componentElements.size();
    for (int i = 0; i < n; ++i)
    {
        dom::Element* componentElement = componentElements[i];
        nextLink.clear();
        if (i < n - 1)
        {
            dom::Element* nextComponentElement = componentElements[i + 1];
            std::u32string nextComponentName = nextComponentElement->GetAttribute(U"name");
            std::string nextComponentFileName = MakeComponentFileName(nextComponentName);
            nextLink = ToUtf32(nextComponentFileName);
        }
        auto it = componentElementFileNameMap.find(componentElement);
        if (it != componentElementFileNameMap.cend())
        {
            const std::string& componentFileName = it->second;
            std::string componentFilePath = Path::Combine(contentDir, componentFileName);
            std::ofstream componentFile(componentFilePath);
            CodeFormatter formatter(componentFile);
            std::u32string componentName = componentElement->GetAttribute(U"name");
            std::u32string componentDescription = componentElement->GetAttribute(U"description");
            std::u32string componentKind = componentElement->GetAttribute(U"kind");
            dom::Document componentDocument;
            std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
            std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
            std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
            metaElement->SetAttribute(U"charset", U"utf-8");
            headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
            std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
            std::u32string title = MakeTitle(componentName) + U" Component";
            titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
            headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
            linkElement->SetAttribute(U"rel", U"stylesheet");
            linkElement->SetAttribute(U"type", U"text/css");
            std::u32string relativeStyleFilePath = ToUtf32(Path::Combine(Path::Combine("..", styleDirName), styleFileName));
            linkElement->SetAttribute(U"href", relativeStyleFilePath);
            headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
            std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
            GenerateNavigation(bodyElement.get(), Navigation::header, indexLink, parentLink, prevLink, nextLink);
            std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
            std::u32string documentTitle = componentDescription;
            if (componentKind == U"library")
            {
                documentTitle.append(U" Library");
            }
            else if (componentKind == U"program")
            {
                documentTitle.append(U" Program");
            }
            documentTitle.append(U" Component");
            h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(documentTitle)));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
            GenerateComponenNamespaceSection(bodyElement.get(), contentXml, componentName);
            GenerateComponentFileSection(sourceFiles, parserFileContents, parserProjectMap, parserSourceFileNodes, bodyElement.get(), componentName);
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"hr")));
            GenerateNavigation(bodyElement.get(), Navigation::footer, indexLink, parentLink, prevLink, nextLink);
            htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
            componentDocument.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
            componentDocument.Write(formatter);
            prevLink = ToUtf32(componentFileName);
        }
        else
        {
            throw std::runtime_error("component '" + ToUtf8(componentElement->Name()) + "' not found");
        }
    }
}

int Log10(int n)
{
    int log10 = 1;
    int m = n / 10;
    while (m > 0)
    {
        ++log10;
        m = m / 10;
    }
    return log10;
}

void GenerateHtmlCodeFiles(const std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles, const std::string& styleDirName, const std::string& styleFileName,
    gendoc::cppsym::SymbolTable& symbolTable, int inlineCodeLimit, std::unordered_map<std::u32string, std::unique_ptr<dom::Element>>& inlineCodeMap)
{
    for (gendoc::cppast::SourceFileNode* sourceFile : sourceFiles)
    {
        std::ifstream inputFile(sourceFile->SourceFilePath());
        std::vector<std::u32string> inputLines;
        std::string line;
        while (std::getline(inputFile, line))
        {
            inputLines.push_back(ToUtf32(line));
        }
        int n = inputLines.size();
        int numDigits = Log10(n);
        HtmlSourceCodeGenerator generator(inputLines, numDigits, styleDirName, styleFileName, symbolTable, inlineCodeLimit, inlineCodeMap);
        sourceFile->Accept(generator);
        generator.WriteDocument();
    }
}

ParserToken* parserTokenGrammar = nullptr;

void GenerateHtmlParserFiles(const std::vector<gendoc::cppast::SourceFileNode*>& parserSourceFiles, const std::string& styleDirName, 
    const std::string& styleFileName)
{
    if (!parserTokenGrammar)
    {
        parserTokenGrammar = ParserToken::Create();
    }
    std::string styleFilePath(Path::Combine(Path::Combine("../../..", styleDirName), styleFileName));
    for (gendoc::cppast::SourceFileNode* parserSourceFile : parserSourceFiles)
    {
        std::string htmlFilePath = parserSourceFile->HtmlSourceFilePath();
        std::ofstream htmlFile(htmlFilePath);
        CodeFormatter formatter(htmlFile);
        HtmlSourceCodeWriter writer;
        std::ifstream inputFile(parserSourceFile->SourceFilePath());
        std::vector<std::u32string> inputLines;
        std::string line;
        while (std::getline(inputFile, line))
        {
            inputLines.push_back(ToUtf32(line));
        }
        int n = inputLines.size();
        int numDigits = Log10(n);
        dom::Document htmlParserFileDocument;
        std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
        std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
        std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
        metaElement->SetAttribute(U"charset", U"utf-8");
        headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
        std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
        titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(parserSourceFile->ProjectName() + U"/" + ToUtf32(parserSourceFile->RelativeSourceFilePath()))));
        std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
        linkElement->SetAttribute(U"rel", U"stylesheet");
        linkElement->SetAttribute(U"type", U"text/css");
        std::u32string relativeStyleFilePath = ToUtf32(styleFilePath);
        linkElement->SetAttribute(U"href", relativeStyleFilePath);
        headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
        htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
        std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
        int currentSourceLineNumber = 1;
        for (const std::u32string& inputLine : inputLines)
        {
            std::unique_ptr<dom::Element> lineElement(new dom::Element(U"span"));
            lineElement->SetAttribute(U"class", U"code");
            lineElement->SetAttribute(U"xml:space", U"preserve");
            lineElement->SetAttribute(U"id", ToUtf32(std::to_string(currentSourceLineNumber)));
            writer.SetLineElement(lineElement.get());
            std::u32string lineNumberText = FormatNumber(currentSourceLineNumber, numDigits);
            writer.WriteLineNumber(lineNumberText);
            writer.WriteSpace(1);
            parserTokenGrammar->Parse(inputLine.c_str(), inputLine.c_str() + inputLine.length(), 0, "", &writer);
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(lineElement.release()));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            ++currentSourceLineNumber;
        }
        htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
        htmlParserFileDocument.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
        htmlParserFileDocument.Write(formatter);
    }
}

} } // namespace gendoc::html
