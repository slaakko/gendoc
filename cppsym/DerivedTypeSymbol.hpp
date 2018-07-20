// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_DERIVED_TYPE_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_DERIVED_TYPE_SYMBOL_INCLUDED
#include <gendoc/cppsym/TypeSymbol.hpp>

namespace gendoc { namespace cppsym {

enum class Derivation : uint8_t
{
    const_, volatile_, lvalueRef, rvalueRef, ptr, base
};

std::vector<Derivation> UnifyDerivations(const std::vector<Derivation>& left, const std::vector<Derivation>& right);

std::u32string MakeDerivedTypeId(const std::vector<Derivation>& derivations, TypeSymbol* baseType);
std::u32string MakeDerivedTypeName(const std::vector<Derivation>& derivations, TypeSymbol* baseType);

class DerivedTypeSymbol : public TypeSymbol
{
public:
    DerivedTypeSymbol(const Span& span_, const std::u32string& name_, TypeSymbol* baseType_, const std::vector<Derivation>& derivations_, const std::u32string& id_);
    const std::vector<Derivation>& Derivations() const { return derivations; }
    int MatchValue(TypeSymbol* argumentType) override;
    TypeSymbol* BaseType() override { return baseType; }
    bool IsDerivedTypeSymbol() const override { return true; }
    std::unique_ptr<dom::Element> CreateElement() override;
    std::u32string Id() override { return id; }
private:
    TypeSymbol* baseType;
    std::vector<Derivation> derivations;
    std::u32string id;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_DERIVED_TYPE_SYMBOL_INCLUDED
