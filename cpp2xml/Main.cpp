// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppbinder/TypeBinder.hpp>
#include <gendoc/cppparser/SolutionGrammar.hpp>
#include <gendoc/cppparser/SourceLines.hpp>
#include <gendoc/cppparser/CppSourceFileGrammar.hpp>
#include <gendoc/cppparser/IncludeDirectiveGrammar.hpp>
#include <gendoc/cppsym/SymbolTable.hpp>
#include <gendoc/cppsym/SymbolCreator.hpp>
#include <gendoc/xpath/InitDone.hpp>
#include <gendoc/xpath/XPathEvaluate.hpp>
#include <gendoc/parsing/InitDone.hpp>
#include <gendoc/parsing/Exception.hpp>
#include <gendoc/dom/Parser.hpp>
#include <gendoc/dom/Element.hpp>
#include <gendoc/dom/Document.hpp>
#include <gendoc/dom/CharacterData.hpp>
#include <gendoc/util/InitDone.hpp>
#include <gendoc/util/Unicode.hpp>
#include <gendoc/util/MappedInputFile.hpp>
#include <gendoc/util/Path.hpp>
#include <gendoc/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace gendoc::cppbinder;
using namespace gendoc::cppparser;
using namespace gendoc::cppsym;
using namespace gendoc::xpath;
using namespace gendoc::dom;
using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

struct InitDone
{
    InitDone()
    {
        gendoc::util::Init();
        gendoc::parsing::Init();
        gendoc::xpath::Init();
    }
    ~InitDone()
    {
        gendoc::xpath::Done();
        gendoc::parsing::Done();
        gendoc::util::Done();
    }
};

gendoc::cppparser::Solution* solutionGrammar = nullptr;

struct ByFilePath
{
    bool operator()(const std::pair<std::string, std::string>& left, const std::pair<std::string, std::string>& right) const
    {
        return left.first < right.first;
    }
};

void GenerateXmlSolution(const std::string& solutionFilePath, const std::string& xmlFilePath)
{
    if (!solutionGrammar)
    {
        solutionGrammar = gendoc::cppparser::Solution::Create();
    }
    MappedInputFile solutionFile(solutionFilePath);
    std::u32string solutionText = ToUtf32(std::string(solutionFile.Begin(), solutionFile.End()));
    std::unique_ptr<gendoc::cppast::Solution> solution(solutionGrammar->Parse(solutionText.c_str(), solutionText.c_str() + solutionText.length(), 0, solutionFilePath));
    Document solutionDoc;
    std::unique_ptr<Element> solutionElement(new Element(U"solution"));
    std::unique_ptr<Element> solutionNameElement(new Element(U"name"));
    solutionNameElement->AppendChild(std::unique_ptr<gendoc::dom::Node>(new Text(ToUtf32(Path::GetFileNameWithoutExtension(Path::MakeCanonical(solutionFilePath))))));
    solutionElement->AppendChild(std::unique_ptr<gendoc::dom::Node>(solutionNameElement.release()));
    std::vector<Project*> projects = solution->BuildOrder();
    for (Project* project : projects)
    {
        if (Path::GetExtension(project->FilePath()) == ".vcxproj")
        {
            std::string projectFilePath = GetFullPath(Path::Combine(solution->BasePath(), project->FilePath()));
            std::string projectBasePath = Path::GetDirectoryName(projectFilePath);
            std::unique_ptr<Document> projectDoc = ReadDocument(projectFilePath);
            std::unique_ptr<Element> projectElement(new Element(U"project"));
            projectElement->SetAttribute(U"id", project->Guid());
            projectElement->SetAttribute(U"name", project->Name());
            std::unique_ptr<Element> projectReferencesElement(new Element(U"references"));
            for (const std::u32string& projectRefId : project->Dependencies())
            {
                std::unique_ptr<Element> projectReferenceElement(new Element(U"reference"));
                projectReferenceElement->SetAttribute(U"id", projectRefId);
                projectReferencesElement->AppendChild(std::unique_ptr<gendoc::dom::Node>(projectReferenceElement.release()));
            }
            projectElement->AppendChild(std::unique_ptr<gendoc::dom::Node>(projectReferencesElement.release()));
            std::unique_ptr<Element> sourcesElement(new Element(U"sources"));
            std::unique_ptr<XPathObject> clInclude(Evaluate(U"/Project/ItemGroup/ClInclude", projectDoc.get()));
            std::vector<std::pair<std::string, std::string>> hppFiles;
            if (clInclude->Type() == XPathObjectType::nodeSet)
            {
                XPathNodeSet* clIncludeNodeSet = static_cast<XPathNodeSet*>(clInclude.get());
                int n = clIncludeNodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    gendoc::dom::Node* node = (*clIncludeNodeSet)[i];
                    if (node->GetNodeType() == NodeType::elementNode)
                    {
                        Element* element = static_cast<Element*>(node);
                        std::u32string hppFileName = element->GetAttribute(U"Include");
                        if (!hppFileName.empty() && Path::GetExtension(ToUtf8(hppFileName)) == ".hpp")
                        {
                            std::string hppFilePath = GetFullPath(Path::Combine(projectBasePath, ToUtf8(hppFileName)));
                            hppFiles.push_back(std::make_pair(hppFilePath, ToUtf8(hppFileName)));
                        }
                    }
                }
            }
            std::sort(hppFiles.begin(), hppFiles.end(), ByFilePath());
            std::unique_ptr<XPathObject> clCompile(Evaluate(U"/Project/ItemGroup/ClCompile", projectDoc.get()));
            std::vector<std::pair<std::string, std::string>> cppFiles;
            if (clCompile->Type() == XPathObjectType::nodeSet)
            {
                XPathNodeSet* clCompileNodeSet = static_cast<XPathNodeSet*>(clCompile.get());
                int n = clCompileNodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    gendoc::dom::Node* node = (*clCompileNodeSet)[i];
                    if (node->GetNodeType() == NodeType::elementNode)
                    {
                        Element* element = static_cast<Element*>(node);
                        std::u32string cppFileName = element->GetAttribute(U"Include");
                        if (!cppFileName.empty() && Path::GetExtension(ToUtf8(cppFileName)) == ".cpp")
                        {
                            std::string cppFilePath = GetFullPath(Path::Combine(projectBasePath, ToUtf8(cppFileName)));
                            cppFiles.push_back(std::make_pair(cppFilePath, ToUtf8(cppFileName)));
                        }
                    }
                }
            }
            std::sort(cppFiles.begin(), cppFiles.end());
            for (const std::pair<std::string, std::string>& hppFile : hppFiles)
            {
                std::unique_ptr<Element> sourceElement(new Element(U"source"));
                sourceElement->SetAttribute(U"filePath", ToUtf32(hppFile.first));
                sourceElement->SetAttribute(U"relativeFilePath", ToUtf32(hppFile.second));
                sourcesElement->AppendChild(std::unique_ptr<gendoc::dom::Node>(sourceElement.release()));
            }
            for (const std::pair<std::string, std::string>& cppFile : cppFiles)
            {
                std::unique_ptr<Element> sourceElement(new Element(U"source"));
                sourceElement->SetAttribute(U"filePath", ToUtf32(cppFile.first));
                sourceElement->SetAttribute(U"relativeFilePath", ToUtf32(cppFile.second));
                sourcesElement->AppendChild(std::unique_ptr<gendoc::dom::Node>(sourceElement.release()));
            }
            projectElement->AppendChild(std::unique_ptr<gendoc::dom::Node>(sourcesElement.release()));
            solutionElement->AppendChild(std::unique_ptr<gendoc::dom::Node>(projectElement.release()));
        }
    }
    solutionDoc.AppendChild(std::unique_ptr<gendoc::dom::Node>(solutionElement.release()));
    std::ofstream xmlFile(xmlFilePath);
    CodeFormatter formatter(xmlFile);
    formatter.SetIndentSize(1);
    solutionDoc.Write(formatter);
}

std::set<std::u32string> ReadFilter(const std::string& filterFilePath)
{
    std::set<std::u32string> filter;
    std::unique_ptr<Document> filterDoc = ReadDocument(filterFilePath);
    std::unique_ptr<XPathObject> projects = Evaluate(U"/filter/project", filterDoc.get());
    if (projects->Type() == XPathObjectType::nodeSet)
    {
        XPathNodeSet* projectNodeSet = static_cast<XPathNodeSet*>(projects.get());
        int n = projectNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            gendoc::dom::Node* node = (*projectNodeSet)[i];
            if (node->GetNodeType() == NodeType::elementNode)
            {
                Element* projectElement = static_cast<Element*>(node);
                std::u32string projectName = projectElement->GetAttribute(U"name");
                filter.insert(projectName);
            }
        }
    }
    return filter;
}

class IncludeDirectiveResolver : public gendoc::cppast::Visitor
{
public:
    IncludeDirectiveResolver(SourceFileNode* sourceFileNode_, const std::unordered_map<std::string, SourceFileNode*>& allSourceFiles_);
    void Visit(SourceFileNode& sourceFileNode) override;
    void Visit(IncludeDirectiveNode& includeDirectiveNode) override;
    void Visit(IncludeDirectiveSequenceNode& includeDirectiveSequenceNode) override;
private:
    SourceFileNode* sourceFileNode;
    const std::unordered_map<std::string, SourceFileNode*>& allSourceFiles;
};

IncludeDirectiveResolver::IncludeDirectiveResolver(SourceFileNode* sourceFileNode_, const std::unordered_map<std::string, SourceFileNode*>& allSourceFiles_) :
    sourceFileNode(sourceFileNode_), allSourceFiles(allSourceFiles_)
{
}

void IncludeDirectiveResolver::Visit(SourceFileNode& sourceFileNode)
{
    if (sourceFileNode.IncludeDirectives())
    {
        sourceFileNode.IncludeDirectives()->Accept(*this);
    }
}

void IncludeDirectiveResolver::Visit(IncludeDirectiveNode& includeDirectiveNode)
{
    for (const std::pair<std::string, SourceFileNode*>& sourceFilePathNode : allSourceFiles)
    {
        if (LastComponentsEqual(includeDirectiveNode.IncludeFilePath(), sourceFilePathNode.first, '/'))
        {
            includeDirectiveNode.SetSourceFileNode(sourceFilePathNode.second);
            break;
        }
    }
}

void IncludeDirectiveResolver::Visit(IncludeDirectiveSequenceNode& includeDirectiveSequenceNode)
{
    includeDirectiveSequenceNode.Left()->Accept(*this);
    includeDirectiveSequenceNode.Right()->Accept(*this);
}

void ResolveIncludeDirectives(SourceFileNode* sourceFileNode, const std::unordered_map<std::string, SourceFileNode*>& allSourceFiles)
{
    IncludeDirectiveResolver resolver(sourceFileNode, allSourceFiles);
    sourceFileNode->Accept(resolver);
}

CppSourceFile* cppSourceFileGrammar = nullptr;
IncludeDirective* includeDirectiveGrammar = nullptr;

std::unique_ptr<SourceFileNode> ParseSourceFile(const std::string& sourceFilePath, const std::string& relativeSourceFilePath, const std::u32string& projectName, bool debugParse, int& sourceFileIndex)
{
    if (!cppSourceFileGrammar)
    {
        cppSourceFileGrammar = CppSourceFile::Create();
    }
    if (!includeDirectiveGrammar)
    {
        includeDirectiveGrammar = IncludeDirective::Create();
    }
    if (debugParse)
    {
        cppSourceFileGrammar->SetLog(&std::cout);
        cppSourceFileGrammar->SetMaxLogLineLength(1024);
    }
    std::u32string sourceFileContent;
    if (boost::filesystem::file_size(sourceFilePath) > 0)
    {
        MappedInputFile sourceFile(sourceFilePath);
        sourceFileContent = ToUtf32(std::string(sourceFile.Begin(), sourceFile.End()));
    }
    std::unique_ptr<SourceFileNode> sourceFileNode(new SourceFileNode(Span(), sourceFilePath, relativeSourceFilePath, projectName));
    includeDirectiveGrammar->Parse(sourceFileContent.c_str(), sourceFileContent.c_str() + sourceFileContent.length(), sourceFileIndex, sourceFilePath, sourceFileNode.get());
    cppSourceFileGrammar->Parse(sourceFileContent.c_str(), sourceFileContent.c_str() + sourceFileContent.length(), sourceFileIndex, sourceFilePath, sourceFileNode.get());
    sourceFileNode->SetSourceFileIndex(sourceFileIndex);
    ++sourceFileIndex;
    return sourceFileNode;
}

std::unique_ptr<gendoc::cppast::Node> ParseSourceFiles(const std::vector<std::pair<std::string, std::string>>& sourceFilePaths,
    std::unordered_map<std::string, SourceFileNode*>& allSourceFiles, bool verbose, int& sourceFileIndex, const std::u32string& projectName)
{
    if (verbose)
    {
        std::cout << "Parsing sources..." << std::endl;
    }
    std::unique_ptr<gendoc::cppast::Node> node;
    std::vector<SourceFileNode*> sourceFileNodes;
    for (const std::pair<std::string, std::string>& sourceFilePath : sourceFilePaths)
    {
        if (verbose)
        {
            std::cout << "> " << sourceFilePath.first << std::endl;
        }
        std::unique_ptr<SourceFileNode> sourceFileNode = ParseSourceFile(sourceFilePath.first, sourceFilePath.second, projectName, false, sourceFileIndex);
        sourceFileNodes.push_back(sourceFileNode.get());
        if (node)
        {
            node.reset(new SourceFileSequenceNode(Span(), node.release(), sourceFileNode.release()));
        }
        else
        {
            node.reset(sourceFileNode.release());
        }
    }
    
    for (SourceFileNode* sourceFileNode : sourceFileNodes)
    {
        allSourceFiles[sourceFileNode->SourceFilePath()] = sourceFileNode;
    }
    for (SourceFileNode* sourceFileNode : sourceFileNodes)
    {
        ResolveIncludeDirectives(sourceFileNode, allSourceFiles);
    }
    return node;
}

SourceLines* sourceLinesGrammar = nullptr;

void CountLines(const std::vector<std::string>& sourceFilePaths, int& projectLineCount, int& totalLineCount)
{
    for (const std::string& sourceFilePath : sourceFilePaths)
    {
        MappedInputFile sourceFile(sourceFilePath);
        std::u32string sourceFileContent = ToUtf32(std::string(sourceFile.Begin(), sourceFile.End()));
        int lines = sourceLinesGrammar->Parse(sourceFileContent.c_str(), sourceFileContent.c_str() + sourceFileContent.length(), 0, sourceFilePath);
        projectLineCount += lines;
        totalLineCount += lines;
    }
}

void GenerateXml(const std::string& xmlFilePath, const std::set<std::u32string>& filter, bool verbose, const std::string& outputXmlFilePath)
{
    if (!sourceLinesGrammar)
    {
        sourceLinesGrammar = SourceLines::Create();
    }
    std::unique_ptr<Document> solutionDoc = ReadDocument(xmlFilePath);
    std::unique_ptr<XPathObject> projects = Evaluate(U"/solution/project", solutionDoc.get());
    SymbolTable symbolTable;
    std::unordered_map<std::string, SourceFileNode*> allSourceFiles;
    int sourceFileIndex = 0;
    int totalLineCount = 0;
    std::vector<std::unique_ptr<gendoc::cppast::Node>> asts;
    if (projects->Type() == XPathObjectType::nodeSet)
    {
        XPathNodeSet* projectNodeSet = static_cast<XPathNodeSet*>(projects.get());
        int n = projectNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            gendoc::dom::Node* node = (*projectNodeSet)[i];
            if (node->GetNodeType() == NodeType::elementNode)
            {
                int projectLineCount = 0;
                Element* projectElement = static_cast<Element*>(node);
                std::u32string projectName = projectElement->GetAttribute(U"name");
                if (!filter.empty())
                {
                    if (filter.find(projectName) == filter.cend()) continue;
                }
                std::unique_ptr<XPathObject> sources = Evaluate(U"sources/source", node);
                std::vector<std::pair<std::string, std::string>> sourceFilePaths;
                if (sources->Type() == XPathObjectType::nodeSet)
                {
                    XPathNodeSet* sourceNodeSet = static_cast<XPathNodeSet*>(sources.get());
                    int m = sourceNodeSet->Length();
                    for (int j = 0; j < m; ++j)
                    {
                        gendoc::dom::Node* sourceNode = (*sourceNodeSet)[j];
                        if (sourceNode->GetNodeType() == NodeType::elementNode)
                        {
                            Element* sourceElement = static_cast<Element*>(sourceNode);
                            std::string sourceFilePath = ToUtf8(sourceElement->GetAttribute(U"filePath"));
                            std::string relativeSourceFilePath = ToUtf8(sourceElement->GetAttribute(U"relativeSourceFilePath"));
                            sourceFilePaths.push_back(std::make_pair(sourceFilePath, relativeSourceFilePath));
                        }
                    }
                }
                std::unique_ptr<gendoc::cppast::Node> ast = ParseSourceFiles(sourceFilePaths, allSourceFiles, verbose, sourceFileIndex, projectName);
                SymbolCreator symbolCreator(symbolTable, projectName);
                ast->Accept(symbolCreator);
                TypeBinder typeBinder(symbolTable);
                ast->Accept(typeBinder);
                asts.push_back(std::move(ast));
            }
        }
    }
    if (!outputXmlFilePath.empty())
    {
        std::unique_ptr<gendoc::dom::Document> symbolTableDocument = symbolTable.ToDomDocument();
        std::ofstream outputXmlFile(outputXmlFilePath);
        CodeFormatter formatter(outputXmlFile);
        formatter.SetIndentSize(1);
        symbolTableDocument->Write(formatter);
        if (verbose)
        {
            std::cout << "==> " << outputXmlFilePath << std::endl;
        }
    }
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        bool prevWasO = false;
        bool prevWasF = false;
        bool debugParse = false;
        bool verbose = false;
        std::string solutionFilePath;
        std::string xmlFilePath;
        std::string outputXmlFilePath;
        std::string cppFilePath;
        std::string filterFilePath;
        int sourceFileIndex = 0;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!arg.empty() && arg[0] == '-')
            {
                if (arg == "--output" || arg == "-o")
                {
                    prevWasO = true;
                }
                else if (arg == "--filter" || arg == "-f")
                {
                    prevWasF = true;
                }
                else if (arg == "--debug-parse" || arg == "-p")
                {
                    debugParse = true;
                }
                else if (arg == "--verbose" || arg == "-v")
                {
                    verbose = true;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (prevWasO)
            {
                outputXmlFilePath = arg;
                prevWasO = false;
            }
            else if (prevWasF)
            {
                filterFilePath = arg;
                prevWasF = false;
            }
            else
            {
                if (Path::GetExtension(arg) == ".sln")
                {
                    solutionFilePath = arg;
                }
                else if (Path::GetExtension(arg) == ".xml")
                {
                    xmlFilePath = arg;
                }
                else if (Path::GetExtension(arg) == ".cpp" || Path::GetExtension(arg) == ".hpp")
                {
                    cppFilePath = arg;
                }
            }
        }
        if (!solutionFilePath.empty() && !outputXmlFilePath.empty())
        {
            GenerateXmlSolution(solutionFilePath, outputXmlFilePath);
        }
        else if (!xmlFilePath.empty())
        {
            std::set<std::u32string> filter;
            if (!filterFilePath.empty())
            {
                filter = ReadFilter(filterFilePath);
            }
            GenerateXml(xmlFilePath, filter, verbose, outputXmlFilePath);
        }
        else if (!cppFilePath.empty())
        {
            std::unique_ptr<SourceFileNode> sourceFileNode = ParseSourceFile(cppFilePath, cppFilePath, std::u32string(), debugParse, sourceFileIndex);
            SymbolTable symbolTable;
            SymbolCreator symbolCreator(symbolTable, std::u32string());
            sourceFileNode->Accept(symbolCreator);
            TypeBinder typeBinder(symbolTable);
            sourceFileNode->Accept(typeBinder);
        }
        else
        {
            throw std::runtime_error("solution file path or xml file path is empty");
        }
    }
    catch (const ParsingException& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
