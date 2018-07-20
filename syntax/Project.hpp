// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_SYNTAX_PROJECT_INCLUDED
#define GENDOC_SYNTAX_PROJECT_INCLUDED
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <memory>

namespace gendoc { namespace syntax {

class Project
{
public:
    Project(const std::string& name_, const std::string& filePath_);
    const std::string& Name() const { return name; }
    const std::string& FilePath() const { return filePath; }
    std::string BasePath() const;
    void AddSourceFile(const std::string& sourceFilePath);
    void AddReferenceFile(const std::string& referenceFilePath);
    const std::vector<std::string>& ReferenceFiles() const { return referenceFiles; }
    const std::vector<std::string>& RelativeReferenceFiles() const { return relativeReferenceFiles; }
    const std::vector<std::string>& SourceFiles() const { return sourceFiles; }
    const std::vector<std::string>& RelativeSourceFiles() const { return relativeSourceFiles; }
private:
    std::string name;
    std::string filePath;
    std::vector<std::string> sourceFiles;
    std::vector<std::string> relativeSourceFiles;
    std::vector<std::string> referenceFiles;
    std::vector<std::string> relativeReferenceFiles;
    boost::filesystem::path base;
};

} } // namespace gendoc::syntax

#endif // GENDOC_SYNTAX_PROJECT_INCLUDED
