// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/xpath/XPathContext.hpp>

namespace gendoc { namespace xpath {

XPathContext::XPathContext(gendoc::dom::Node* node_, int position_, int size_) : node(node_), position(position_), size(size_)
{
}

} } // namespace gendoc::xpath
