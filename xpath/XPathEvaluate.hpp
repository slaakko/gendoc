// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_XPATH_XPATH_EVALUATE
#define GENDOC_XPATH_XPATH_EVALUATE
#include <gendoc/xpath/XPathObject.hpp>

namespace gendoc { namespace xpath {

std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, gendoc::dom::Node* node);
std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, gendoc::dom::Document* document);

} } // namespace gendoc::xpath

#endif // GENDOC_XPATH_XPATH_EVALUATE
