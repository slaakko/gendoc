// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_SYNTAX_COMPILER_INCLUDED
#define GENDOC_SYNTAX_COMPILER_INCLUDED

#include <string>
#include <vector>

namespace gendoc { namespace syntax {

void Generate(const std::string& projectFilePath, const std::vector<std::string>& libraryDirectories);

} } // namespace gendoc::syntax

#endif // GENDOC_SYNTAX_COMPILER_INCLUDED

