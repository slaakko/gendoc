// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/gendoc/HtmlSourceCodeWriter.hpp>
#include <gendoc/dom/CharacterData.hpp>

namespace gendoc { namespace html {

HtmlSourceCodeWriter::HtmlSourceCodeWriter() : lineElement(nullptr), inlineLineElement(nullptr), currentPos(0), inlineCurrentPos(0)
{
}

void HtmlSourceCodeWriter::WriteToElement(dom::Element* element, const std::u32string& text)
{
    for (char32_t c : text)
    {
        if (c == ' ')
        {
            element->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        }
        else
        {
            element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(std::u32string(1, c))));
        }
    }
}

void HtmlSourceCodeWriter::MoveTo(const Span& span)
{
    if (currentPos < span.Start())
    {
        int length = span.Start() - currentPos;
        WriteSpaceKind(length, WriteKind::line);
    }
    if (inlineCurrentPos < span.Start())
    {
        int length = span.Start() - inlineCurrentPos;
        WriteSpaceKind(length, WriteKind::inlineLine);
    }
}

void HtmlSourceCodeWriter::WriteLineNumberKind(const std::u32string& lineNumber, WriteKind kind)
{
    std::unique_ptr<dom::Element> lineNumberElement(new dom::Element(U"span"));
    lineNumberElement->SetAttribute(U"class", U"lineNumber");
    WriteToElement(lineNumberElement.get(), lineNumber);
    if ((kind & WriteKind::inlineLine) != WriteKind::none)
    {
        if (inlineLineElement)
        {
            inlineLineElement->AppendChild(lineNumberElement->CloneNode(true));
            inlineCurrentPos += lineNumber.length();
        }
    }
    if ((kind & WriteKind::line) != WriteKind::none)
    {
        lineElement->AppendChild(std::unique_ptr<dom::Node>(lineNumberElement.release()));
        currentPos += lineNumber.length();
    }
}
void HtmlSourceCodeWriter::WriteLineNumber(const std::u32string& lineNumber)
{
    WriteLineNumberKind(lineNumber, WriteKind::both);
}

void HtmlSourceCodeWriter::WriteComment(const std::u32string& comment)
{
    std::unique_ptr<dom::Element> commentElement(new dom::Element(U"span"));
    commentElement->SetAttribute(U"class", U"comment");
    WriteToElement(commentElement.get(), comment);
    if (inlineLineElement)
    {
        inlineLineElement->AppendChild(commentElement->CloneNode(true));
        inlineCurrentPos += comment.length();
    }
    lineElement->AppendChild(std::unique_ptr<dom::Node>(commentElement.release()));
    currentPos += comment.length();
}

void HtmlSourceCodeWriter::WriteKeyword(const std::u32string& keyword)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"span"));
    keywordElement->SetAttribute(U"class", U"kw");
    WriteToElement(keywordElement.get(), keyword);
    if (inlineLineElement)
    {
        inlineLineElement->AppendChild(keywordElement->CloneNode(true));
        inlineCurrentPos += keyword.length();
    }
    lineElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentPos += keyword.length();
}

void HtmlSourceCodeWriter::WriteIdentifier(const std::u32string& identifier)
{
    std::unique_ptr<dom::Element> identifierElement(new dom::Element(U"span"));
    identifierElement->SetAttribute(U"class", U"identifier");
    WriteToElement(identifierElement.get(), identifier);
    if (inlineLineElement)
    {
        inlineLineElement->AppendChild(identifierElement->CloneNode(true));
        inlineCurrentPos += identifier.length();
    }
    lineElement->AppendChild(std::unique_ptr<dom::Node>(identifierElement.release()));
    currentPos += identifier.length();
}

void HtmlSourceCodeWriter::WriteLink(const std::u32string& identifier, const std::u32string& ref)
{
    if (ref.empty())
    {
        std::unique_ptr<dom::Element> identifierElement(new dom::Element(U"span"));
        identifierElement->SetAttribute(U"class", U"identifier");
        WriteToElement(identifierElement.get(), identifier);
        lineElement->AppendChild(std::unique_ptr<dom::Node>(identifierElement.release()));
        currentPos += identifier.length();
    }
    else
    {
        std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
        linkElement->SetAttribute(U"href", ref);
        WriteToElement(linkElement.get(), identifier);
        lineElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
        currentPos += identifier.length();
    }
}

void HtmlSourceCodeWriter::WriteInlineLink(const std::u32string& identifier, const std::u32string& ref)
{
    if (inlineLineElement)
    {
        if (ref.empty())
        {
            std::unique_ptr<dom::Element> identifierElement(new dom::Element(U"span"));
            identifierElement->SetAttribute(U"class", U"identifier");
            WriteToElement(identifierElement.get(), identifier);
            inlineLineElement->AppendChild(std::unique_ptr<dom::Node>(identifierElement.release()));
            inlineCurrentPos += identifier.length();
        }
        else
        {
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", ref);
            WriteToElement(linkElement.get(), identifier);
            inlineLineElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            inlineCurrentPos += identifier.length();
        }
    }
}

void HtmlSourceCodeWriter::WriteType(const std::u32string& type)
{
    std::unique_ptr<dom::Element> typeElement(new dom::Element(U"span"));
    typeElement->SetAttribute(U"class", U"type");
    WriteToElement(typeElement.get(), type);
    if (inlineLineElement)
    {
        inlineLineElement->AppendChild(typeElement->CloneNode(true));
        inlineCurrentPos += type.length();
    }
    lineElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    currentPos += type.length();
}

void HtmlSourceCodeWriter::WritePP(const std::u32string& pp)
{
    std::unique_ptr<dom::Element> ppElement(new dom::Element(U"span"));
    ppElement->SetAttribute(U"class", U"pp");
    WriteToElement(ppElement.get(), pp);
    if (inlineLineElement)
    {
        inlineLineElement->AppendChild(ppElement->CloneNode(true));
        inlineCurrentPos += pp.length();
    }
    lineElement->AppendChild(std::unique_ptr<dom::Node>(ppElement.release()));
    currentPos += pp.length();
}

void HtmlSourceCodeWriter::WriteString(const std::u32string& string)
{
    std::unique_ptr<dom::Element> stringElement(new dom::Element(U"span"));
    stringElement->SetAttribute(U"class", U"string");
    WriteToElement(stringElement.get(), string);
    if (inlineLineElement)
    {
        inlineLineElement->AppendChild(stringElement->CloneNode(true));
        inlineCurrentPos += string.length();
    }
    lineElement->AppendChild(std::unique_ptr<dom::Node>(stringElement.release()));
    currentPos += string.length();
}

void HtmlSourceCodeWriter::WriteSpaceKind(int numSpaces, WriteKind kind)
{
    if ((kind & WriteKind::line) != WriteKind::none)
    {
        for (int i = 0; i < numSpaces; ++i)
        {
            lineElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        }
        currentPos += numSpaces;
    }
    if (inlineLineElement)
    {
        if ((kind & WriteKind::inlineLine) != WriteKind::none)
        {
            for (int i = 0; i < numSpaces; ++i)
            {
                inlineLineElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
            }
            inlineCurrentPos += numSpaces;
        }
    }
}

void HtmlSourceCodeWriter::WriteSpace(int numSpaces)
{
    WriteSpaceKind(numSpaces, WriteKind::both);
}

void HtmlSourceCodeWriter::WriteOther(const std::u32string& other)
{
    std::unique_ptr<dom::Element> otherElement(new dom::Element(U"span"));
    otherElement->SetAttribute(U"class", U"other");
    WriteToElement(otherElement.get(), other);
    if (inlineLineElement)
    {
        inlineLineElement->AppendChild(otherElement->CloneNode(true));
        inlineCurrentPos += other.length();
    }
    lineElement->AppendChild(std::unique_ptr<dom::Node>(otherElement.release()));
    currentPos += other.length();
}

} } // namespace gendoc::html
