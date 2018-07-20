// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_XPATH_XPATH_CONTEXT
#define GENDOC_XPATH_XPATH_CONTEXT
#include <gendoc/dom/Node.hpp>

namespace gendoc { namespace xpath {

class XPathContext
{
public:
    XPathContext(gendoc::dom::Node* node_, int position_, int size_);
    gendoc::dom::Node* Node() const { return node; }
    int Position() const { return position; }
    int Size() const { return size; }
private:
    gendoc::dom::Node* node;
    int position;
    int size;
};

} } // namespace gendoc::xpath

#endif // GENDOC_XPATH_XPATH_CONTEXT
