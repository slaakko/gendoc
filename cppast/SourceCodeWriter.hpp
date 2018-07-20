// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_SOURCE_CODE_WRITER_INCLUDED
#define GENDOC_CPPAST_SOURCE_CODE_WRITER_INCLUDED
#include <string>

namespace gendoc { namespace cppast {

class SourceCodeWriter
{
public:
    virtual ~SourceCodeWriter();
    virtual void WriteLineNumber(const std::u32string& lineNumber) = 0;
    virtual void WriteComment(const std::u32string& comment) = 0;
    virtual void WriteKeyword(const std::u32string& keyword) = 0;
    virtual void WriteIdentifier(const std::u32string& identifier) = 0;
    virtual void WriteLink(const std::u32string& identifier, const std::u32string& ref) = 0;
    virtual void WritePP(const std::u32string& pp) = 0;
    virtual void WriteString(const std::u32string& string) = 0;
    virtual void WriteType(const std::u32string& type) = 0;
    virtual void WriteSpace(int numSpaces) = 0;
    virtual void WriteOther(const std::u32string& other) = 0;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_SOURCE_CODE_WRITER_INCLUDED
