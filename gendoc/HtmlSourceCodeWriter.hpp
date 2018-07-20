// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_HTML_SOURCE_CODE_WRITER_INCLUDED
#define GENDOC_HTML_SOURCE_CODE_WRITER_INCLUDED
#include <gendoc/cppast/SourceCodeWriter.hpp>
#include <gendoc/cppast/Node.hpp>
#include <gendoc/dom/Element.hpp>

namespace gendoc { namespace html {

using namespace gendoc::cppast;

enum class WriteKind : uint8_t
{
    none = 0,
    line = 1 << 0,
    inlineLine = 1 << 1,
    both = line | inlineLine
};

inline WriteKind operator|(WriteKind left, WriteKind right)
{
    return WriteKind(uint8_t(left) | uint8_t(right));
}

inline WriteKind operator&(WriteKind left, WriteKind right)
{
    return WriteKind(uint8_t(left) & uint8_t(right));
}

class HtmlSourceCodeWriter : public gendoc::cppast::SourceCodeWriter
{
public:
    HtmlSourceCodeWriter();
    void SetLineElement(dom::Element* lineElement_) { lineElement = lineElement_; }
    void SetInlineLineElement(dom::Element* inlineLineElement_) { inlineLineElement = inlineLineElement_; }
    void SetCurrentPos(int currentPos_) { currentPos = currentPos_; }
    void SetInlineCurrentPos(int inlineCurrentPos_) { inlineCurrentPos = inlineCurrentPos_; }
    void MoveTo(const Span& span);
    void WriteLineNumber(const std::u32string& lineNumber) override;
    void WriteLineNumberKind(const std::u32string& lineNumber, WriteKind kind);
    void WriteComment(const std::u32string& comment) override;
    void WriteKeyword(const std::u32string& keyword) override;
    void WriteIdentifier(const std::u32string& identifier) override;
    void WriteLink(const std::u32string& identifier, const std::u32string& ref) override;
    void WriteInlineLink(const std::u32string& identifier, const std::u32string& ref);
    void WriteType(const std::u32string& type) override;
    void WritePP(const std::u32string& pp) override;
    void WriteString(const std::u32string& string) override;
    void WriteSpace(int numSpaces) override;
    void WriteSpaceKind(int numSpaces, WriteKind kind);
    void WriteOther(const std::u32string& other) override;
private:
    dom::Element* lineElement;
    dom::Element* inlineLineElement;
    int currentPos;
    int inlineCurrentPos;
    void WriteToElement(dom::Element* element, const std::u32string& text);
};

} } // namespace gendoc::html

#endif // GENDOC_HTML_SOURCE_CODE_WRITER_INCLUDED
