// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/Symbol.hpp>
#include <gendoc/cppsym/ContainerSymbol.hpp>
#include <gendoc/cppsym/ClassTypeSymbol.hpp>
#include <gendoc/util/Sha1.hpp>
#include <gendoc/util/Unicode.hpp>

namespace gendoc { namespace cppsym {

using namespace gendoc::unicode;
using namespace gendoc::util;

std::u32string ToString(SymbolAccess access)
{
    switch (access)
    {
        case SymbolAccess::public_: return U"public";
        case SymbolAccess::protected_: return U"protected";
        case SymbolAccess::private_: return U"private";
    }
    return std::u32string();
}

Symbol::Symbol(const Span& span_, const std::u32string& name_) : span(span_), name(name_), parent(nullptr), access(SymbolAccess::private_)
{
}

Symbol::~Symbol()
{
}

std::u32string Symbol::FullName() 
{
    std::u32string fullName;
    if (Parent())
    {
        fullName = Parent()->FullName();
    }
    if (fullName.empty())
    {
        return Name();
    }
    else
    {
        fullName.append(U"::").append(Name());
        return fullName;
    }
}

void Symbol::SetAccess(Specifier accessSpecifier)
{
    if ((accessSpecifier & Specifier::public_) != Specifier::none)
    {
        access = SymbolAccess::public_;
    }
    else if ((accessSpecifier & Specifier::protected_) != Specifier::none)
    {
        access = SymbolAccess::protected_;
    }
    else if ((accessSpecifier & Specifier::private_) != Specifier::none)
    {
        access = SymbolAccess::private_;
    }
}

std::u32string Symbol::IdStr()
{
    return FullName();
}

std::u32string Symbol::Id()
{
    std::u32string id = KindStr();
    id.append(1, '_').append(SimpleName()).append(1, '_').append(ToUtf32(GetSha1MessageDigest(ToUtf8(IdStr()))));
    return id;
}

std::unique_ptr<dom::Element> Symbol::ToDomElement()
{
    std::unique_ptr<dom::Element> element = CreateElement();
    element->SetAttribute(U"name", Name());
    element->SetAttribute(U"id", Id());
    element->SetAttribute(U"access", ToString(access));
    std::unique_ptr<dom::Element> namespaces = Namespaces();
    if (namespaces)
    {
        element->AppendChild(std::unique_ptr<dom::Node>(namespaces.release()));
    }
    std::unique_ptr<dom::Element> classes = Classes();
    if (classes)
    {
        element->AppendChild(std::unique_ptr<dom::Node>(classes.release()));
    }
    std::unique_ptr<dom::Element> constructors = Constructors();
    if (constructors)
    {
        element->AppendChild(std::unique_ptr<dom::Node>(constructors.release()));
    }
    std::unique_ptr<dom::Element> functions = Functions();
    if (functions)
    {
        element->AppendChild(std::unique_ptr<dom::Node>(functions.release()));
    }
    std::unique_ptr<dom::Element> enumTypes = EnumTypes();
    if (enumTypes)
    {
        element->AppendChild(std::unique_ptr<dom::Node>(enumTypes.release()));
    }
    std::unique_ptr<dom::Element> enumerators = Enumerators();
    if (enumerators)
    {
        element->AppendChild(std::unique_ptr<dom::Node>(enumerators.release()));
    }
    std::unique_ptr<dom::Element> typedefs = Typedefs();
    if (typedefs)
    {
        element->AppendChild(std::unique_ptr<dom::Node>(typedefs.release()));
    }
    std::unique_ptr<dom::Element> variables = Variables();
    if (variables)
    {
        element->AppendChild(std::unique_ptr<dom::Node>(variables.release()));
    }
    std::unique_ptr<dom::Element> grammars = Grammars();
    if (grammars)
    {
        element->AppendChild(std::unique_ptr<dom::Node>(grammars.release()));
    }
    return element;
}

std::unique_ptr<dom::Element> Symbol::Namespaces()
{
    return std::unique_ptr<dom::Element>();
}

std::unique_ptr<dom::Element> Symbol::Classes()
{
    return std::unique_ptr<dom::Element>();
}

std::unique_ptr<dom::Element> Symbol::Constructors()
{
    return std::unique_ptr<dom::Element>();
}

std::unique_ptr<dom::Element> Symbol::Functions()
{
    return std::unique_ptr<dom::Element>();
}

std::unique_ptr<dom::Element> Symbol::EnumTypes()
{
    return std::unique_ptr<dom::Element>();
}

std::unique_ptr<dom::Element> Symbol::Enumerators()
{
    return std::unique_ptr<dom::Element>();
}

std::unique_ptr<dom::Element> Symbol::Typedefs()
{
    return std::unique_ptr<dom::Element>();
}

std::unique_ptr<dom::Element> Symbol::Variables()
{
    return std::unique_ptr<dom::Element>();
}

std::unique_ptr<dom::Element> Symbol::Grammars()
{
    return std::unique_ptr<dom::Element>();
}

ClassTypeSymbol* Symbol::Class()
{
    if (IsClassTypeSymbol())
    {
        return static_cast<ClassTypeSymbol*>(this);
    }
    else if (parent)
    {
        return parent->Class();
    }
    else
    {
        return nullptr;
    }
}

} } // namespace gendoc::cppsym
