// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_PARAMETER_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_PARAMETER_SYMBOL_INCLUDED
#include <gendoc/cppsym/Symbol.hpp>

namespace gendoc { namespace cppsym {

class TypeSymbol;

class ParameterSymbol : public Symbol
{
public:
    ParameterSymbol(const Span& span_, const std::u32string& name_);
    std::u32string KindStr() override { return U"parameter"; }
    std::u32string IdStr() override;
    std::unique_ptr<dom::Element> CreateElement() override;
    bool IsParameterSymbol() const override { return true; }
    TypeSymbol* GetType() override { return type; }
    void SetType(TypeSymbol* type_) { type = type_; }
private:
    TypeSymbol* type;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_PARAMETER_SYMBOL_INCLUDED
