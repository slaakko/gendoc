// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_SYNTAX_CODEGENERATOR_INCLUDED
#define GENDOC_SYNTAX_CODEGENERATOR_INCLUDED

#include <gendoc/syntax/ParserFileContent.hpp>

namespace gendoc { namespace syntax {

void SetForceCodeGen(bool force);
void GenerateCode(const std::vector<std::unique_ptr<ParserFileContent>>& parserFiles);

} } // namespace gendoc::syntax

#endif // GENDOC_SYNTAX_CODEGENERATOR_INCLUDED
