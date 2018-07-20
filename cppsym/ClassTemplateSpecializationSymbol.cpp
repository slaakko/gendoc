// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/ClassTemplateSpecializationSymbol.hpp>

namespace gendoc { namespace cppsym {

std::u32string MakeClassTemplateSpecializationId(TypeSymbol* primaryClassTemplateSymbol, const std::vector<TypeSymbol*>& templateArgumentSymbols)
{
    std::u32string id = primaryClassTemplateSymbol->Id();
    for (TypeSymbol* templateArgument : templateArgumentSymbols)
    {
        id.append(U".").append(templateArgument->Id());
    }
    return id;
}

std::u32string MakeClassTemplateSpecializationName(TypeSymbol* primaryClassTemplateSymbol, const std::vector<TypeSymbol*>& templateArgumentSymbols)
{
    std::u32string name = primaryClassTemplateSymbol->Name();
    name.append(1, '<');
    bool first = true;
    for (TypeSymbol* templateArgument : templateArgumentSymbols)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            name.append(U", ");
        }
        name.append(templateArgument->Name());
    }
    name.append(1, '>');
    return name;
}

ClassTemplateSpecializationSymbol::ClassTemplateSpecializationSymbol(const Span& span_, const std::u32string& name_, TypeSymbol* primaryClassTemplateSymbol_,
    TemplateIdNode* templateIdNode_, const std::vector<TypeSymbol*>& templateArgumentSymbols_, const std::vector<Node*>& templateArgumentNodes_, const std::u32string& id_) :
    ClassTypeSymbol(span_, name_, primaryClassTemplateSymbol_->GetClassKey()), primaryClassTemplateSymbol(primaryClassTemplateSymbol_), templateIdNode(templateIdNode_), 
    templateArgumentSymbols(templateArgumentSymbols_), templateArgumentNodes(templateArgumentNodes_), id(id_)
{
}

bool ClassTemplateSpecializationSymbol::HasBaseClass(TypeSymbol* type, int& distance)
{
    return primaryClassTemplateSymbol->HasBaseClass(type, distance);
}

std::unique_ptr<dom::Element> ClassTemplateSpecializationSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> typeElement(new dom::Element(U"specialization"));
    typeElement->SetAttribute(U"primaryTypeId", primaryClassTemplateSymbol->Id());
    std::unique_ptr<dom::Element> templateArgumentsElement(new dom::Element(U"templateArguments"));
    for (TypeSymbol* templateArgument : templateArgumentSymbols)
    {
        std::unique_ptr<dom::Element> templateArgumentElement(new dom::Element(U"templateArgument"));
        templateArgumentElement->SetAttribute(U"typeId", templateArgument->Id());
        templateArgumentsElement->AppendChild(std::unique_ptr<dom::Node>(templateArgumentElement.release()));
    }
    typeElement->AppendChild(std::unique_ptr<dom::Node>(templateArgumentsElement.release()));
    return typeElement;
}

} } // namespace gendoc::cppsym