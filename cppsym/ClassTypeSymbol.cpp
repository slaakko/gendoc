// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/ClassTypeSymbol.hpp>
#include <gendoc/cppsym/TemplateSymbol.hpp>
#include <gendoc/util/Unicode.hpp>
#include <algorithm>

namespace gendoc { namespace cppsym {

using namespace gendoc::unicode;

ClassTypeSymbol::ClassTypeSymbol(const Span& span_, const std::u32string& name_, ClassKey classKey_) : TypeSymbol(span_, name_), classKey(classKey_), isAbstract(false)
{
}

bool ClassTypeSymbol::HasBaseClass(TypeSymbol* type, int& distance)
{
    int n = baseClasses.size();
    for (int i = 0; i < n; ++i)
    {
        TypeSymbol* baseClass = baseClasses[i];
        if (baseClass == type)
        {
            ++distance;
            return true;
        }
        if (baseClass->HasBaseClass(type, distance))
        {
            ++distance;
            return true;
        }
    }
    return false;
}

int ClassTypeSymbol::MatchValue(TypeSymbol* argumentType)
{
    int parentMatchValue = TypeSymbol::MatchValue(argumentType);
    if (parentMatchValue == std::numeric_limits<int>::max())
    {
        return parentMatchValue; // exact match
    }
    if (argumentType != nullptr)
    {
        int distance = 1;
        if (argumentType->BaseType()->HasBaseClass(this, distance))
        {
            return distance;
        }
    }
    if (parentMatchValue > 0)
    {
        return parentMatchValue;
    }
    return 0;
}

void ClassTypeSymbol::AddTemplateParameter(std::unique_ptr<TypeSymbol>&& templateParameter)
{
    templateParameters.push_back(std::move(templateParameter));
}

ContainerSymbol* ClassTypeSymbol::GetBase(int i) const
{
    return baseClasses[i];
}

void ClassTypeSymbol::AddBaseClass(TypeSymbol* baseClass)
{
    baseClasses.push_back(baseClass);
}

void ClassTypeSymbol::AddDerivedClass(TypeSymbol* derivedClass)
{
    derivedClasses.push_back(derivedClass);
}

std::u32string ClassTypeSymbol::IdStr()
{
    std::u32string idStr = FullName();
    for (const std::unique_ptr<TypeSymbol>& templateParameter : templateParameters)
    {
        idStr.append(1, '_').append(templateParameter->Name());
    }
    return idStr;
}

std::u32string ClassTypeSymbol::KindStr()
{
    return ToString(classKey);
}

std::unique_ptr<dom::Element> ClassTypeSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> classElement(new dom::Element(U"class"));
    classElement->SetAttribute(U"key", gendoc::cppast::ToString(classKey));
    if (!templateParameters.empty())
    {
        std::unique_ptr<dom::Element> templateParametersElement(new dom::Element(U"templateParameters"));
        for (const std::unique_ptr<TypeSymbol>& templateParameter : templateParameters)
        {
            std::unique_ptr<dom::Element> templateParameterElement = templateParameter->ToDomElement();
            templateParametersElement->AppendChild(std::unique_ptr<dom::Node>(templateParameterElement.release()));
        }
        classElement->AppendChild(std::unique_ptr<dom::Node>(templateParametersElement.release()));
    }
    if (!baseClasses.empty())
    {
        std::unique_ptr<dom::Element> baseClassesElement(new dom::Element(U"baseClasses"));
        std::sort(baseClasses.begin(), baseClasses.end(), ByName());
        for (TypeSymbol* baseClass : baseClasses)
        {
            std::unique_ptr<dom::Element> baseClassElement(new dom::Element(U"baseClass"));
            baseClassElement->SetAttribute(U"baseClassId", baseClass->Id());
            baseClassesElement->AppendChild(std::unique_ptr<dom::Node>(baseClassElement.release()));
        }
        classElement->AppendChild(std::unique_ptr<dom::Node>(baseClassesElement.release()));
    }
    if (!derivedClasses.empty())
    {
        std::unique_ptr<dom::Element> derivedClassesElement(new dom::Element(U"derivedClasses"));
        std::sort(derivedClasses.begin(), derivedClasses.end(), ByName());
        for (TypeSymbol* derivedClass : derivedClasses)
        {
            std::unique_ptr<dom::Element> derivedClassElement(new dom::Element(U"derivedClass"));
            derivedClassElement->SetAttribute(U"derivedClassId", derivedClass->Id());
            derivedClassesElement->AppendChild(std::unique_ptr<dom::Node>(derivedClassElement.release()));
        }
        classElement->AppendChild(std::unique_ptr<dom::Node>(derivedClassesElement.release()));
    }
    if (GetSpan().Valid())
    {
        classElement->SetAttribute(U"definitionFile", ToUtf32(fileName));
        classElement->SetAttribute(U"definitionLine", ToUtf32(std::to_string(GetSpan().LineNumber())));
        classElement->SetAttribute(U"definitionFileId", fileId);
    }
    if (isAbstract)
    {
        classElement->SetAttribute(U"abstract", U"true");
    }
    return classElement;
}

void ClassTypeSymbol::AddVirtualFunction(FunctionSymbol* virtualFunction)
{
    virtualFunctions.push_back(virtualFunction);
}

ElaborateClassTypeSymbol::ElaborateClassTypeSymbol(const Span& span_, ClassKey classKey_, TypeSymbol* classType_, const std::u32string& id_) :
    TypeSymbol(span_, classType_->Name()), classKey(classKey_), classType(classType_), id(id_)
{
}

std::unique_ptr<dom::Element> ElaborateClassTypeSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> elaborateClassElement(new dom::Element(U"elaborate_class"));
    return elaborateClassElement;
}

ClassGroupSymbol::ClassGroupSymbol(const Span& span_, const std::u32string& name_) : Symbol(span_, name_)
{
}

void ClassGroupSymbol::AddClass(std::unique_ptr<ClassTypeSymbol>&& cls)
{
    cls->SetParent(Parent());
    arityClassMap[cls->Arity()] = cls.get();
    classes.push_back(std::move(cls));
}

ClassTypeSymbol* ClassGroupSymbol::GetClass(int arity) const
{
    auto it = arityClassMap.find(arity);
    if (it != arityClassMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

std::unique_ptr<dom::Element> ClassGroupSymbol::CreateElement()
{
    return std::unique_ptr<dom::Element>(new dom::Element(U"class_group"));
}

ClassGroupTypeSymbol::ClassGroupTypeSymbol(ClassGroupSymbol* classGroup_) : TypeSymbol(classGroup_->GetSpan(), classGroup_->Name()), classGroup(classGroup_)
{
}

} } // namespace gendoc::cppsym