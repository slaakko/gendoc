// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/gendoc/Solution.hpp>
#include <gendoc/cppparser/SolutionGrammar.hpp>
#include <gendoc/syntax/ProjectFile.hpp>
#include <gendoc/dom/Element.hpp>
#include <gendoc/dom/Parser.hpp>
#include <gendoc/dom/CharacterData.hpp>
#include <gendoc/xpath/XPathEvaluate.hpp>
#include <gendoc/util/MappedInputFile.hpp>
#include <gendoc/util/Unicode.hpp>
#include <gendoc/util/Path.hpp>
#include <algorithm>
#include <iostream>
#include <fstream>

namespace gendoc { namespace solution {

using namespace gendoc::cppparser;
using namespace gendoc::syntax;
using namespace gendoc::util;
using namespace gendoc::unicode;

struct ByFilePath
{
    bool operator()(const std::pair<std::string, std::string>& left, const std::pair<std::string, std::string>& right) const
    {
        return left.first < right.first;
    }
};

ProjectFile* cmpgProjectFileGrammar = nullptr;

void ReadCmpgPRoject(const std::string& ppFilePath, std::vector<std::pair<std::string, std::string>>& parserFiles, std::vector<std::pair<std::string, std::string>>& referenceFiles)
{
    if (!cmpgProjectFileGrammar)
    {
        cmpgProjectFileGrammar = ProjectFile::Create();
    }
    MappedInputFile ppFile(ppFilePath);
    std::u32string ppText = ToUtf32(std::string(ppFile.Begin(), ppFile.End()));
    std::unique_ptr<gendoc::syntax::Project> cmpgProject(cmpgProjectFileGrammar->Parse(ppText.c_str(), ppText.c_str() + ppText.length(), 0, ppFilePath));
    for (const std::string& parserFileName : cmpgProject->RelativeSourceFiles())
    {
        std::string parserFilePath = Path::Combine(cmpgProject->BasePath(), parserFileName);
        std::pair<std::string, std::string> parserFile(parserFilePath, parserFileName);
        if (std::find(parserFiles.begin(), parserFiles.end(), parserFile) == parserFiles.end())
        {
            parserFiles.push_back(parserFile);
        }
    }
    for (const std::string& referenceFileName : cmpgProject->RelativeReferenceFiles())
    {
        std::string referenceFilePath = Path::Combine(cmpgProject->BasePath(), referenceFileName);
        std::pair<std::string, std::string> referenceFile(referenceFilePath, referenceFileName);
        if (std::find(referenceFiles.begin(), referenceFiles.end(), referenceFile) == referenceFiles.end())
        {
            referenceFiles.push_back(referenceFile);
        }
    }
}

gendoc::cppparser::Solution* solutionGrammar = nullptr;

std::unique_ptr<dom::Document> GenerateSolutionXml(const std::string& solutionFilePath, const std::string& solutionXmlFilePath, bool verbose)
{
    if (!solutionGrammar)
    {
        solutionGrammar = gendoc::cppparser::Solution::Create();
    }
    MappedInputFile solutionFile(solutionFilePath);
    std::u32string solutionText = ToUtf32(std::string(solutionFile.Begin(), solutionFile.End()));
    std::unique_ptr<gendoc::cppast::Solution> solution(solutionGrammar->Parse(solutionText.c_str(), solutionText.c_str() + solutionText.length(), 0, solutionFilePath));
    std::unique_ptr<dom::Document> solutionXml(new dom::Document());
    std::unique_ptr<dom::Element> solutionElement(new dom::Element(U"solution"));
    std::unique_ptr<dom::Element> solutionNameElement(new dom::Element(U"name"));
    solutionNameElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(ToUtf32(Path::GetFileNameWithoutExtension(Path::MakeCanonical(solutionFilePath))))));
    solutionElement->AppendChild(std::unique_ptr<dom::Node>(solutionNameElement.release()));
    std::vector<gendoc::cppast::Project*> projects = solution->BuildOrder();
    for (gendoc::cppast::Project* project : projects)
    {
        if (Path::GetExtension(project->FilePath()) == ".vcxproj")
        {
            std::string projectFilePath = GetFullPath(Path::Combine(solution->BasePath(), project->FilePath()));
            std::string projectBasePath = Path::GetDirectoryName(projectFilePath);
            std::unique_ptr<dom::Document> projectDoc = dom::ReadDocument(projectFilePath);
            std::unique_ptr<dom::Element> projectElement(new dom::Element(U"project"));
            projectElement->SetAttribute(U"id", project->Guid());
            projectElement->SetAttribute(U"name", project->Name());
            std::unique_ptr<dom::Element> projectReferencesElement(new dom::Element(U"references"));
            for (const std::u32string& projectRefId : project->Dependencies())
            {
                std::unique_ptr<dom::Element> projectReferenceElement(new dom::Element(U"reference"));
                projectReferenceElement->SetAttribute(U"id", projectRefId);
                projectReferencesElement->AppendChild(std::unique_ptr<dom::Node>(projectReferenceElement.release()));
            }
            projectElement->AppendChild(std::unique_ptr<dom::Node>(projectReferencesElement.release()));
            std::unique_ptr<dom::Element> sourcesElement(new dom::Element(U"sources"));
            std::unique_ptr<dom::Element> parserSourcesElement(new dom::Element(U"parserSources"));
            std::unique_ptr<xpath::XPathObject> none(xpath::Evaluate(U"/Project/ItemGroup/None", projectDoc.get())); 
            std::vector<std::pair<std::string, std::string>> ppFiles;
            if (none->Type() == xpath::XPathObjectType::nodeSet)
            {
                xpath::XPathNodeSet* noneNodeSet = static_cast<xpath::XPathNodeSet*>(none.get());
                int n = noneNodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* node = (*noneNodeSet)[i];
                    if (node->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* element = static_cast<dom::Element*>(node);
                        std::u32string ppFileName = element->GetAttribute(U"Include");
                        if (!ppFileName.empty() && Path::GetExtension(ToUtf8(ppFileName)) == ".pp")
                        {
                            std::string ppFilePath = GetFullPath(Path::Combine(projectBasePath, ToUtf8(ppFileName)));
                            ppFiles.push_back(std::make_pair(ppFilePath, ToUtf8(ppFileName)));
                        }
                    }
                }
            }
            std::vector<std::pair<std::string, std::string>> parserFiles;
            std::vector<std::pair<std::string, std::string>> referenceFiles;
            int n = ppFiles.size();
            for (int i = 0; i < n; ++i)
            {
                const std::string& ppFilePath = ppFiles[i].first;
                ReadCmpgPRoject(ppFilePath, parserFiles, referenceFiles);
            }
            std::unique_ptr<xpath::XPathObject> clInclude(xpath::Evaluate(U"/Project/ItemGroup/ClInclude", projectDoc.get()));
            std::vector<std::pair<std::string, std::string>> hppFiles;
            if (clInclude->Type() == xpath::XPathObjectType::nodeSet)
            {
                xpath::XPathNodeSet* clIncludeNodeSet = static_cast<xpath::XPathNodeSet*>(clInclude.get());
                int n = clIncludeNodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* node = (*clIncludeNodeSet)[i];
                    if (node->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* element = static_cast<dom::Element*>(node);
                        std::u32string hppFileName = element->GetAttribute(U"Include");
                        if (!hppFileName.empty() && Path::GetExtension(ToUtf8(hppFileName)) == ".hpp")
                        {
                            std::string hppFilePath = GetFullPath(Path::Combine(projectBasePath, ToUtf8(hppFileName)));
                            std::string baseName = Path::GetFileNameWithoutExtension(ToUtf8(hppFileName));
                            bool parserFileExists = false;
                            for (const std::pair<std::string, std::string>& parserFile : parserFiles)
                            {
                                const std::string& parserFileName = parserFile.second;
                                if (baseName == Path::GetFileNameWithoutExtension(parserFileName))
                                {
                                    parserFileExists = true;
                                    break; 
                                }
                            }
                            if (!parserFileExists)
                            {
                                hppFiles.push_back(std::make_pair(hppFilePath, ToUtf8(hppFileName)));
                            }
                        }
                    }
                }
            }
            std::sort(hppFiles.begin(), hppFiles.end(), ByFilePath());
            std::unique_ptr<xpath::XPathObject> clCompile(xpath::Evaluate(U"/Project/ItemGroup/ClCompile", projectDoc.get()));
            std::vector<std::pair<std::string, std::string>> cppFiles;
            if (clCompile->Type() == xpath::XPathObjectType::nodeSet)
            {
                xpath::XPathNodeSet* clCompileNodeSet = static_cast<xpath::XPathNodeSet*>(clCompile.get());
                int n = clCompileNodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* node = (*clCompileNodeSet)[i];
                    if (node->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* element = static_cast<dom::Element*>(node);
                        std::u32string cppFileName = element->GetAttribute(U"Include");
                        if (!cppFileName.empty() && Path::GetExtension(ToUtf8(cppFileName)) == ".cpp")
                        {
                            std::string cppFilePath = GetFullPath(Path::Combine(projectBasePath, ToUtf8(cppFileName)));
                            std::string baseName = Path::GetFileNameWithoutExtension(ToUtf8(cppFileName));
                            bool parserFileExists = false;
                            for (const std::pair<std::string, std::string>& parserFile : parserFiles)
                            {
                                const std::string& parserFileName = parserFile.second;
                                if (baseName == Path::GetFileNameWithoutExtension(parserFileName))
                                {
                                    parserFileExists = true;
                                    break;
                                }
                            }
                            if (!parserFileExists)
                            {
                                cppFiles.push_back(std::make_pair(cppFilePath, ToUtf8(cppFileName)));
                            }
                        }
                    }
                }
            }
            std::sort(cppFiles.begin(), cppFiles.end());
            for (const std::pair<std::string, std::string>& hppFile : hppFiles)
            {
                std::unique_ptr<dom::Element> sourceElement(new dom::Element(U"source"));
                sourceElement->SetAttribute(U"filePath", ToUtf32(hppFile.first));
                sourceElement->SetAttribute(U"relativeFilePath", ToUtf32(hppFile.second));
                sourcesElement->AppendChild(std::unique_ptr<dom::Node>(sourceElement.release()));
            }
            for (const std::pair<std::string, std::string>& cppFile : cppFiles)
            {
                std::unique_ptr<dom::Element> sourceElement(new dom::Element(U"source"));
                sourceElement->SetAttribute(U"filePath", ToUtf32(cppFile.first));
                sourceElement->SetAttribute(U"relativeFilePath", ToUtf32(cppFile.second));
                sourcesElement->AppendChild(std::unique_ptr<dom::Node>(sourceElement.release()));
            }
            for (const std::pair<std::string, std::string>& parserFile : parserFiles)
            {
                std::unique_ptr<dom::Element> sourceElement(new dom::Element(U"parserSource"));
                sourceElement->SetAttribute(U"filePath", ToUtf32(parserFile.first));
                sourceElement->SetAttribute(U"relativeFilePath", ToUtf32(parserFile.second));
                parserSourcesElement->AppendChild(std::unique_ptr<dom::Node>(sourceElement.release()));
            }
            projectElement->AppendChild(std::unique_ptr<dom::Node>(sourcesElement.release()));
            projectElement->AppendChild(std::unique_ptr<dom::Node>(parserSourcesElement.release()));
            solutionElement->AppendChild(std::unique_ptr<dom::Node>(projectElement.release()));
        }
    }
    solutionXml->AppendChild(std::unique_ptr<dom::Node>(solutionElement.release()));
    std::ofstream solutionXmlFile(solutionXmlFilePath);
    CodeFormatter formatter(solutionXmlFile);
    formatter.SetIndentSize(1);
    solutionXml->Write(formatter);
    if (verbose)
    {
        std::cout << "==> " << solutionXmlFilePath << std::endl;
    }
    return solutionXml;
}

std::unique_ptr<gendoc::dom::Document> ReadSolutionXml(const std::string& solutionXmlFilePath)
{
    return dom::ReadDocument(solutionXmlFilePath);
}

} } // namespace gendoc::solution
