// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_XPATH_XPATH_DEBUG
#define GENDOC_XPATH_XPATH_DEBUG
#include <gendoc/dom/Node.hpp>
#include <chrono>

namespace gendoc { namespace xpath {

bool XPathDebugParsing();
void SetXPathDebugParsing();
bool XPathDebugQuery();
void SetXPathDebugQuery();
void SetXPathQueryDom(std::unique_ptr<dom::Node>&& queryDom);
std::unique_ptr<dom::Node> GetXPathQueryDom();
void SetXPathQueryDuration(std::chrono::nanoseconds duration);
std::chrono::nanoseconds XPathQueryDuration();
void SetXPathExecuteDuration(std::chrono::nanoseconds duration);
std::chrono::nanoseconds XPathExecuteDuration();
void InitDebug();
void DoneDebug();

} } // namespace gendoc::xpath

#endif // GENDOC_XPATH_XPATH_DEBUG
