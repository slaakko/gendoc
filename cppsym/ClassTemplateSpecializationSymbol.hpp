// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_CLASS_TEMPLATE_SPECIALIZATION_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_CLASS_TEMPLATE_SPECIALIZATION_SYMBOL_INCLUDED
#include <gendoc/cppsym/ClassTypeSymbol.hpp>
#include <gendoc/cppast/Template.hpp>

namespace gendoc { namespace cppsym {

std::u32string MakeClassTemplateSpecializationId(TypeSymbol* primaryClassTemplateSymbol, const std::vector<TypeSymbol*>& templateArgumentSymbols);
std::u32string MakeClassTemplateSpecializationName(TypeSymbol* primaryClassTemplateSymbol, const std::vector<TypeSymbol*>& templateArgumentSymbols);

class ClassTemplateSpecializationSymbol : public ClassTypeSymbol
{
public:
    ClassTemplateSpecializationSymbol(const Span& span_, const std::u32string& name_, TypeSymbol* primaryClassTemplateSymbol_, TemplateIdNode* templateIdNode_,
        const std::vector<TypeSymbol*>& templateArgumentSymbols_, const std::vector<Node*>& templateArgumentNodes_, const std::u32string& id_);
    bool IsClassTemplateSpecializationSymbol() const override { return true; }
    bool HasBaseClass(TypeSymbol* type, int& distance) override;
    ClassKey GetClassKey() const override { return primaryClassTemplateSymbol->GetClassKey(); }
    TypeSymbol* PrimaryClassTemplateSymbol() { return primaryClassTemplateSymbol; }
    TemplateIdNode* GetTemplateIdNode() { return templateIdNode; }
    const std::vector<TypeSymbol*>& TemplateArgumentSymbols() const { return templateArgumentSymbols; }
    const std::vector<Node*>& TemplateArgumentNodes() const { return templateArgumentNodes; }
    std::unique_ptr<dom::Element> CreateElement() override;
    std::u32string Id() override { return id; }
private:
    std::u32string id;
    TypeSymbol* primaryClassTemplateSymbol;
    TemplateIdNode* templateIdNode;
    std::vector<TypeSymbol*> templateArgumentSymbols;
    std::vector<Node*> templateArgumentNodes;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_CLASS_TEMPLATE_SPECIALIZATION_SYMBOL_INCLUDED
