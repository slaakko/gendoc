// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppbinder/BoundSourceFile.hpp>

namespace gendoc { namespace cppbinder {

BoundSourceFile::BoundSourceFile(SourceFileNode& sourceFileNode_) : sourceFileNode(sourceFileNode_)
{
}

BoundSourceFile::~BoundSourceFile()
{
    int x = 0;
}

void BoundSourceFile::AddFileScope()
{
    fileScopes.push_back(std::unique_ptr<FileScope>(new FileScope()));
}

} } // namespace gendoc::cppbinder
