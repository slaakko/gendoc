// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/parsing/XmlLog.hpp>
#include <gendoc/parsing/Utility.hpp>
#include <algorithm>

namespace gendoc { namespace parsing {

void XmlLog::WriteElement(const std::string& elementName, const std::string& elementContent)
{
    std::string converted = XmlEscape(elementContent);
    int convertedLength = int(converted.length());
    int lineLength = indent + 2 * int(elementName.length()) + 5 + convertedLength;
    std::string s = converted;
    if (lineLength > maxLineLength)
    {
        lineLength += 3;
        s = converted.substr(0, std::max(0, convertedLength - (lineLength - maxLineLength))) + "...";
    }
    Write("<" + elementName + ">" + s + "</" + elementName + ">");
}

void XmlLog::Write(const std::string& s)
{
    if (indent > 0)
    {
        stream << std::string(indent, ' ');
    }
    stream << s << std::endl;
}

} } // namespace gendoc::parsing
