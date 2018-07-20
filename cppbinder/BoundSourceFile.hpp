// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPBINDER_BOUND_SOURCE_FILE_INCLUDED
#define GENDOC_CPPBINDER_BOUND_SOURCE_FILE_INCLUDED
#include <gendoc/cppsym/Scope.hpp>
#include <gendoc/cppast/SourceFile.hpp>

namespace gendoc { namespace cppbinder {

using namespace gendoc::cppsym;

class BoundSourceFile
{
public:
    BoundSourceFile(SourceFileNode& sourceFileNode_);
    ~BoundSourceFile();
    void AddFileScope();
    std::vector<std::unique_ptr<FileScope>>& FileScopes() { return fileScopes; }
private:
    SourceFileNode & sourceFileNode;
    std::vector<std::unique_ptr<FileScope>> fileScopes;
};

} } // namespace gendoc::cppbinder

#endif // GENDOC_CPPBINDER_BOUND_SOURCE_FILE_INCLUDED
