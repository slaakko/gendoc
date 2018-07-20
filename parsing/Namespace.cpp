// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/parsing/Namespace.hpp>
#include <gendoc/parsing/Scope.hpp>
#include <gendoc/parsing/Visitor.hpp>

namespace gendoc { namespace parsing {

Namespace::Namespace(const std::u32string& name_, Scope* enclosingScope_) : ParsingObject(name_, enclosingScope_, ObjectKind::ns)
{
    SetScope(new Scope(Name(), EnclosingScope())); 
}

void Namespace::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    GetScope()->Accept(visitor);
    visitor.EndVisit(*this);
}

UsingObject::UsingObject(gendoc::codedom::UsingObject* subject_, Scope* enclosingScope_) : 
    ParsingObject(subject_->Name(), enclosingScope_, ObjectKind::usingObject), subject(subject_)
{
    if (!subject->IsOwned())
    {
        subject->SetOwned();
        ownedSubject.reset(subject);
    }
}

void UsingObject::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace gendoc::parsing
