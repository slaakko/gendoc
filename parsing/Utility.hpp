// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_PARSING_UTILITY_INCLUDED
#define GENDOC_PARSING_UTILITY_INCLUDED
#include <stdint.h>
#include <string>
#include <vector>

namespace gendoc { namespace parsing {

std::string HexEscape(char c);
std::string XmlCharStr(char c);
std::string XmlEscape(const std::string& s);

} } // namespace gendoc::parsing

#endif // GENDOC_PARSING_UTILITY_INCLUDED
