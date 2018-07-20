// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_UTIL_MAPPED_INPUT_FILE_INCLUDED
#define GENDOC_UTIL_MAPPED_INPUT_FILE_INCLUDED
#include <string>
#include <stdint.h>

namespace gendoc { namespace util {

class MappedInputFileImpl;

class MappedInputFile
{
public:
    MappedInputFile(const std::string& fileName_);
    ~MappedInputFile();
    const char* Begin() const;
    const char* End() const;
private:
    MappedInputFileImpl* impl;
};

std::string ReadFile(const std::string& fileName);

} } // namespace gendoc::util

#endif // GENDOC_UTIL_MAPPED_INPUT_FILE_INCLUDED
