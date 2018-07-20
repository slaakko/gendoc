// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/gendoc/Input.hpp>
#include <gendoc/gendoc/Solution.hpp>
#include <gendoc/gendoc/Content.hpp>
#include <gendoc/gendoc/Html.hpp>
#include <gendoc/gendoc/HtmlTreeDoc.hpp>
#include <gendoc/parsing/InitDone.hpp>
#include <gendoc/parsing/Exception.hpp>
#include <gendoc/xpath/InitDone.hpp>
#include <gendoc/dom/Document.hpp>
#include <gendoc/util/InitDone.hpp>
#include <gendoc/util/Path.hpp>
#include <boost/filesystem.hpp>
#include <stdexcept>
#include <iostream>

using namespace gendoc::parsing;
using namespace gendoc::util;

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

const int inlineCodeLimit = 10;

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        bool verbose = false;
        bool force = false;
        bool prevWasPhase = false;
        std::string phase;
        std::string inputXmlFilePath;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg[0] == '-')
            {
                if (arg == "--verbose" || arg == "-v")
                {
                    verbose = true;
                }
                else if (arg == "--force" || arg == "-F")
                {
                    force = true;
                }
                else if (arg == "--phase" || arg == "-p")
                {
                    prevWasPhase = true;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (prevWasPhase)
            {
                phase = arg;
                prevWasPhase = false;
            }
            else
            {
                if (!inputXmlFilePath.empty())
                {
                    throw std::runtime_error("input xml file path already set");
                }
                inputXmlFilePath = GetFullPath(arg);
            }
        }
        if (verbose)
        {
            std::cout << "reading '" << inputXmlFilePath << "'..." << std::endl;
        }
        gendoc::cppsym::SymbolTable symbolTable;
        std::unique_ptr<gendoc::parsing::ParsingDomain> parsingDomain(new gendoc::parsing::ParsingDomain());
        parsingDomain->SetOwned();
        parsingDomain->SetProjectPrefix(U"cmajor");
        std::vector<std::unique_ptr<gendoc::syntax::ParserFileContent>> parserFileContents;
        std::unordered_map<int, std::u32string> parserProjectMap;
        std::vector<std::unique_ptr<gendoc::cppast::SourceFileNode>> parserSourceFileNodes;
        std::unordered_map<std::u32string, std::unique_ptr<gendoc::dom::Element>> inlineCodeMap;
        std::unique_ptr<gendoc::dom::Document> inputXml = gendoc::input::ReadInputXml(inputXmlFilePath);
        std::string solutionFilePath = gendoc::input::GetSolutionFilePath(inputXml.get());
        std::string solutionXmlFilePath = gendoc::input::GetSolutionXmlFilePath(inputXml.get());
        std::unique_ptr<gendoc::dom::Document> solutionXml;
        if (!boost::filesystem::exists(solutionXmlFilePath) || force || phase == "solution")
        {
            if (verbose)
            {
                std::cout << "generating '" << solutionXmlFilePath << "'..." << std::endl;
            }
            solutionXml = gendoc::solution::GenerateSolutionXml(solutionFilePath, solutionXmlFilePath, verbose);
        }
        else
        {
            if (verbose)
            {
                std::cout << "reading '" << solutionXmlFilePath << "'..." << std::endl;
            }
            solutionXml = gendoc::solution::ReadSolutionXml(solutionXmlFilePath);
        }
        std::string contentXmlFilePath = gendoc::input::GetContentXmlFilePath(inputXml.get());
        std::unique_ptr<gendoc::dom::Document> contentXml;
        std::vector<std::unique_ptr<gendoc::cppast::Node>> asts;
        std::vector<gendoc::cppast::SourceFileNode*> sourceFiles;
        if (!boost::filesystem::exists(contentXmlFilePath) || force || phase == "content" || phase == "solution")
        {
            if (verbose)
            {
                std::cout << "generating '" << contentXmlFilePath << "'..." << std::endl;
            }
            contentXml = gendoc::content::GenerateContentXml(inputXml.get(), solutionXml.get(), contentXmlFilePath, verbose, symbolTable, asts, sourceFiles, 
                parsingDomain.get(), parserFileContents, parserProjectMap);
        }
        else
        {
            if (verbose)
            {
                std::cout << "reading '" << contentXmlFilePath << "'..." << std::endl;
            }
            contentXml = gendoc::content::ReadContentXml(contentXmlFilePath);
        }
        std::string targetDir = gendoc::input::GetTargetDir(inputXml.get());
        std::string htmlDir;
        std::string contentDir;
        std::string styleDir;
        gendoc::html::MakeDirectories(targetDir, htmlDir, contentDir, styleDir, sourceFiles);
        if (!boost::filesystem::exists(contentXmlFilePath) || force || phase == "html" || phase == "content" || phase == "solution")
        {
            if (verbose)
            {
                std::cout << "generating html..." << std::endl;
            }
            std::string styleDirName;
            std::string styleFileName;
            gendoc::html::GenerateStyleSheet(styleDir, styleDirName, styleFileName);
            if (verbose)
            {
                std::cout << "generating html for C++ source files..." << std::endl;
            }
            gendoc::html::GenerateHtmlCodeFiles(sourceFiles, styleDirName, styleFileName, symbolTable, inlineCodeLimit, inlineCodeMap);
            if (verbose)
            {
                std::cout << "generating html reference docs..." << std::endl;
            }
            std::vector<gendoc::dom::Element*> componentElements;
            std::unordered_map<gendoc::dom::Element*, std::string> componentElementFileNameMap;
            gendoc::html::GenerateIndexHtml(inputXml.get(), htmlDir, styleDirName, styleFileName, componentElements, componentElementFileNameMap);
            gendoc::html::GenerateComponentHtml(inputXml.get(), contentXml.get(), contentDir, styleDirName, styleFileName, componentElements, componentElementFileNameMap,
                sourceFiles, parserFileContents, parserProjectMap, parserSourceFileNodes);
            gendoc::html::GenerateContent(contentXml.get(), contentDir, styleDirName, styleFileName, inlineCodeMap);
            if (verbose)
            {
                std::cout << "generating html for parser source files..." << std::endl;
            }
            std::vector<gendoc::cppast::SourceFileNode*> parserSourceFiles;
            for (const std::unique_ptr<gendoc::cppast::SourceFileNode>& parserSourceFile : parserSourceFileNodes)
            {
                parserSourceFiles.push_back(parserSourceFile.get());
            }
            gendoc::html::MakeDirectories(targetDir, htmlDir, contentDir, styleDir, parserSourceFiles);
            gendoc::html::GenerateHtmlParserFiles(parserSourceFiles, styleDirName, styleFileName);
            std::vector<gendoc::input::Tree> trees = gendoc::input::GetTreeData(inputXml.get());
            gendoc::html::GenerateTreeDocs(symbolTable, trees, targetDir, contentDir, styleDirName, styleFileName, verbose);
            if (verbose)
            {
                std::cout << "html generated." << std::endl;
            }
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