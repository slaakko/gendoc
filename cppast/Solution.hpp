// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_SOLUTION_INCLUDED
#define GENDOC_CPPAST_SOLUTION_INCLUDED
#include <stdint.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace gendoc { namespace cppast {

struct Version
{
    Version();
    Version(uint32_t major_, uint32_t minor_, uint32_t revision_, uint32_t build_);
    uint32_t major;
    uint32_t minor;
    uint32_t revision;
    uint32_t build;
};

class Project
{
public:
    Project(const std::u32string& languageGuid_, const std::u32string& name_, const std::string filePath_, std::u32string guid_);
    void AddDependency(const std::u32string& dependency);
    const std::u32string& LanguageGuid() const { return languageGuid; }
    const std::u32string& Name() const { return name; }
    const std::string& FilePath() const { return filePath; }
    const std::u32string& Guid() const { return guid; }
    const std::vector<std::u32string>& Dependencies() const { return dependencies; }
private:
    std::u32string languageGuid;
    std::u32string name;
    std::string filePath;
    std::u32string guid;
    std::vector<std::u32string> dependencies;
};

class Solution
{
public:
    Solution(const std::string& filePath_, const Version& formatVersion_, const Version& visualStudioVersion_, const Version& minimumVisualStudioVersion_);
    void AddProject(Project* project);
    Project* GetProject(const std::u32string& guid) const;
    std::vector<Project*> BuildOrder();
    const std::string& BasePath() const { return basePath; }
private:
    std::string filePath;
    std::string basePath;
    Version formatVersion;
    Version visualStudioVersion;
    Version minimumVisualStudioVersion;
    std::vector<std::unique_ptr<Project>> projects;
    std::unordered_map<std::u32string, Project*> projectMap;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_SOLUTION_INCLUDED
