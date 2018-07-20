// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_RULE_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_RULE_SYMBOL_INCLUDED
#include <gendoc/cppsym/Symbol.hpp>

namespace gendoc { namespace cppsym {

class RuleSymbol : public Symbol
{
public:
    RuleSymbol(const Span& span_, const std::u32string& name_);
    std::u32string KindStr() override;
    std::unique_ptr<dom::Element> CreateElement() override;
    bool IsRuleSymbol() const override { return true; }
    dom::Element* Body() { return ruleBody.get(); }
private:
    std::unique_ptr<dom::Element> ruleBody;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_RULE_SYMBOL_INCLUDED
