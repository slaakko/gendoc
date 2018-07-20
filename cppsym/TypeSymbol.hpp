// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_TYPE_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_TYPE_SYMBOL_INCLUDED
#include <gendoc/cppsym/ContainerSymbol.hpp>
#include <gendoc/cppast/Class.hpp>

namespace gendoc { namespace cppsym {

using namespace gendoc::cppast;

class TypeSymbol : public ContainerSymbol
{
public:
    TypeSymbol(const Span& span_, const std::u32string& name_);
    bool IsTypeSymbol() const override { return true; }
    TypeSymbol* GetType() override { return this; }
    virtual TypeSymbol* BaseType() { return this; }
    virtual bool HasBaseClass(TypeSymbol* type, int& distance);
    virtual int MatchValue(TypeSymbol* argumentType);
    virtual bool IsSimpleTypeSymbol() const { return false; }
    virtual bool IsElaborateClassTypeSymbol() const { return false; }
    virtual bool IsDerivedTypeSymbol() const { return false; }
    virtual bool IsExternalTypeSymbol() const { return false; }
    virtual ClassKey GetClassKey() const { return ClassKey::class_; }
    std::u32string KindStr() override { return U"type";  }
    std::unique_ptr<dom::Element> CreateElement() override;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_TYPE_SYMBOL_INCLUDED
