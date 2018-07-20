// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/EnumerationSymbol.hpp>
#include <gendoc/util/Unicode.hpp>

namespace gendoc { namespace cppsym {

using namespace gendoc::unicode;

EnumTypeSymbol::EnumTypeSymbol(const Span& span_, const std::u32string& name_, EnumKey enumKey_) : TypeSymbol(span_, name_), enumKey(enumKey_), baseType(nullptr)
{
}

std::unique_ptr<dom::Element> EnumTypeSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> enumTypeElement(new dom::Element(U"enumType"));
    enumTypeElement->SetAttribute(U"key", ToString(enumKey));
    if (baseType)
    {
        enumTypeElement->SetAttribute(U"baseType", baseType->Id());
    }
    if (GetSpan().Valid())
    {
        enumTypeElement->SetAttribute(U"definitionFile", ToUtf32(fileName));
        enumTypeElement->SetAttribute(U"definitionLine", ToUtf32(std::to_string(GetSpan().LineNumber())));
        enumTypeElement->SetAttribute(U"definitionFileId", fileId);
    }
    return enumTypeElement;
}

EnumeratorSymbol::EnumeratorSymbol(const Span& span_, const std::u32string& name_, const std::u32string& value_) : Symbol(span_, name_), value(value_)
{
}

std::unique_ptr<dom::Element> EnumeratorSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> enumeratorElement(new dom::Element(U"enumerator"));
    enumeratorElement->SetAttribute(U"value", value);
    return enumeratorElement;
}

} } // namespace gendoc::cppsym
