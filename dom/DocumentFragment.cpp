// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/dom/DocumentFragment.hpp>

namespace gendoc { namespace dom {

DocumentFragment::DocumentFragment() : ParentNode(NodeType::documentFragmentNode, U"document_fragment")
{
}

std::unique_ptr<Node> DocumentFragment::CloneNode(bool deep) 
{
    std::unique_ptr<Node> clonedDocumentFragment(new DocumentFragment());
    if (deep)
    {
        ParentNode* parentNode = static_cast<ParentNode*>(clonedDocumentFragment.get());
        CloneChildrenTo(parentNode);
    }
    return clonedDocumentFragment;
}

} } // namespace gendoc::dom
