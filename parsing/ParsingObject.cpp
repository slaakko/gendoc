// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/parsing/ParsingObject.hpp>
#include <gendoc/parsing/Scope.hpp>
#include <gendoc/parsing/Exception.hpp>

namespace gendoc { namespace parsing {

ParsingObject::ParsingObject(const std::u32string& name_, ObjectKind kind_) : kind(kind_), name(name_), owner(external), isOwned(false), enclosingScope(nullptr), scope(nullptr)
{
}

ParsingObject::ParsingObject(const std::u32string& name_, Scope* enclosingScope_, ObjectKind kind_) : kind(kind_), name(name_), owner(external), isOwned(false), enclosingScope(enclosingScope_), scope(nullptr)
{
}

ParsingObject::~ParsingObject()
{
}

std::u32string ParsingObject::FullName() const
{
    std::u32string scopeName = U"";
    if (enclosingScope)
    {
        scopeName = enclosingScope->FullName();
    }
    if (scopeName.empty())
    {
        return name;
    }
    return scopeName + U"." + name;
}

void ParsingObject::Own(ParsingObject* object)
{
    if (object)
    {
        if (!object->IsOwned())
        {
            object->SetOwned();
            ownedObjects.push_back(std::unique_ptr<ParsingObject>(object));
        }
    }
}

void ParsingObject::SetScope(Scope* scope_)
{
    scope = scope_;
    Own(scope);
}

void ParsingObject::AddToScope()
{
    if (enclosingScope)
    {
        try
        {
            enclosingScope->Add(this);
        }
        catch (std::exception& ex)
        {
            ThrowException(ex.what(), span);
        }
        if (scope)
        {
            scope->SetEnclosingScope(enclosingScope);
        }
    }
}

} } // namespace gendoc::parsing

