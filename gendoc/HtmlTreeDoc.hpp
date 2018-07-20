// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_HTML_TREE_DOC_INCLUDED
#define GENDOC_HTML_TREE_DOC_INCLUDED
#include <gendoc/cppsym/SymbolTable.hpp>
#include <gendoc/gendoc/Input.hpp>

namespace gendoc { namespace html {

using namespace gendoc::cppsym;

void GenerateTreeDocs(SymbolTable& symbolTable, const std::vector<gendoc::input::Tree>& trees, const std::string& targetDir, const std::string& contentDir, 
    const std::string& styleDirName, const std::string& styleFileName, bool verbose);

} } // namespace gendoc::html

#endif // GENDOC_HTML_TREE_DOC_INCLUDED
