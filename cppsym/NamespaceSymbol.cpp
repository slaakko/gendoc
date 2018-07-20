// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/NamespaceSymbol.hpp>

namespace gendoc { namespace cppsym {

NamespaceSymbol::NamespaceSymbol(const Span& span_, const std::u32string& name_) : ContainerSymbol(span_, name_)
{
}

std::unique_ptr<dom::Element> NamespaceSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> namespaceElement(new dom::Element(U"namespace"));
    if (!projects.empty())
    {
        std::unique_ptr<dom::Element> projectsElement(new dom::Element(U"projects"));
        for (const std::u32string& projectName : projects)
        {
            std::unique_ptr<dom::Element> projectElement(new dom::Element(U"project"));
            projectElement->SetAttribute(U"name", projectName);
            projectsElement->AppendChild(std::unique_ptr<dom::Node>(projectElement.release()));
        }
        namespaceElement->AppendChild(std::unique_ptr<dom::Node>(projectsElement.release()));
    }
    return namespaceElement;
}

void NamespaceSymbol::AddProject(const std::u32string& projectName)
{
    if (projectName == U"binder")
    {
        int x = 0;
    }
    projects.insert(projectName);
}

} } // namespace gendoc::cppsym
