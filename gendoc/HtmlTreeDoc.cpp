// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/gendoc/HtmlTreeDoc.hpp>
#include <gendoc/gendoc/TreeCode.hpp>
#include <gendoc/dom/Document.hpp>
#include <gendoc/dom/Parser.hpp>
#include <gendoc/dom/CharacterData.hpp>
#include <gendoc/parsing/Utility.hpp>
#include <gendoc/util/Unicode.hpp>
#include <gendoc/util/CodeFormatter.hpp>
#include <gendoc/util/Path.hpp>
#include <gendoc/util/Util.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>

namespace gendoc {namespace html {

using namespace gendoc::unicode;
using namespace gendoc::util;

struct TreeNode
{
    TreeNode(const std::u32string& name_, const std::u32string& id_, int level_, const std::u32string& link_);
    void Write(CodeFormatter& formatter);
    std::u32string name;
    std::u32string id;
    int level;
    std::u32string link;
};

TreeNode::TreeNode(const std::u32string& name_, const std::u32string& id_, int level_, const std::u32string& link_) : name(name_), id(id_), level(level_), link(link_)
{
}

void TreeNode::Write(CodeFormatter& formatter)
{
    formatter.Write("{ ");
    formatter.Write("name: \"");
    formatter.Write(gendoc::parsing::XmlEscape(ToUtf8(name)));
    formatter.Write("\", ");
    if (!id.empty())
    {
        formatter.Write("id: \"" + ToUtf8(id) + "\", ");
    }
    else
    {
        formatter.Write("id: \"\", ");
    }
    formatter.Write("level: ");
    formatter.Write(std::to_string(level));
    formatter.Write(", ");
    formatter.Write("link: \"");
    formatter.Write(ToUtf8(link));
    formatter.Write("\"");
    formatter.Write(" }");
}

void GenerateTreeNodes(const std::u32string& prefix, dom::Document* treeXmlDoc, const std::vector<gendoc::cppsym::NamespaceSymbol*>& ns, std::vector<TreeNode>& treeNodes, 
    dom::Element* nodeElement, int level, bool siblings, std::unordered_set<std::u32string>& visited)
{
    bool visit = true;
    std::u32string nodeId = nodeElement->GetAttribute(U"id");
    if (!nodeId.empty())
    {
        if (visited.find(nodeId) != visited.cend())
        {
            visit = false;
        }
        else
        {
            visited.insert(nodeId);
        }
    }
    if (visit)
    {
        std::u32string name = nodeElement->Name();
        gendoc::cppsym::Symbol* symbol = nullptr;
        bool nodeInserted = false;
        bool setName = false;
        bool processChildren = true;
        if (name != U"symbolTable" && name != U"symbols" && name != U"types" && name != U"type" && name != U"returnType")
        {
            for (gendoc::cppsym::NamespaceSymbol* n : ns)
            {
                symbol = n->GetContainerScope()->Lookup(name);
                if (symbol) break;
            }
        }
        else if (name == U"symbolTable")
        {
            for (gendoc::cppsym::NamespaceSymbol* n : ns)
            {
                symbol = n->GetContainerScope()->Lookup(U"SymbolTable");
                if (symbol) break;
            }
        }
        else if (name == U"type" || name == U"returnType")
        {
            std::u32string ref = nodeElement->GetAttribute(U"ref");
            if (!ref.empty())
            {
                dom::Element* referredNode = treeXmlDoc->GetElementById(ref);
                if (referredNode && referredNode->FirstChild() && referredNode->FirstChild()->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* childNode = static_cast<dom::Element*>(referredNode->FirstChild());
                    for (gendoc::cppsym::NamespaceSymbol* n : ns)
                    {
                        symbol = n->GetContainerScope()->Lookup(childNode->Name());
                        if (symbol) break;
                    }
                    if (symbol)
                    {
                        if (symbol->Name() != U"ClassTemplateSpecializationSymbol")
                        {
                            GenerateTreeNodes(name, treeXmlDoc, ns, treeNodes, childNode, level, false, visited);
                            visited.insert(ref);
                            processChildren = false;
                            nodeInserted = true;
                            symbol = nullptr;
                        }
                        else
                        {
                            name.append(1, '(').append(ref).append(1, ')');
                            treeNodes.push_back(TreeNode(U"type: " + ref, U"", level, ref));
                            processChildren = false;
                            nodeInserted = true;
                            symbol = nullptr;
                        }
                    }
                }
            }
            else if (!nodeId.empty())
            {
                name = nodeId;
            }
        }
        if (symbol)
        {
            if (symbol->IsClassGroupSymbol())
            {
                ClassGroupSymbol* group = static_cast<ClassGroupSymbol*>(symbol);
                int classArity = 0;
                std::u32string classArityStr = nodeElement->GetAttribute(U"classArity");
                if (!classArityStr.empty())
                {
                    classArity = boost::lexical_cast<int>(ToUtf8(classArityStr));
                }
                symbol = group->GetClass(classArity);
            }
            if (symbol->IsClassTypeSymbol())
            {
                ClassTypeSymbol* cls = static_cast<ClassTypeSymbol*>(symbol);
                std::u32string info = nodeElement->GetAttribute(U"info");
                if (!info.empty())
                {
                    name.append(1, '(').append(info).append(1, ')');
                }
                std::u32string link = cls->Id() + U".html";
                std::u32string fullName = prefix;
                if (!fullName.empty())
                {
                    fullName.append(U": ");
                }
                fullName.append(name);
                treeNodes.push_back(TreeNode(fullName, U"", level, link));
            }
            else
            {
                throw std::runtime_error("class type symbol expected");
            }
        }
        else if (!nodeInserted)
        {
            std::u32string id = nodeElement->GetAttribute(U"id");
            std::u32string link = U"";
            std::u32string fullName = prefix;
            if (!fullName.empty())
            {
                fullName.append(U": ");
            }
            fullName.append(name);
            treeNodes.push_back(TreeNode(fullName, id, level, link));
        }
        dom::Node* child = nullptr;
        if (processChildren)
        {
            child = nodeElement->FirstChild();
        }
        if (child && child->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* childElement = static_cast<dom::Element*>(child);
            GenerateTreeNodes(U"", treeXmlDoc, ns, treeNodes, childElement, level + 1, true, visited);
        }
    }
    if (siblings)
    {
        dom::Node* siblingNode = nodeElement->NextSibling();
        while (siblingNode && siblingNode->GetNodeType() == dom::NodeType::elementNode)
        {
            nodeElement = static_cast<dom::Element*>(siblingNode);
            GenerateTreeNodes(U"", treeXmlDoc, ns, treeNodes, nodeElement, level, false, visited);
            siblingNode = nodeElement->NextSibling();
        }
    }
}

std::vector<gendoc::cppsym::NamespaceSymbol*> GetNamespaces(SymbolTable& symbolTable, const std::vector<std::u32string>& namespaceNames)
{
    std::vector<gendoc::cppsym::NamespaceSymbol*> namespaces;
    for (const std::u32string& namespaceName : namespaceNames)
    {
        std::vector<std::u32string> components = Split(namespaceName, '.');
        gendoc::cppsym::NamespaceSymbol* ns = symbolTable.GlobalNs();
        int n = components.size();
        for (int i = 0; i < n; ++i)
        {
            std::u32string component = components[i];
            Symbol* symbol = ns->GetContainerScope()->Lookup(component);
            if (symbol && symbol->IsNamespaceSymbol())
            {
                ns = static_cast<NamespaceSymbol*>(symbol);
            }
            else
            {
                throw std::runtime_error("namespace symbol expected");
            }
        }
        if (ns)
        {
            namespaces.push_back(ns);
        }
    }
    return namespaces;
}

void GenerateTreeDoc(SymbolTable& symbolTable, const gendoc::input::Tree& tree, const std::string& targetDir, const std::string& contentDir, 
    const std::string& styleDirName, const std::string& styleFileName, bool verbose)
{
    std::string treeHtmlFilePath = GetFullPath(Path::Combine(contentDir, tree.outputFile));
    std::string xmlFilePath = GetFullPath(Path::Combine(targetDir, tree.xmlFilePath));
    if (verbose)
    {
        std::cout << "reading " << xmlFilePath << "..." << std::endl;
    }
    std::unique_ptr<dom::Document> treeXmlDoc = dom::ReadDocument(xmlFilePath);
    dom::Document treeHtmlDoc;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(tree.title)));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(Path::Combine(Path::Combine("..", styleDirName), styleFileName));
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    std::unique_ptr<dom::Element> scriptElement(new dom::Element(U"script"));
    scriptElement->SetAttribute(U"type", U"text/javascript");
    scriptElement->SetAttribute(U"src", ToUtf32(Path::GetFileNameWithoutExtension(treeHtmlFilePath) + ".js"));
    scriptElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
    headElement->AppendChild(std::unique_ptr<dom::Node>(scriptElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    bodyElement->SetAttribute(U"onload", U"drawTree()");
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(tree.title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    std::unique_ptr<dom::Element> divElement(new dom::Element(U"div"));
    divElement->SetAttribute(U"class", U"tree");
    std::unique_ptr<dom::Element> svgElement(new dom::Element(U"svg"));
    svgElement->SetAttribute(U"width", U"0");
    svgElement->SetAttribute(U"height", U"0");
    svgElement->SetAttribute(U"id", U"treeDiagram");
    svgElement->SetAttribute(U"xmlns", U"http://www.w3.org/2000/svg");
    svgElement->SetAttribute(U"version", U"2.0");
    divElement->AppendChild(std::unique_ptr<dom::Node>(svgElement.release()));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(divElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    std::ofstream treeHtmlFile(treeHtmlFilePath);
    CodeFormatter formatter(treeHtmlFile);
    formatter.SetIndentSize(1);
    treeHtmlDoc.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    treeHtmlDoc.Write(formatter);
    if (verbose)
    {
        std::cout << "==> " << treeHtmlFilePath << "..." << std::endl;
    }
    std::vector<TreeNode> treeNodes;
    std::unordered_set<Symbol*> usedTypes;
    std::unordered_set<std::u32string> visited;
    GenerateTreeNodes(U"", treeXmlDoc.get(), GetNamespaces(symbolTable, tree.ns), treeNodes, treeXmlDoc->DocumentElement(), 0, true, visited);
    std::string treeJsFilePath = GetFullPath(Path::Combine(contentDir, Path::GetFileNameWithoutExtension(treeHtmlFilePath) + ".js"));
    std::ofstream treeJsFile(treeJsFilePath);
    CodeFormatter treeJsFormatter(treeJsFile);
    treeJsFormatter.SetIndentSize(1);
    GenerateTreeCode(treeJsFormatter);
    treeJsFormatter.WriteLine("function drawTree() {");
    treeJsFormatter.IncIndent();
    treeJsFormatter.WriteLine("var nodes = [");
    treeJsFormatter.IncIndent();
    bool first = true;
    for (TreeNode& treeNode : treeNodes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            treeJsFormatter.WriteLine(",");
        }
        treeNode.Write(treeJsFormatter);
    }
    treeJsFormatter.WriteLine("];");
    treeJsFormatter.DecIndent();
    treeJsFormatter.WriteLine("drawTreeNodes(nodes);");
    treeJsFormatter.DecIndent();
    treeJsFormatter.WriteLine("}");
    treeJsFormatter.WriteLine();
    if (verbose)
    {
        std::cout << "==> " << treeJsFilePath << "..." << std::endl;
    }
}

void GenerateTreeDocs(SymbolTable& symbolTable, const std::vector<gendoc::input::Tree>& trees, const std::string& targetDir, const std::string& contentDir,
    const std::string& styleDirName, const std::string& styleFileName, bool verbose)
{
    for (const gendoc::input::Tree& tree : trees)
    {
        GenerateTreeDoc(symbolTable, tree, targetDir, contentDir, styleDirName, styleFileName, verbose);
    }
}

} } // namespace gendoc::html
