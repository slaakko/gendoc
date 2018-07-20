// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_GRAMMAR_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_GRAMMAR_SYMBOL_INCLUDED
#include <gendoc/cppsym/ContainerSymbol.hpp>
#include <gendoc/cppsym/RuleSymbol.hpp>

namespace gendoc { namespace cppsym {

class GrammarSymbol : public ContainerSymbol
{
public:
    GrammarSymbol(const Span& span_, const std::u32string& name_);
    void AddMember(std::unique_ptr<Symbol>&& member) override;
    std::u32string KindStr() override;
    std::unique_ptr<dom::Element> CreateElement() override;
    bool IsGrammarSymbol() const override { return true; }
private:
    std::vector<RuleSymbol*> rules;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_GRAMMAR_SYMBOL_INCLUDED
