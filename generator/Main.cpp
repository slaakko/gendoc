// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/syntax/Generator.hpp>
#include <gendoc/syntax/CodeGenerator.hpp>
#include <gendoc/parsing/InitDone.hpp>
#include <gendoc/util/TextUtils.hpp>
#include <gendoc/util/Unicode.hpp>
#include <gendoc/util/InitDone.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>

#if defined(_MSC_VER) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

class InitDone
{
public:
    InitDone()
    {
        gendoc::parsing::Init();
        gendoc::util::Init();
    }
    ~InitDone()
    {
        gendoc::util::Done();
        gendoc::parsing::Done();
    }
};

using gendoc::util::Split;

void AddToLibraryDirectories(const std::string& libraryPath, std::vector<std::string>& libraryDirectories)
{
    std::vector<std::string> dirs = Split(libraryPath, ';');
    int n = int(dirs.size());
    for (int i = 0; i < n; ++i)
    {
        std::string dir = dirs[i];
        libraryDirectories.push_back(dir);
    }
}

const char* version = "3.0";

int main(int argc, const char** argv)
{
#if defined(_MSC_VER) && !defined(NDEBUG)
    int dbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(dbgFlags);
    //_CrtSetBreakAlloc(40183);
#endif // defined(_MSC_VER) && !defined(NDEBUG)
    try
    {
        std::cout << "GenDoc Parser Generator version " << version << std::endl;
        if (argc < 2)
        {
            std::cout << "Usage: cmpg [options] {<file>.pp}\n" << "Compile GenDoc parsing project file(s) <file>.pp..." << std::endl;
            std::cout << "options:\n";
            std::cout << "-L <dir1>;<dir2>;...: add <dir1>, <dir2>, ... to library reference directories" << std::endl;
            std::cout << "-F                  : force code generation" << std::endl;
            return 1;
        }
        std::vector<std::string> projectFilePaths;
        std::vector<std::string> libraryDirectories;
        std::string gendoc_root;
        const char* gendoc_root_env = getenv("GENDOC_ROOT");
        if (gendoc_root_env)
        {
            gendoc_root = gendoc_root_env;
        }
        if (gendoc_root.empty())
        {
            std::cerr << "please set 'GENDOC_ROOT' environment variable to contain /path/to/gendoc directory." << std::endl;
            return 2;
        }
        std::string libraryPath = (boost::filesystem::path(gendoc_root) / boost::filesystem::path("parsing")).generic_string();
        AddToLibraryDirectories(libraryPath, libraryDirectories);
        bool prevWasL = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg == "-L")
            {
                prevWasL = true;
            }
            else if (arg == "-F")
            {
                gendoc::syntax::SetForceCodeGen(true);
            }
            else if (prevWasL)
            {
                prevWasL = false;
                AddToLibraryDirectories(arg, libraryDirectories);
            }
            else
            {
                projectFilePaths.push_back(arg);
            }
        }
        InitDone initDone;
        int n = int(projectFilePaths.size());
        for (int i = 0; i < n; ++i)
        {
            const std::string& projectFilePath = projectFilePaths[i];
            gendoc::syntax::Generate(projectFilePath, libraryDirectories);
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 3;
    }
    catch (...)
    {
        std::cerr << "unknown exception" << std::endl;
        return 4;
    }
    return 0;
}
