// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_DOM_PARSER_INCLUDED
#define GENDOC_DOM_PARSER_INCLUDED
#include <gendoc/dom/Document.hpp>

namespace gendoc { namespace dom {

std::unique_ptr<Document> ParseDocument(const std::u32string& content, const std::string& systemId);
std::unique_ptr<Document> ReadDocument(const std::string& fileName);

} } // namespace gendoc::dom

#endif // GENDOC_DOM_PARSER_INCLUDED
