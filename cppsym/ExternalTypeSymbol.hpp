// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_EXTERNAL_TYPE_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_EXTERNAL_TYPE_SYMBOL_INCLUDED
#include <gendoc/cppsym/TypeSymbol.hpp>
#include <gendoc/cppast/Class.hpp>

namespace gendoc { namespace cppsym {

class ExternalTypeSymbol : public TypeSymbol
{
public:
    ExternalTypeSymbol(const Span& span_, const std::u32string& name_, ClassKey classKey_, const std::u32string& id_);
    bool IsExternalTypeSymbol() const override { return true; }
    ClassKey GetClassKey() const { return classKey; }
    std::unique_ptr<dom::Element> CreateElement() override;
    std::u32string Id() override { return id; }
private:
    ClassKey classKey;
    std::u32string id;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_EXTERNAL_TYPE_SYMBOL_INCLUDED
