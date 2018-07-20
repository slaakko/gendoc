// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_DOM_DOCUMENT_FRAGMENT_INCLUDED
#define GENDOC_DOM_DOCUMENT_FRAGMENT_INCLUDED
#include <gendoc/dom/Node.hpp>

namespace gendoc { namespace dom {

class DocumentFragment : public ParentNode
{
public:
    DocumentFragment();
    DocumentFragment(const DocumentFragment&) = delete;
    DocumentFragment& operator=(const DocumentFragment&) = delete;
    DocumentFragment(DocumentFragment&&) = delete;
    DocumentFragment& operator=(DocumentFragment&&) = delete;
    std::unique_ptr<Node> CloneNode(bool deep) override;
};

} } // namespace gendoc::dom

#endif // GENDOC_DOM_DOCUMENT_FRAGMENT_INCLUDED
