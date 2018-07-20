// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

// ==========================================================
// Interface to a non-validating XML parser with SAX-like API
// Author: S. Laakko
// ==========================================================

#ifndef CMINOR_XML_XML_PARSER
#define CMINOR_XML_XML_PARSER
#include <gendoc/xml/XmlContentHandler.hpp>

namespace gendoc { namespace xml {

//  ==================================================================================
//  ParseXmlFile parses given UTF-8 encoded XML file using given content handler.
//  ==================================================================================

void ParseXmlFile(const std::string& xmlFileName, XmlContentHandler* contentHandler);

//  ==================================================================================
//  ParseXmlContent parses given UTF-8 encoded XML string using given content handler.
//  systemId is used for error messages only. It can be for example a file name or URL 
//  that identifies the XML string to parse.
//  ===================================================================================

void ParseXmlContent(const std::string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler);

//  ===================================================================================
//  ParseXmlContent parses given UTF-32 encoded XML string using given content handler.
//  systemId is used for error messages only. It can be for example a file name or URL 
//  that identifies the XML string to parse.
//  ===================================================================================

void ParseXmlContent(const std::u32string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler);

} } // namespace gendoc::xml

#endif // CMINOR_XML_XML_PARSER
