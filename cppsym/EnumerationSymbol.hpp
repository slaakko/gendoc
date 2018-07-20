// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_ENUMERATION_SYMBOL_INCLUDED
#define GENDOC_CPPSYM_ENUMERATION_SYMBOL_INCLUDED
#include <gendoc/cppsym/TypeSymbol.hpp>
#include <gendoc/cppast/Enumeration.hpp>

namespace gendoc { namespace cppsym {

using namespace gendoc::cppast;

class EnumTypeSymbol : public TypeSymbol
{
public:
    EnumTypeSymbol(const Span& span_, const std::u32string& name_, EnumKey enumKey_);
    std::u32string KindStr() override { return U"enum"; }
    std::unique_ptr<dom::Element> CreateElement() override;
    bool IsEnumTypeSymbol() const override { return true; }
    bool IsParentSymbol() const override { return true; }
    TypeSymbol* BaseType() { return baseType; }
    void SetBaseType(TypeSymbol* baseType_) { baseType = baseType_; }
    void SetFileId(const std::u32string& fileId_) { fileId = fileId_; }
    const std::u32string& FileId() const { return fileId; }
    void SetFileName(const std::string& fileName_) { fileName = fileName_; }
    const std::string& FileName() const { return fileName; }
private:
    EnumKey enumKey;
    TypeSymbol* baseType;
    std::u32string fileId;
    std::string fileName;
};

class EnumeratorSymbol : public Symbol
{
public:
    EnumeratorSymbol(const Span& span_, const std::u32string& name_, const std::u32string& value_);
    TypeSymbol* GetType() override { return Parent()->GetType(); }
    std::u32string KindStr() override { return U"enumerator"; }
    std::unique_ptr<dom::Element> CreateElement() override;
    bool IsEnumeratorSymbol() const override { return true; }
    const std::u32string& Value() const { return value; }
private:
    std::u32string value;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_ENUMERATION_SYMBOL_INCLUDED
