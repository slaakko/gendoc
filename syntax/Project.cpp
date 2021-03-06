// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/syntax/Project.hpp>

namespace gendoc { namespace syntax {

using namespace boost::filesystem;

Project::Project(const std::string& name_, const std::string& filePath_): name(name_), filePath(filePath_), base(filePath)
{
    base.remove_filename();
}

std::string Project::BasePath() const
{
    return base.generic_string();
}

void Project::AddSourceFile(const std::string& sourceFilePath)
{
    path sfp(sourceFilePath);
    path sfpa = absolute(sfp, base);
    sourceFiles.push_back(sfpa.generic_string());
    relativeSourceFiles.push_back(sfp.generic_string());
}

void Project::AddReferenceFile(const std::string& referenceFilePath)
{
    referenceFiles.push_back(referenceFilePath);
    relativeReferenceFiles.push_back(referenceFilePath);
}

} } // namespace gendoc::syntax
