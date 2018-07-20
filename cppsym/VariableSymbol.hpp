// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_VARIABLE_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_VARIABLE_SYMBOL_INCLUDED
#include <gendoc/cppsym/TypeSymbol.hpp>

namespace gendoc { namespace cppsym {

class VariableSymbol : public Symbol
{
public:
    VariableSymbol(const Span& span_, const std::u32string& name_);
    std::u32string KindStr() override { return U"variable"; }
    std::unique_ptr<dom::Element> CreateElement() override;
    bool IsVariableSymbol() const override { return true; }
    void AddSpecifiers(Specifier specifiers_);
    Specifier Specifiers() const { return specifiers; }
    TypeSymbol* GetType() override { return type; }
    void SetType(TypeSymbol* type_) { type = type_; }
private:
    Specifier specifiers;
    TypeSymbol * type;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_VARIABLE_SYMBOL_INCLUDED
