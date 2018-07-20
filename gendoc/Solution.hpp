// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_SOLUTION_INCLUDED
#define GENDOC_SOLUTION_INCLUDED
#include <gendoc/dom/Document.hpp>

namespace gendoc { namespace solution {

std::unique_ptr<gendoc::dom::Document> GenerateSolutionXml(const std::string& solutionFilePath, const std::string& solutionXmlFilePath, bool verbose);
std::unique_ptr<gendoc::dom::Document> ReadSolutionXml(const std::string& solutionXmlFilePath);

} } // namespace gendoc::solution

#endif // GENDOC_SOLUTION_INCLUDED
