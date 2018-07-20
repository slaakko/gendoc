// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/GrammarSymbol.hpp>

namespace gendoc { namespace cppsym {

GrammarSymbol::GrammarSymbol(const Span& span_, const std::u32string& name_) : ContainerSymbol(span_, name_)
{
    SetAccess(Specifier::public_);
}

std::u32string GrammarSymbol::KindStr()
{
    return U"grammar";
}

void GrammarSymbol::AddMember(std::unique_ptr<Symbol>&& member)
{
    Symbol* m = member.get();
    ContainerSymbol::AddMember(std::move(member));
    if (m->IsRuleSymbol())
    {
        rules.push_back(static_cast<RuleSymbol*>(m));
    }
}

std::unique_ptr<dom::Element> GrammarSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> grammarElement(new dom::Element(U"grammar"));
    if (!rules.empty())
    {
        std::unique_ptr<dom::Element> rulesElement(new dom::Element(U"rules"));
        for (RuleSymbol* ruleSymbol : rules)
        {
            std::unique_ptr<dom::Element> ruleElement(ruleSymbol->ToDomElement());
            rulesElement->AppendChild(std::unique_ptr<dom::Node>(ruleElement.release()));
        }
        grammarElement->AppendChild(std::unique_ptr<dom::Node>(rulesElement.release()));
    }
    return grammarElement;
}

} } // namespace gendoc::cppsym
